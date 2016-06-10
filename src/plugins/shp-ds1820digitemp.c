/* The SmartHomeProject.org
 * Copyright (C) 2015  Ognyan Tonchev <otonchev at gmail.com >
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a plugin for obtaining readings from a 1-Wire Serial network
 * (digitemp needs to be installed and running).
 * The digitemp.conf must also be present in the 'config-dir' directory.
 *
 * digitemp.conf:
 *
 *	TTY /dev/ttyUSB0
 *	READ_TIME 1000
 *	LOG_TYPE 1
 *	LOG_FORMAT "Room %s: %.1C C"
 *	CNT_FORMAT "%b %d %H:%M:%S Sensor %s #%n %C"
 *	HUM_FORMAT "%b %d %H:%M:%S Sensor %s C: %.2C F: %.2F H: %h%%"
 *	SENSORS 5
 *	ROM 0 0x10 0x38 0x86 0x0A 0x02 0x08 0x00 0xC2
 *	ROM 1 0x10 0xEE 0x9F 0x74 0x02 0x08 0x00 0x86
 *	ROM 2 0x10 0x65 0x80 0x0A 0x02 0x08 0x00 0x3A
 *	ROM 3 0x10 0xD3 0xDB 0x74 0x02 0x08 0x00 0xD7
 *	ROM 4 0x10 0x07 0x77 0x0A 0x02 0x08 0x00 0x60
 *
 * This is the configuration specification for this plugin:
 *
 * Config spec:
 *
 * Properties inherited from baseplugin:
 *
 * device-id=<string>    Optional
 *
 * Properties for this plugin (Required):
 *
 * config-dir=<string>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "../shp-plugin-factory.h"

#include "shp-base1wire.h"
#include "shp-ds1820digitemp.h"

#define NAME "ds1820digitemp"

#define READ_INTERVAL 5*60 /* read sensor data every 5 minutes */

#define NO_TRIGGER -256.0

#define DEFAULT_CONFIG_DIR "./"

#define DIGITEMP_COMMAND_ALL \
    "digitemp_DS9097 -c %s/digitemp.conf -a -q"

enum
{
  PROP_0,
  PROP_CONFIG_DIR,
  PROP_LAST
};

static gboolean shp_ds1820digitemp_start (ShpComponent * component);
static gboolean shp_ds1820digitemp_stop (ShpComponent * component);
static gfloat shp_ds1820digitemp_read_sensor_data (ShpBase1wire * self,
    const gchar * id);
static GPtrArray* _read_sensors_data (ShpDs1820digitemp * self);
static void shp_ds1820digitemp_finalize (GObject * object);

static void shp_ds1820digitemp_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_ds1820digitemp_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct _ShpSensorData ShpSensorData;
struct _ShpSensorData
{
  gchar* id;
  gfloat reading;
};

G_DEFINE_TYPE (ShpDs1820digitemp, shp_ds1820digitemp, SHP_BASE1WIRE_TYPE);

