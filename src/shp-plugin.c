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
 * This is an abstract class for all plugins. It allows the application to
 * control devices in an uniform way.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-plugin.h"

G_DEFINE_TYPE (ShpPlugin, shp_plugin, SHP_COMPONENT_TYPE);

struct _ShpPluginPrivate {
};

enum
{
  PROP_0,
  PROP_LAST
};

enum
{
  SIGNAL_STATUS_UPDATE,
  SIGNAL_LAST
};

static guint shp_plugin_signals[SIGNAL_LAST] = { 0 };

static void shp_plugin_finalize (GObject * object);
static void shp_plugin_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_plugin_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
shp_plugin_class_init (ShpPluginClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpPluginPrivate));

  gobject_class->finalize = shp_plugin_finalize;
  gobject_class->set_property = shp_plugin_set_property;
  gobject_class->get_property = shp_plugin_get_property;

  shp_plugin_signals[SIGNAL_STATUS_UPDATE] =
      g_signal_new ("status-update", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (ShpPluginClass, status_update), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 1, G_TYPE_NONE);
}

static void
shp_plugin_init (ShpPlugin * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_PLUGIN_TYPE,
                                            ShpPluginPrivate);
}

static void
shp_plugin_finalize (GObject * object)
{
}

static void
shp_plugin_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  //ShpPlugin *plugin = SHP_PLUGIN (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_plugin_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  //ShpPlugin *plugin = SHP_PLUGIN (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

void
shp_plugin_status_update (ShpPlugin * plugin)
{
  g_return_if_fail (IS_SHP_PLUGIN (plugin));

  g_signal_emit (plugin, shp_plugin_signals[SIGNAL_STATUS_UPDATE], 0, NULL);
}
