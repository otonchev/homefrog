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
 * This is an abstract class for all plugins. It allows the application to
 * control devices in an uniform way.
 */

#ifndef __SHP_PLUGIN_H__
#define __SHP_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-component.h"

G_BEGIN_DECLS

#define SHP_PLUGIN_TYPE (shp_plugin_get_type ())
#define SHP_PLUGIN(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_PLUGIN_TYPE,ShpPlugin))
#define SHP_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_PLUGIN_TYPE, ShpPluginClass))
#define IS_SHP_PLUGIN(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_PLUGIN_TYPE))
#define IS_SHP_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_PLUGIN_TYPE))
#define SHP_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_PLUGIN_TYPE, ShpPluginClass))

typedef struct _ShpPlugin ShpPlugin;
typedef struct _ShpPluginPrivate ShpPluginPrivate;
typedef struct _ShpPluginClass ShpPluginClass;

#define SHP_PLUGIN_REGISTER(init)	\
G_BEGIN_DECLS				\
void shp_plugin_register ();		\
					\
void shp_plugin_register ()		\
{					\
  init ();				\
}					\
G_END_DECLS

/**
 * ShpPlugin:
 *
 */
struct _ShpPlugin {
  ShpComponent parent;

  /*< protected >*/

  /*< private >*/
  ShpPluginPrivate *priv;
};

/**
 * ShpPluginClass:
 *
 */
struct _ShpPluginClass {
  ShpComponentClass parent_class;

  /*< private >*/

  /* signal callbacks */
  void (*status_update) (ShpPlugin * plugin);

  /*< protected >*/

  /* virtual methods for subclasses */
};

void shp_plugin_status_update (ShpPlugin * plugin);

GType shp_plugin_get_type (void);

G_END_DECLS

#endif /* __SHP_PLUGIN_H__ */
