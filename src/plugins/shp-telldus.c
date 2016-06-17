/* homefrog
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
 * This plugin is used for or emitting radio frequency signals and thus
 * turning on/off wall-plug sockets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <telldus-core.h>

#include "../shp-message.h"
#include "../shp-plugin-factory.h"
#include "shp-telldus.h"

#define NAME "telldus"
#define UNKNOWN_DEVICE_ID -1

#define DEFAULT_DEVICE_ID UNKNOWN_DEVICE_ID

enum
{
  PROP_0,
  PROP_DEVICE_ID,
  PROP_LAST
};

static void status_update (ShpPlugin * plugin);

static gboolean change_status (ShpTelldus * self, gboolean on);
static gint get_status (ShpTelldus * self);

static void message_received (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message);

static void shp_telldus_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_telldus_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpTelldus, shp_telldus, SHP_SLAVE_PLUGIN_TYPE);

static void
shp_telldus_class_init (ShpTelldusClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = shp_telldus_set_property;
  gobject_class->get_property = shp_telldus_get_property;

  SHP_SLAVE_PLUGIN_CLASS (klass)->message_received = message_received;

  g_object_class_install_property (gobject_class, PROP_DEVICE_ID,
      g_param_spec_int ("device-id", "Device id",
          "Device id to control (from the telldus configuration file)",
          -1, G_MAXINT, DEFAULT_DEVICE_ID, G_PARAM_READWRITE));
}

static void
shp_telldus_init (ShpTelldus * self)
{
  g_signal_connect (G_OBJECT (self), "status-update",
      G_CALLBACK (status_update), NULL);
}

static void
shp_telldus_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpTelldus *self = SHP_TELLDUS (object);

  switch (propid) {
    case PROP_DEVICE_ID:
      g_value_set_int (value, self->device_id);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_telldus_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpTelldus *self = SHP_TELLDUS (object);

  switch (propid) {
    case PROP_DEVICE_ID:
      self->device_id = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
signal_new_status (ShpTelldus * self, gboolean on)
{
  ShpMessage *msg;
  ShpComponent *component = SHP_COMPONENT (self);

  g_debug ("telldus: signalling new status: %d", on);

  msg = shp_message_new (shp_component_get_path (component));
  shp_message_add_string (msg, "status", (on) ? "on" : "off");

  if (!shp_component_post_message (component, msg))
    g_warning ("could not post message on bus");
}

static void
status_update (ShpPlugin * plugin)
{
  gint status;

  status = get_status (SHP_TELLDUS (plugin));
  if (status == -1) {
    g_warning ("telldus: unable to get device status");
    return;
  }

  signal_new_status (SHP_TELLDUS (plugin), (status == 1));
}

static gboolean
change_status (ShpTelldus * self, gboolean on)
{
  gint retval;

  g_return_val_if_fail (IS_SHP_TELLDUS (self), FALSE);

  g_debug ("telldus: turning %s %d", (on) ? "on" : "off", self->device_id);

  if (on)
    retval = tdTurnOn (self->device_id);
  else
    retval = tdTurnOff (self->device_id);

  if (retval != TELLSTICK_SUCCESS ) {
    gchar *errorString = tdGetErrorString (retval);
    g_critical ("%s: error: %s", NAME, errorString);
    tdReleaseString (errorString);
    return FALSE;
  }

  signal_new_status (self, on);

  return TRUE;
}

static gint
get_status (ShpTelldus * self)
{
  int state;

  g_return_val_if_fail (IS_SHP_TELLDUS (self), -1);

  state = tdLastSentCommand (self->device_id,
      TELLSTICK_TURNON | TELLSTICK_TURNOFF);
  if (state == TELLSTICK_TURNON) {
    g_debug ("%d last command On", self->device_id);
    return 1;
  } else if (state == TELLSTICK_TURNOFF) {
    g_debug ("%d last command Off", self->device_id);
    return 0;
  }

  g_debug ("%d last command Unknown", self->device_id);
  return -1;
}

static void
message_received (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message)
{
  gboolean set_on;
  const gchar *status;
  ShpTelldus *self = SHP_TELLDUS (plugin);

  if (self->device_id == UNKNOWN_DEVICE_ID) {
    g_warning ("incomplete configuration, missing 'device-id'");
    return;
  }

  status = shp_message_get_string (message, "status");
  set_on = !g_strcmp0 (status, "on");

  if (set_on && get_status (SHP_TELLDUS (plugin)) != 1)
    change_status (SHP_TELLDUS (plugin), TRUE);
  else if (get_status (SHP_TELLDUS (plugin)) != 0)
    change_status (SHP_TELLDUS (plugin), FALSE);
}

static void
plugin_register (void)
{
  g_debug ("%s: loading plugin", NAME);
  shp_plugin_factory_register (NAME, SHP_TELLDUS_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
