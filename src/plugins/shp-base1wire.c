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
 * This is a base class for plugins obtaining readings from a 1-Wire Serial
 * network
 *
 * It also implements the 1Wire interface which allows handling 1Wire supported
 * networks in an unified manner.
 *
 * This is the configuration specification for this plugin:
 *
 * Config spec:
 *
 * Properties inherited from baseplugin:
 *
 * interval=<integer>    Optional
 *
 * Properties for this plugin:
 *
 * device-id=<string>    Optional
 * trigger=<double>      Optional
 * descending=<boolean>  Optional
 *
 * If 'trigger' is set, check_config () will succeed whenever sensor readings
 * match 'trigger' and 'descending'. If it is not set, check_config () will
 * always succeed and generate result of the following kind:
 *
 * SensorReading=<double>
 * ID=<string>
 *
 * This result can be used as input to other plugins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "../shp-data.h"
#include "../shp-plugin-factory.h"
#include "../shp-1wire.h"
#include "shp-base1wire.h"

#define NAME "base1wire"

#define NO_TRIGGER -256.0

#define DEFAULT_DEVICE_ID NULL
#define DEFAULT_TRIGGER NO_TRIGGER
#define DEFAULT_DESCENDING TRUE

enum
{
  PROP_0,
  PROP_DEVICE_ID,
  PROP_TRIGGER,
  PROP_DESCENDING,
  PROP_LAST
};

static void shp_base1wire_interface_init (Shp1WireInterface * iface);
static gfloat read_sensor_data (Shp1Wire * wire, const gchar * id);
static void shp_base1wire_finalize (GObject * object);

static void shp_base1wire_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_base1wire_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static gboolean shp_base1wire_start (ShpPlugin * plugin);
static void shp_base1wire_stop (ShpPlugin * plugin);
static gboolean shp_base1wire_ready (ShpPlugin * plugin, ShpData ** result);

typedef struct _ShpSensorData ShpSensorData;
struct _ShpSensorData
{
  gchar* id;
  gfloat reading;
};

G_DEFINE_TYPE_WITH_CODE (ShpBase1wire, shp_base1wire,
    SHP_PLUGIN_TYPE, G_IMPLEMENT_INTERFACE (SHP_TYPE_1WIRE,
        shp_base1wire_interface_init));

