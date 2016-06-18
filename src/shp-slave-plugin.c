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
 * This is an abstract class for all slave_plugins. It allows the application to
 * control devices in an uniform way.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-slave-plugin.h"
#include "shp-bus.h"

G_DEFINE_TYPE (ShpSlavePlugin, shp_slave_plugin, SHP_PLUGIN_TYPE);

struct _ShpSlavePluginPrivate {
  ShpBusMessageHandler *handler;
};

enum
{
  PROP_0,
  PROP_LAST
};

static gboolean shp_slave_plugin_start (ShpComponent * component);
static gboolean shp_slave_plugin_stop (ShpComponent * component);

static void shp_slave_plugin_finalize (GObject * object);
static void shp_slave_plugin_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_slave_plugin_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
shp_slave_plugin_class_init (ShpSlavePluginClass * klass)
{
  GObjectClass *gobject_class;
  ShpComponentClass *component_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpSlavePluginPrivate));

  gobject_class->finalize = shp_slave_plugin_finalize;
  gobject_class->set_property = shp_slave_plugin_set_property;
  gobject_class->get_property = shp_slave_plugin_get_property;

  component_class = SHP_COMPONENT_CLASS (klass);
  component_class->start = shp_slave_plugin_start;
  component_class->stop = shp_slave_plugin_stop;
}

static void
shp_slave_plugin_init (ShpSlavePlugin * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_SLAVE_PLUGIN_TYPE,
                                            ShpSlavePluginPrivate);
}

static void
message_received (ShpBus * bus, ShpMessage * message, gpointer user_data)
{
  ShpSlavePluginClass *klass;
  ShpSlavePlugin *plugin = SHP_SLAVE_PLUGIN (user_data);

  klass = SHP_SLAVE_PLUGIN_GET_CLASS (plugin);

  if (klass->message_received)
    klass->message_received (plugin, bus, message);
}

static void
shp_slave_plugin_finalize (GObject * object)
{
}

static gboolean
shp_slave_plugin_start (ShpComponent * component)
{
  ShpSlavePlugin *plugin = SHP_SLAVE_PLUGIN (component);
  ShpSlavePluginPrivate *priv;
  ShpBus *bus;

  g_return_val_if_fail (IS_SHP_SLAVE_PLUGIN (plugin), FALSE);

  priv = plugin->priv;

  g_debug ("slaveplugin: starting slaveplugin");

  bus = shp_component_find_bus (component);
  if (!bus) {
    g_warning ("slaveplugin: unable to find bus");
    return FALSE;
  }

  g_debug ("slaveplugin: registering handler for path: %s",
      shp_component_get_path (component));

  priv->handler = shp_bus_add_async_handler (bus, message_received,
      g_object_ref (plugin), g_object_unref,
      NULL, shp_component_get_path (component));
  g_object_unref (bus);

  return TRUE;
}

static gboolean
shp_slave_plugin_stop (ShpComponent * component)
{
  ShpSlavePlugin *plugin = SHP_SLAVE_PLUGIN (component);
  ShpSlavePluginPrivate *priv;
  ShpBus *bus;

  g_return_if_fail (IS_SHP_SLAVE_PLUGIN (plugin));

  priv = plugin->priv;

  g_debug ("stopping slaveplugin");

  bus = shp_component_find_bus (component);
  if (!bus) {
    g_warning ("slaveplugin: unable to find bus");
    return FALSE;
  }

  if (priv->handler) {
    shp_bus_remove_async_handler (bus, priv->handler);
    priv->handler = NULL;
  }
  g_object_unref (bus);

  return TRUE;
}

static void
shp_slave_plugin_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  //ShpSlavePlugin *slave_plugin = SHP_SLAVE_PLUGIN (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_slave_plugin_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  //ShpSlavePlugin *slave_plugin = SHP_SLAVE_PLUGIN (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}
