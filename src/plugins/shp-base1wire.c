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
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "../shp-message.h"
#include "../shp-component.h"
#include "../shp-plugin-factory.h"
#include "shp-base1wire.h"

#define DEFAULT_DEVICE_ID NULL

enum
{
  PROP_0,
  PROP_DEVICE_ID,
  PROP_LAST
};

static void shp_base1wire_finalize (GObject * object);

static void shp_base1wire_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_base1wire_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static gboolean shp_base1wire_start (ShpComponent * component);
static gboolean shp_base1wire_stop (ShpComponent * component);

typedef struct _ShpSensorData ShpSensorData;
struct _ShpSensorData
{
  gchar* id;
  gfloat reading;
};

G_DEFINE_TYPE (ShpBase1wire, shp_base1wire, SHP_PLUGIN_TYPE);

static void
shp_base1wire_class_init (ShpBase1wireClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_base1wire_finalize;
  gobject_class->set_property = shp_base1wire_set_property;
  gobject_class->get_property = shp_base1wire_get_property;

  SHP_COMPONENT_CLASS (klass)->start = shp_base1wire_start;
  SHP_COMPONENT_CLASS (klass)->stop = shp_base1wire_stop;

  g_object_class_install_property (gobject_class, PROP_DEVICE_ID,
      g_param_spec_string ("device-id", "Device Id",
          "The Id of the 1Wire device to be read by the plugin",
          DEFAULT_DEVICE_ID, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_base1wire_init (ShpBase1wire * self)
{
  self->device_id = g_strdup (DEFAULT_DEVICE_ID);
}

static void
shp_base1wire_finalize (GObject * object)
{
  ShpBase1wire *self = SHP_BASE1WIRE (object);

  g_free (self->device_id);
}

static gfloat
read_sensor_data (ShpBase1wire * self, const gchar * id)
{
  if (SHP_BASE1WIRE_GET_CLASS (self)->read_sensor_data == NULL) {
    g_warning ("no read_sensor_data implemented");
    return SHP_BASE1WIRE_INVALID_READING;
  }
  return SHP_BASE1WIRE_GET_CLASS (self)->read_sensor_data (self, id);
}

static gboolean
dispatch_cb (gpointer user_data)
{
  ShpComponent *component = SHP_COMPONENT (user_data);
  ShpBase1wire *self = SHP_BASE1WIRE (component);
  ShpMessage *msg;
  gfloat reading;

  g_debug ("base1wire: reading sensor data");

  if (self->device_id == NULL) {
    g_warning ("incomplete configuration, missing 'device-id'");
    return TRUE;
  }

  reading = read_sensor_data (self, self->device_id);
  if (reading == SHP_BASE1WIRE_INVALID_READING) {
    g_warning ("invalid temperature reading");
    return TRUE;
  }

  msg = shp_message_new (shp_component_get_name (component),
      shp_component_get_path (component));
  shp_message_add_double (msg, "reading", reading);

  if (!shp_component_post_message (component, msg))
    g_warning ("could not post message on bus");

  return TRUE;
}

static gboolean
shp_base1wire_start (ShpComponent * component)
{
  ShpBase1wire *self = SHP_BASE1WIRE (component);

  if (self->device_id == NULL) {
    g_warning ("no 'device-id' provided");
    return FALSE;
  }

  self->dispatch_source = g_timeout_source_new_seconds (60);
  g_source_set_callback (self->dispatch_source, dispatch_cb,
      g_object_ref (self), g_object_unref);
  g_source_attach (self->dispatch_source, NULL);

  return TRUE;
}

static gboolean
shp_base1wire_stop (ShpComponent * component)
{
  ShpBase1wire *self = SHP_BASE1WIRE (component);

  g_source_destroy (self->dispatch_source);
  g_source_unref (self->dispatch_source);
  self->dispatch_source = NULL;

  return TRUE;
}
