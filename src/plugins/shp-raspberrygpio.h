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

#ifndef __SHP_RASPBERRYGPIO_H__
#define __SHP_RASPBERRYGPIO_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-base1wire.h"

G_BEGIN_DECLS

#define SHP_RASPBERRYGPIO_TYPE (shp_raspberrygpio_get_type ())
#define SHP_RASPBERRYGPIO(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_RASPBERRYGPIO_TYPE,ShpRaspberrygpio))
#define SHP_RASPBERRYGPIO_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_RASPBERRYGPIO_TYPE, ShpRaspberrygpioClass))
#define IS_SHP_RASPBERRYGPIO(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_RASPBERRYGPIO_TYPE))
#define IS_SHP_RASPBERRYGPIO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_RASPBERRYGPIO_TYPE))
#define SHP_RASPBERRYGPIO_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_RASPBERRYGPIO_TYPE, ShpRaspberrygpioClass))

typedef struct _ShpRaspberrygpio ShpRaspberrygpio;
typedef struct _ShpRaspberrygpioClass ShpRaspberrygpioClass;

struct _ShpRaspberrygpio {
  ShpBase1wire parent;

  /*< protected >*/

  /*< private >*/
};

struct _ShpRaspberrygpioClass {
  ShpBase1wireClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_raspberrygpio_get_type (void);

G_END_DECLS

#endif /* __SHP_RASPBERRYGPIO_H__ */
