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
#ifndef __SHP_SCENECTL_H__
#define __SHP_SCENECTL_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-scene.h"
#include "../shp-slave-plugin.h"

G_BEGIN_DECLS

#define SHP_SCENECTL_TYPE (shp_scenectl_get_type ())
#define SHP_SCENECTL(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_SCENECTL_TYPE,ShpScenectl))
#define SHP_SCENECTL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_SCENECTL_TYPE, ShpScenectlClass))
#define IS_SHP_SCENECTL(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_SCENECTL_TYPE))
#define IS_SHP_SCENECTL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_SCENECTL_TYPE))
#define SHP_SCENECTL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_SCENECTL_TYPE, ShpScenectlClass))

typedef struct _ShpScenectl ShpScenectl;
typedef struct _ShpScenectlClass ShpScenectlClass;

struct _ShpScenectl {
  ShpSlavePlugin parent;

  /*< protected >*/

  /*< private >*/
  ShpScene *scene;
};

struct _ShpScenectlClass {
  ShpSlavePluginClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_scenectl_get_type (void);

G_END_DECLS

#endif /* __SHP_SCENECTL_H__ */
