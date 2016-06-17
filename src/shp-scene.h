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
 * ShpScene groups events so that thay can be activated simultaneously.
 */

#ifndef __SHP_SCENE_H__
#define __SHP_SCENE_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-message.h"
#include "shp-bus.h"

G_BEGIN_DECLS

#define SHP_SCENE_TYPE (shp_scene_get_type ())
#define SHP_SCENE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_SCENE_TYPE,ShpScene))
#define SHP_SCENE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_SCENE_TYPE, ShpSceneClass))
#define IS_SHP_SCENE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_SCENE_TYPE))
#define IS_SHP_SCENE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_SCENE_TYPE))
#define SHP_SCENE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_SCENE_TYPE, ShpSceneClass))

typedef struct _ShpScene ShpScene;
typedef struct _ShpScenePrivate ShpScenePrivate;
typedef struct _ShpSceneClass ShpSceneClass;

struct _ShpScene {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpScenePrivate *priv;
};

struct _ShpSceneClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpScene * shp_scene_new (ShpBus * bus);
void shp_scene_add_event (ShpScene * scene, ShpMessage * event);
gboolean shp_scene_activate (ShpScene * scene);

GType shp_scene_get_type (void);

G_END_DECLS

#endif /* __SHP_SCENE_H__ */
