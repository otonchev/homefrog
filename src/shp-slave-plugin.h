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

#ifndef __SHP_SLAVE_PLUGIN_H__
#define __SHP_SLAVE_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-plugin.h"

G_BEGIN_DECLS

#define SHP_SLAVE_PLUGIN_TYPE (shp_slave_plugin_get_type ())
#define SHP_SLAVE_PLUGIN(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_SLAVE_PLUGIN_TYPE,ShpSlavePlugin))
#define SHP_SLAVE_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_SLAVE_PLUGIN_TYPE, ShpSlavePluginClass))
#define IS_SHP_SLAVE_PLUGIN(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_SLAVE_PLUGIN_TYPE))
#define IS_SHP_SLAVE_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_SLAVE_PLUGIN_TYPE))
#define SHP_SLAVE_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_SLAVE_PLUGIN_TYPE, ShpSlavePluginClass))

typedef struct _ShpSlavePlugin ShpSlavePlugin;
typedef struct _ShpSlavePluginPrivate ShpSlavePluginPrivate;
typedef struct _ShpSlavePluginClass ShpSlavePluginClass;

/**
 * ShpSlavePlugin:
 *
 */
struct _ShpSlavePlugin {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  ShpSlavePluginPrivate *priv;
};

/**
 * ShpSlavePluginClass:
 *
 */
struct _ShpSlavePluginClass {
  ShpPluginClass parent_class;

  /*< private >*/

  /* signal callbacks */

  /*< protected >*/

  /* virtual methods for subclasses */
  void (*message_received) (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message);
};

GType shp_slave_plugin_get_type (void);

G_END_DECLS

#endif /* __SHP_SLAVE_PLUGIN_H__ */
