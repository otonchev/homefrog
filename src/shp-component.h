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
 * A ShpComponent represents a component of plugins, allowing them to be managed as a
 * component.
 */

#ifndef __SHP_COMPONENT_H__
#define __SHP_COMPONENT_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-bus.h"
#include "shp-message.h"

G_BEGIN_DECLS

#define SHP_COMPONENT_TYPE (shp_component_get_type ())
#define SHP_COMPONENT(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_COMPONENT_TYPE,ShpComponent))
#define SHP_COMPONENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_COMPONENT_TYPE, ShpComponentClass))
#define IS_SHP_COMPONENT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_COMPONENT_TYPE))
#define IS_SHP_COMPONENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_COMPONENT_TYPE))
#define SHP_COMPONENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_COMPONENT_TYPE, ShpComponentClass))

typedef struct _ShpComponent ShpComponent;
typedef struct _ShpComponentPrivate ShpComponentPrivate;
typedef struct _ShpComponentClass ShpComponentClass;

/**
 * ShpComponent:
 *
 */
struct _ShpComponent {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpComponentPrivate *priv;
};

/**
 * ShpComponentClass:
 *
 */
struct _ShpComponentClass {
  GObjectClass parent_class;

  /*< private >*/

  /* signal callbacks */

  /*< protected >*/

  /* virtual methods for subclasses */
  gboolean (*start) (ShpComponent * component);
  gboolean (*stop) (ShpComponent * component);
};

ShpComponent* shp_component_new (ShpBus * bus);
gboolean shp_component_start (ShpComponent * component);
gboolean shp_component_stop (ShpComponent * component);
ShpBus* shp_component_get_bus (ShpComponent * component);
ShpComponent* shp_component_get_parent (ShpComponent * component);
void shp_component_set_parent (ShpComponent * component, ShpComponent * parent);
const gchar* shp_component_get_name (ShpComponent * component);
const gchar* shp_component_get_path (ShpComponent * component);
gboolean shp_component_post_message (ShpComponent * component,
    ShpMessage * message);
ShpBus* shp_component_find_bus (ShpComponent * component);

GType shp_component_get_type (void);

G_END_DECLS

#endif /* __SHP_COMPONENT_H__ */