static void
shp_base1wire_class_init (ShpBase1wireClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_base1wire_finalize;
  gobject_class->set_property = shp_base1wire_set_property;
  gobject_class->get_property = shp_base1wire_get_property;

  SHP_PLUGIN_CLASS (klass)->start = shp_base1wire_start;
  SHP_PLUGIN_CLASS (klass)->stop = shp_base1wire_stop;

  g_object_class_install_property (gobject_class, PROP_DEVICE_ID,
      g_param_spec_string ("device-id", "Device Id",
          "The Id of the 1Wire device to be read by the plugin",
          DEFAULT_DEVICE_ID, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_DESCENDING,
      g_param_spec_boolean ("descending", "Trigger is descending",
          "Whether plugin reacts to descending temperatures",
          DEFAULT_DESCENDING, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_TRIGGER,
      g_param_spec_double ("trigger", "Temperature reading",
          "Temperature activating plugin (-256 disabled)", -G_MAXDOUBLE,
          G_MAXDOUBLE, DEFAULT_TRIGGER, G_PARAM_READWRITE));
}

static void
shp_base1wire_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpBase1wire *self = SHP_BASE1WIRE (object);

  switch (propid) {
    case PROP_DEVICE_ID:
      g_value_take_string (value, g_strdup (self->device_id));
      break;
    case PROP_TRIGGER:
      g_value_set_double (value, self->trigger);
      break;
    case PROP_DESCENDING:
      g_value_set_boolean (value, self->descending);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_base1wire_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpBase1wire *self = SHP_BASE1WIRE (object);

  switch (propid) {
    case PROP_DEVICE_ID:
      g_free (self->device_id);
      self->device_id = g_strdup (g_value_get_string (value));
      break;
    case PROP_TRIGGER:
      self->trigger = g_value_get_double (value);
      break;
    case PROP_DESCENDING:
      self->descending = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_base1wire_init (ShpBase1wire * self)
{
  self->trigger = NO_TRIGGER;
  self->device_id = g_strdup (DEFAULT_DEVICE_ID);
}

static void
shp_base1wire_finalize (GObject * object)
{
  ShpBase1wire *self = SHP_BASE1WIRE (object);

  g_free (self->device_id);
}

static gboolean
dispatch_cb (gpointer user_data)
{
  ShpPlugin *plugin = SHP_PLUGIN (user_data);
  ShpData *data;

  if (shp_base1wire_ready (plugin, &data)) {
    shp_plugin_send (plugin, data);
  }

  return TRUE;
}

static gboolean
shp_base1wire_start (ShpPlugin * plugin)
{
  ShpBase1wire *self = SHP_BASE1WIRE (plugin);

  if (self->device_id == NULL) {
    g_debug ("no 'device-id' provided, switching to idle mode");
    return TRUE;
  }

  self->dispatch_source = g_timeout_source_new_seconds (15);
  g_source_set_callback (self->dispatch_source, dispatch_cb,
      g_object_ref (plugin), g_object_unref);
  g_source_attach (self->dispatch_source, NULL);

  return TRUE;
}

static void
shp_base1wire_stop (ShpPlugin * plugin)
{
  ShpBase1wire *self = SHP_BASE1WIRE (plugin);

  if (self->dispatch_source != NULL) {
    g_source_destroy (self->dispatch_source);
    g_source_unref (self->dispatch_source);
    self->dispatch_source = NULL;
  }
}

static void
shp_base1wire_interface_init (Shp1WireInterface * iface)
{
  iface->read_sensor_data = read_sensor_data;
}

static gfloat
read_sensor_data (Shp1Wire * wire, const gchar * id)
{
  ShpBase1wire *self = SHP_BASE1WIRE (wire);
  return SHP_BASE1WIRE_GET_CLASS (wire)->read_sensor_data (self, id);
}

static gboolean
shp_base1wire_ready (ShpPlugin * plugin, ShpData ** result)
{
  const gchar *id;
  gfloat trigger;
  gboolean descending;
  gfloat reading;
  ShpBase1wire *self = SHP_BASE1WIRE (plugin);

  g_debug ("base1wire: check");

  /* chain up to parent first */
  if (!shp_plugin_check (plugin)) {
    return FALSE;
  }

  if (self->device_id == NULL) {
    g_warning ("incomplete configuration, missing 'device-id'");
    return FALSE;
  }
  id = self->device_id;

  reading = read_sensor_data (SHP_1WIRE (plugin), id);
  if (reading == SHP_1WIRE_INVALID_READING) {
    g_warning ("invalid temperature reading");
    return FALSE;
  }

  if (self->trigger == NO_TRIGGER) {
    /* no trigger request, we are called to generate result */
    g_debug ("missing Descending or Trigger, success");
    if (result != NULL) {
      *result = shp_data_new ();
      shp_data_add_double (*result, "SensorReading", reading);
      shp_data_add_string (*result, "ID", plugin->id);
    }
    return TRUE;
  }

  descending = self->descending;
  trigger = self->trigger;

  if ((descending && reading < trigger) || (!descending && reading > trigger)) {
    /* trigger is met */

    if (!self->active) {
      self->active = TRUE;

      g_debug ("configuration matched, success");

      if (result != NULL) {
        *result = shp_data_new ();
        shp_data_add_double (*result, "SensorReading", reading);
        shp_data_add_string (*result, "ID", plugin->id);
      }

      return TRUE;
    }

    g_debug ("current trigger already signalled");
    return FALSE;
  }

  self->active = FALSE;
  return FALSE;
}
