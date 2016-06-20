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

#include "../shp-message.h"
#include "../shp-plugin-factory.h"
#include "shp-scenectl.h"

#define NAME "scenectl"

enum
{
  PROP_0,
  PROP_SCENE,
  PROP_LAST
};

static void status_update (ShpPlugin * plugin);

static void message_received (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message);

static void shp_scenectl_finalize (GObject * object);
static void shp_scenectl_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_scenectl_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpScenectl, shp_scenectl, SHP_SLAVE_PLUGIN_TYPE);

static void
shp_scenectl_class_init (ShpScenectlClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_scenectl_finalize;
  gobject_class->set_property = shp_scenectl_set_property;
  gobject_class->get_property = shp_scenectl_get_property;

  SHP_SLAVE_PLUGIN_CLASS (klass)->message_received = message_received;

  g_object_class_install_property (gobject_class, PROP_SCENE,
      g_param_spec_object ("scene", "The Scene",
          "Scene the plugin will activate",
          SHP_SCENE_TYPE, G_PARAM_READWRITE));
}

static void
shp_scenectl_init (ShpScenectl * self)
{
  g_signal_connect (G_OBJECT (self), "status-update",
      G_CALLBACK (status_update), NULL);
}

static void
shp_scenectl_finalize (GObject * object)
{
  ShpScenectl *self = SHP_SCENECTL (object);

  if (self->scene) {
    g_object_unref (self->scene);
    self->scene = NULL;
  }
}

static void
shp_scenectl_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpScenectl *self = SHP_SCENECTL (object);

  switch (propid) {
    case PROP_SCENE:
      g_value_set_object (value, self->scene);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_scenectl_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpScenectl *self = SHP_SCENECTL (object);

  switch (propid) {
    case PROP_SCENE:
      if (self->scene)
        g_object_unref (self->scene);
      self->scene = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
status_update (ShpPlugin * plugin)
{
  ShpMessage *msg;

  g_debug ("scenectl: reqest for signalling status");

  if (SHP_SCENECTL (plugin)->scene == NULL) {
    g_warning ("scenectl: incomplete configuration, missing 'scene'");
    return;
  }

  msg = shp_message_new (shp_component_get_path (SHP_COMPONENT (plugin)));

  if (!shp_component_post_message (SHP_COMPONENT (plugin), msg))
    g_warning ("could not post message on bus");
}

static void
message_received (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message)
{
  gboolean activate;
  const gchar *command;
  ShpScenectl *self = SHP_SCENECTL (plugin);

  g_debug ("scenectl: received event");

  if (self->scene == NULL) {
    g_warning ("incomplete configuration, missing 'scene'");
    return;
  }

  command = shp_message_get_string (message, "command");
  g_assert (command != NULL);
  activate = !g_strcmp0 (command, "activate");

  g_debug ("scenectl: command: %s", command);

  if (activate) {
    g_debug ("scenectl: activating scene");
    shp_scene_activate (self->scene);
  } else
    g_debug ("scenectl: not activating scene");
}

static void
plugin_register (void)
{
  g_debug ("%s: loading plugin", NAME);
  shp_plugin_factory_register (NAME, SHP_SCENECTL_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