static void
shp_ds1820digitemp_class_init (ShpDs1820digitempClass * klass)
{
  GObjectClass *gobject_class;
  ShpBase1wireClass *base1wire_class;

  gobject_class = G_OBJECT_CLASS (klass);
  base1wire_class = SHP_BASE1WIRE_CLASS (klass);

  gobject_class->finalize = shp_ds1820digitemp_finalize;
  gobject_class->set_property = shp_ds1820digitemp_set_property;
  gobject_class->get_property = shp_ds1820digitemp_get_property;

  base1wire_class->read_sensor_data = shp_ds1820digitemp_read_sensor_data;

  g_object_class_install_property (gobject_class, PROP_CONFIG_DIR,
      g_param_spec_string ("config-dir", "digitemp config dir",
          "Directory where digitemp.conf file is located", DEFAULT_CONFIG_DIR,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  SHP_COMPONENT_CLASS (klass)->stop = shp_ds1820digitemp_stop;
  SHP_COMPONENT_CLASS (klass)->start = shp_ds1820digitemp_start;

  g_mutex_init (&klass->mutex);
}

static void
shp_ds1820digitemp_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpDs1820digitemp *self = SHP_DS1820DIGITEMP (object);

  switch (propid) {
    case PROP_CONFIG_DIR:
      g_value_take_string (value, g_strdup (self->config_dir));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_ds1820digitemp_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpDs1820digitemp *self = SHP_DS1820DIGITEMP (object);

  switch (propid) {
    case PROP_CONFIG_DIR:
      g_free (self->config_dir);
      self->config_dir = g_strdup (g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

/* the main thread executing the source which reads sensor data at regular
 * intervals */
static gpointer
loop (gpointer data)
{
  ShpDs1820digitemp *temp = data;

  g_main_loop_run (temp->loop);
  g_object_unref (temp);

  return NULL;
}

/* read and store sensor data */
static gboolean
read_data_cb (gpointer data)
{
  ShpDs1820digitemp *self = SHP_DS1820DIGITEMP (data);
  ShpDs1820digitempClass *klass;
  glong ts_now;
  GTimeVal timeval;

  klass = SHP_DS1820DIGITEMP_GET_CLASS (self);

  g_get_current_time (&timeval);
  ts_now = timeval.tv_sec;

  g_mutex_lock (&klass->mutex);

  if (klass->ts != 0 && klass->ts + READ_INTERVAL > ts_now) {
    g_debug ("ds1820digitemp: sensor data up to date");
    g_mutex_unlock (&klass->mutex);
    return TRUE;
  }

  g_debug ("ds1820digitemp: obtaining sensor data");

  klass->ts = ts_now;
  if (klass->readings != NULL)
    g_ptr_array_unref (klass->readings);
  klass->readings = _read_sensors_data (self);

  g_mutex_unlock (&klass->mutex);

  return TRUE;
}

static void
shp_ds1820digitemp_init (ShpDs1820digitemp * self)
{
  self->config_dir = g_strdup (DEFAULT_CONFIG_DIR);
}

static void
shp_ds1820digitemp_finalize (GObject * object)
{
  ShpDs1820digitemp *self = SHP_DS1820DIGITEMP (object);
  g_free (self->config_dir);
}

static gboolean
shp_ds1820digitemp_start (ShpComponent * component)
{
  ShpDs1820digitemp *self = SHP_DS1820DIGITEMP (component);
  ShpDs1820digitempClass *klass;
  GSource *source;

  g_return_val_if_fail (IS_SHP_DS1820DIGITEMP (component), FALSE);

  g_debug ("ds1820digitemp: starting");

  /* chain up to parent first */
  if (!SHP_COMPONENT_CLASS (shp_ds1820digitemp_parent_class)->start (component)) {
    return FALSE;
  }

  klass = SHP_DS1820DIGITEMP_GET_CLASS (self);

  /* initial read of sensor data, the timeout source below will not be
      executed immediately */
  read_data_cb (self);
  g_mutex_lock (&klass->mutex);
  if (klass->readings == NULL) {
    g_warning ("unable to obtain initial readings");
    g_mutex_unlock (&klass->mutex);
    return FALSE;
  }
  g_mutex_unlock (&klass->mutex);

  self->context = g_main_context_new ();
  self->loop = g_main_loop_new (self->context, TRUE);
  self->thread = g_thread_new (NULL, loop, g_object_ref (self));

  /* add timeout source reading sensor data at regular intervals, this is
     necessary since reading sensors data using serial is very slow */
  source = g_timeout_source_new_seconds (READ_INTERVAL);
  g_source_set_callback (source, read_data_cb, g_object_ref (self),
      g_object_unref);
  g_source_attach (source, self->context);
  g_source_unref (source);

  return TRUE;
}

static gboolean
shp_ds1820digitemp_stop (ShpComponent * component)
{
  ShpDs1820digitemp * self = SHP_DS1820DIGITEMP (component);

  g_return_val_if_fail (IS_SHP_DS1820DIGITEMP (component), FALSE);

  /* chain up to parent first */
  SHP_COMPONENT_CLASS (shp_ds1820digitemp_parent_class)->stop (component);

  if (self->loop != NULL) {
    g_main_loop_quit (self->loop);
  }
  if (self->thread != NULL) {
    g_thread_join (self->thread);
    self->thread = NULL;
  }
  if (self->loop != NULL) {
    g_main_loop_unref (self->loop);
    self->loop = NULL;
    self->context = NULL;
  }

  return TRUE;
}

static void
free_sensor_data (gpointer data)
{
  ShpSensorData *sensor = data;
  g_free (sensor->id);
  g_free (sensor);
}

static gfloat
shp_ds1820digitemp_read_sensor_data (ShpBase1wire * basewire, const gchar * id)
{
  gfloat result = SHP_BASE1WIRE_INVALID_READING;
  guint idx;
  GPtrArray *readings;
  ShpSensorData *reading;
  ShpDs1820digitemp *self;
  ShpDs1820digitempClass *klass;

  g_return_val_if_fail (IS_SHP_DS1820DIGITEMP (basewire),
      SHP_BASE1WIRE_INVALID_READING);

  self = SHP_DS1820DIGITEMP (basewire);
  klass = SHP_DS1820DIGITEMP_GET_CLASS (self);

  g_debug ("ds1820digitemp: reading sensor data: %s", id);

  g_mutex_lock (&klass->mutex);

  readings = klass->readings;
  if (readings == NULL) {
    g_warning ("no readings found in cache, something broke?");
    g_mutex_unlock (&klass->mutex);
    return SHP_BASE1WIRE_INVALID_READING;
  }
  for (idx = 0; idx < readings->len; idx++) {
    reading = g_ptr_array_index (readings, idx);
    if (reading == NULL)
      break;
    g_debug ("ds1820digitemp: checking sensor %s", reading->id);
    if (!g_strcmp0 (reading->id, id)) {
      result = reading->reading;
      break;
    }
  }

  g_mutex_unlock (&klass->mutex);

  return result;
}

static GPtrArray*
_read_sensors_data (ShpDs1820digitemp * self)
{
  FILE *fp;
  gchar line[1035];
  GPtrArray *result = NULL;
  gchar *command;

  command = g_strdup_printf (DIGITEMP_COMMAND_ALL, self->config_dir);

  g_debug ("ds1820digitemp: reading sensors data. command: %s", command);

  /* open command for reading */
  fp = popen (command, "r");
  if (fp == NULL) {
    g_warning ("%s: error, failed to open command", NAME);
    g_free (command);
    return NULL;
  }

  g_free (command);

  /* read the output a line at a time */
  while (fgets (line, sizeof (line) - 1, fp) != NULL) {
    gint id;
    gfloat reading;
    ShpSensorData *data;

    if (sscanf (line, "%d %f", &id, &reading) != 2) {
      g_warning ("%s: error, failed to parse line: %s", NAME, line);
      continue;
    }

    data = g_new0 (ShpSensorData, 1);
    data->id = g_strdup_printf ("%d", id);
    data->reading = reading;

    if (result == NULL) {
      result = g_ptr_array_new_with_free_func (free_sensor_data);
    }

    g_ptr_array_add (result, data);
  }

  pclose (fp);
  return result;
}

static void
plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_DS1820DIGITEMP_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
