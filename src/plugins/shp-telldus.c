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
 * This plugin is used for or emitting radio frequency signals and thus
 * turning on/off wall-plug sockets.
 *
 * It implements also the Radio interface for controlling radio device
 * in an unified manner.
 *
 * In order for the plugin to work devices should be properly configured
 * in the telldus configuration file.
 *
 * Configure specification:
 *
 * Properties inherited from baseplugin:
 *
 * interval=<integer>
 *
 * Properties for this plugin:
 *
 * device-id=<integer>
 * set-on=<boolean>
 *
 * Where device-id is a device id from the Telldus configuration file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <telldus-core.h>

#include "../shp-data.h"
#include "../shp-plugin-factory.h"
#include "../shp-radio.h"
#include "shp-telldus.h"

#define NAME "telldus"
#define UNKNOWN_DEVICE_ID -1

#define DEFAULT_SET_ON TRUE
#define DEFAULT_DEVICE_ID UNKNOWN_DEVICE_ID

enum
{
  PROP_0,
  PROP_DEVICE_ID,
  PROP_SET_ON,
  PROP_LAST
};


static void shp_telldus_interface_init (ShpRadioInterface * iface);
static gboolean turn_on (ShpRadio * radio, guint id);
static gboolean turn_off (ShpRadio * radio, guint id);
static RadioStatus get_status (ShpRadio * radio, guint id);

static void shp_data_received (ShpPlugin * plugin, ShpPad * pad,
    ShpData * data);

static void shp_telldus_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_telldus_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE_WITH_CODE (ShpTelldus, shp_telldus, SHP_PLUGIN_TYPE,
    G_IMPLEMENT_INTERFACE (SHP_TYPE_RADIO,
        shp_telldus_interface_init));

static void
shp_telldus_class_init (ShpTelldusClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = shp_telldus_set_property;
  gobject_class->get_property = shp_telldus_get_property;

  SHP_PLUGIN_CLASS (klass)->data_received = shp_data_received;

  g_object_class_install_property (gobject_class, PROP_SET_ON,
      g_param_spec_boolean ("set-on", "Turn device on",
          "Whether toturn a device on", DEFAULT_SET_ON, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DEVICE_ID,
      g_param_spec_int ("device-id", "Device id",
          "Device id to turn on/off (from the telldus configuration file)",
          -1, G_MAXINT, DEFAULT_DEVICE_ID, G_PARAM_READWRITE));
}

static void
shp_telldus_init (ShpTelldus * self)
{
}

static void
shp_telldus_interface_init (ShpRadioInterface * iface)
{
  iface->turn_on = turn_on;
  iface->turn_off = turn_off;
  iface->get_status = get_status;
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
    case PROP_SET_ON:
      g_value_set_boolean (value, self->set_on);
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
    case PROP_SET_ON:
      self->set_on = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static gboolean
turn_on (ShpRadio * radio, guint id)
{
  gint retval;

  g_return_val_if_fail (IS_SHP_TELLDUS (radio), FALSE);

  g_debug ("telldus: turning on %d", id);

  retval = tdTurnOn (id);

  if (retval != TELLSTICK_SUCCESS ) {
    gchar *errorString = tdGetErrorString (retval);
    g_critical ("%s: error: %s", errorString, NAME);
    tdReleaseString (errorString);
    return FALSE;
  }

  return TRUE;
}

static gboolean
turn_off (ShpRadio * radio, guint id)
{
  gint retval;

  g_return_val_if_fail (IS_SHP_TELLDUS (radio), FALSE);

  g_debug ("telldus: turning off %d", id);

  retval = tdTurnOff (id);

  if (retval != TELLSTICK_SUCCESS ) {
    gchar *errorString = tdGetErrorString (retval);
    g_critical ("%s: error: %s", NAME, errorString);
    tdReleaseString (errorString);
    return FALSE;
  }

  return TRUE;
}

static RadioStatus
get_status (ShpRadio * radio, guint id)
{
  int state;

  g_return_val_if_fail (IS_SHP_TELLDUS (radio), RADIO_UNKNOWN);

  state = tdLastSentCommand (id, TELLSTICK_TURNON | TELLSTICK_TURNOFF);
  if (state == TELLSTICK_TURNON) {
    g_debug ("%d last command On", id);
    return RADIO_ON;
  } else if (state == TELLSTICK_TURNOFF) {
    g_debug ("%d last command Off", id);
    return RADIO_OFF;
  }

  g_debug ("%d last command Unknown", id);
  return RADIO_UNKNOWN;
}

static void
shp_data_received (ShpPlugin * plugin, ShpPad * pad, ShpData * data)
{
  gint id;
  gboolean set_on;
  ShpTelldus *self = SHP_TELLDUS (plugin);

  /* chain up to parent first */
  if (!shp_plugin_check (plugin)) {
    return;
  }

  if (self->device_id == UNKNOWN_DEVICE_ID) {
    g_warning ("incomplete configuration, missing 'device-id'");
    return;
  }
  id = self->device_id;
  set_on = self->set_on;

  if (set_on && get_status (SHP_RADIO (plugin), id) != RADIO_ON)
    turn_on (SHP_RADIO (plugin), id);
  else if (get_status (SHP_RADIO (plugin), id) != RADIO_OFF)
    turn_off (SHP_RADIO (plugin), id);
}

void
shp_plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_TELLDUS_TYPE);
}
