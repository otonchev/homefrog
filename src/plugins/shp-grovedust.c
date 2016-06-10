/* The SmartHomeProject.org
 * Copyright (C) 2016  Ognyan Tonchev <otonchev at gmail.com >
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
 * Config spec:
 *
 * Properties inherited from baseplugin:
 *
 * interval=<integer>    Optional
 *
 * Properties for this plugin:
 *
 * pin=<pin number>      Required
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "air_utils.h"
#include "lngpio.h"
#include "../shp-data.h"
#include "../shp-plugin-factory.h"
#include "../shp-1wire.h"
#include "shp-grovedust.h"

#define NAME "grovedust"

#define IN  0
#define LOW  0
#define HIGH 1

#define DEFAULT_PIN 4

enum
{
  PROP_0,
  PROP_PIN,
  PROP_LAST
};

static void shp_grovedust_finalize (GObject * object);

static void shp_grovedust_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_grovedust_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void shp_grovedust_interface_init (Shp1WireInterface * iface);
static gfloat read_sensor_data (Shp1Wire * wire, const gchar * id);
static gboolean shp_grovedust_check (ShpGrovedust * self, guint reading);
static gboolean shp_grovedust_start (ShpPlugin * plugin);
static void shp_grovedust_stop (ShpPlugin * plugin);

static unsigned long sampletime_ms = 30000; /* 30s */

G_DEFINE_TYPE_WITH_CODE (ShpGrovedust, shp_grovedust,
    SHP_PLUGIN_TYPE, G_IMPLEMENT_INTERFACE (SHP_TYPE_1WIRE,
        shp_grovedust_interface_init));

static void
shp_grovedust_class_init (ShpGrovedustClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_grovedust_finalize;
  gobject_class->set_property = shp_grovedust_set_property;
  gobject_class->get_property = shp_grovedust_get_property;

  g_object_class_install_property (gobject_class, PROP_PIN,
      g_param_spec_int ("pin", "Pin Number",
          "The number of the pin the dust sensor is connected to",
          4, 256, DEFAULT_PIN, G_PARAM_READWRITE));

  SHP_PLUGIN_CLASS (klass)->start = shp_grovedust_start;
  SHP_PLUGIN_CLASS (klass)->stop = shp_grovedust_stop;
}

static void
shp_grovedust_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpGrovedust *self = SHP_GROVEDUST (object);

  switch (propid) {
    case PROP_PIN:
      g_value_set_int (value, self->pin);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_grovedust_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpGrovedust *self = SHP_GROVEDUST (object);

  switch (propid) {
    case PROP_PIN:
      self->pin = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_grovedust_init (ShpGrovedust * self)
{
  self->pin = DEFAULT_PIN;
  g_mutex_init (&self->mutex);
}

static void
shp_grovedust_finalize (GObject * object)
{
  ShpGrovedust *self = SHP_GROVEDUST (object);
  g_mutex_clear (&self->mutex);
}

static void
shp_grovedust_interface_init (Shp1WireInterface * iface)
{
  iface->read_sensor_data = read_sensor_data;
}

static gfloat
read_sensor_data (Shp1Wire * wire, const gchar * id)
{
  ShpGrovedust *self = SHP_GROVEDUST (wire);
  gfloat result;

  g_mutex_lock (&self->mutex);
  result = (gfloat)self->last_reading;
  g_mutex_unlock (&self->mutex);

  return result;
}

static long
millis ()
{
  struct timeval  tv;
  gettimeofday (&tv, NULL);

  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

static void
pulse_detected (ShpGrovedust *self, unsigned long pulse_duration)
{
  self->lowpulseoccupancy = self->lowpulseoccupancy + pulse_duration;

  if ((millis () - self->starttime) > sampletime_ms) {
    float ratio;
    float concentration_pcs;
    float concentration_ugm3;
    int aqi;

    ratio = self->lowpulseoccupancy / (sampletime_ms * 10.0);
    concentration_pcs =
        1.1 * pow (ratio, 3) - 3.8 * pow (ratio, 2) + 520 * ratio + 0.62;
    concentration_ugm3 = pm25pcs2ugm3 (concentration_pcs);
    aqi = pm25ugm32aqi (concentration_ugm3);

    g_debug ("%s %f pcs/0.01cf, %f μg/m3, %d AQI", NAME,
        concentration_pcs, concentration_ugm3, aqi);

    g_mutex_lock (&self->mutex);
    self->last_reading = aqi;
    g_mutex_unlock (&self->mutex);

    shp_grovedust_check (self, aqi);

    self->lowpulseoccupancy = 0;
    self->starttime = millis ();
  }
}

static void
status_changed (int pin, int status, gpointer data)
{
  long micros;
  ShpGrovedust *self = SHP_GROVEDUST (data);

  if (status == 0) {
    gettimeofday (&self->t_low, NULL);
  } else if (status == 1) {
    gettimeofday (&self->t_high, NULL);

    micros = (self->t_high.tv_sec - self->t_low.tv_sec) * 1000000L;
    micros += (self->t_high.tv_usec - self->t_low.tv_usec);

    if (micros > 95000 || micros < 8500)
      g_debug ("pulse duration out of bounds: %ld", micros);

    pulse_detected (self, micros);
  }
}

static gboolean
shp_grovedust_start (ShpPlugin * plugin)
{
  ShpGrovedust *self = SHP_GROVEDUST (plugin);

  if (lngpio_is_exported (self->pin))
    lngpio_unexport (self->pin);

  if (-1 == lngpio_export (self->pin)) {
    g_critical ("could not export pin: %d", self->pin);
    return FALSE;
  }

  if (-1 == lngpio_wait_for_pin (self->pin)) {
    g_critical ("could not wait for pin: %d", self->pin);
    lngpio_unexport (self->pin);
    return FALSE;
  }

  if (-1 == lngpio_set_direction (self->pin, LNGPIO_PIN_DIRECTION_IN)) {
    g_critical ("could not set pin direction: %d", self->pin);
    lngpio_unexport (self->pin);
    return FALSE;
  }

  if (-1 == lngpio_set_edge (self->pin, LNGPIO_PIN_EDGE_BOTH)) {
    g_critical ("could not set pin edge: %d", self->pin);
    lngpio_unexport (self->pin);
    return FALSE;
  }

  self->monitor = lngpio_pin_monitor_create (self->pin, status_changed, self);
  if (NULL == self->monitor) {
    g_critical ("could not create pin monitor for pin: %d", self->pin);
    lngpio_unexport (self->pin);
    return FALSE;
  }

  return TRUE;
}

static void
shp_grovedust_stop (ShpPlugin * plugin)
{
  ShpGrovedust * self = SHP_GROVEDUST (plugin);

  g_return_if_fail (IS_SHP_GROVEDUST (plugin));

  if (self->monitor) {
    lngpio_pin_monitor_stop (self->monitor);
    self->monitor = NULL;
    lngpio_unexport (self->pin);
  }
}

static gboolean
shp_grovedust_check (ShpGrovedust * self, guint reading)
{
  ShpData *data;

  g_debug ("grovedust: check");

  /* chain up to parent first */
  if (!shp_plugin_check (SHP_PLUGIN (self))) {
    return FALSE;
  }

  g_debug ("current last reading: %d", reading);

  data = shp_data_new ();
  shp_data_add_integer (data, "SensorReading", reading);
  shp_data_add_string (data, "ID", SHP_PLUGIN (self)->id);
  shp_plugin_send (SHP_PLUGIN (self), data);

  return TRUE;
}

void
shp_plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_GROVEDUST_TYPE);
}
