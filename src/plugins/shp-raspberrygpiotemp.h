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
 * This a plugin for obtaining readings from a 1-Wire Serial network.
 * Read in .c file for detailed information on how to use it.
 */

#ifndef __SHP_RASPBERRYGPIOTEMP_H__
#define __SHP_RASPBERRYGPIOTEMP_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-base1wire.h"

G_BEGIN_DECLS

#define SHP_RASPBERRYGPIOTEMP_TYPE (shp_raspberrygpiotemp_get_type ())
#define SHP_RASPBERRYGPIOTEMP(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_RASPBERRYGPIOTEMP_TYPE,ShpRaspberrygpiotemp))
#define SHP_RASPBERRYGPIOTEMP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_RASPBERRYGPIOTEMP_TYPE, ShpRaspberrygpiotempClass))
#define IS_SHP_RASPBERRYGPIOTEMP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_RASPBERRYGPIOTEMP_TYPE))
#define IS_SHP_RASPBERRYGPIOTEMP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_RASPBERRYGPIOTEMP_TYPE))
#define SHP_RASPBERRYGPIOTEMP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_RASPBERRYGPIOTEMP_TYPE, ShpRaspberrygpiotempClass))

typedef struct _ShpRaspberrygpiotemp ShpRaspberrygpiotemp;
typedef struct _ShpRaspberrygpiotempClass ShpRaspberrygpiotempClass;

struct _ShpRaspberrygpiotemp {
  ShpBase1wire parent;

  /*< protected >*/

  /*< private >*/
};

struct _ShpRaspberrygpiotempClass {
  ShpBase1wireClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_raspberrygpiotemp_get_type (void);

G_END_DECLS

#endif /* __SHP_RASPBERRYGPIOTEMP_H__ */
