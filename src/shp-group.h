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
 * A ShpGroup represents a group of components, allowing them to be managed as a
 * group.
 */

#ifndef __SHP_GROUP_H__
#define __SHP_GROUP_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-component.h"

G_BEGIN_DECLS

#define SHP_GROUP_TYPE (shp_group_get_type ())
#define SHP_GROUP(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_GROUP_TYPE,ShpGroup))
#define SHP_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_GROUP_TYPE, ShpGroupClass))
#define IS_SHP_GROUP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_GROUP_TYPE))
#define IS_SHP_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_GROUP_TYPE))
#define SHP_GROUP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_GROUP_TYPE, ShpGroupClass))

typedef struct _ShpGroup ShpGroup;
typedef struct _ShpGroupPrivate ShpGroupPrivate;
typedef struct _ShpGroupClass ShpGroupClass;

/**
 * ShpGroup:
 *
 */
struct _ShpGroup {
  ShpComponent parent;

  /*< protected >*/

  /*< private >*/
  ShpGroupPrivate *priv;
};

/**
 * ShpGroupClass:
 *
 */
struct _ShpGroupClass {
  ShpComponentClass parent_class;

  /*< private >*/

  /* signal callbacks */

  /*< protected >*/

  /* virtual methods for subclasses */
};

ShpGroup* shp_group_new ();
gboolean shp_group_add (ShpGroup * self, ShpComponent * component);

GType shp_group_get_type (void);

G_END_DECLS

#endif /* __SHP_GROUP_H__ */
