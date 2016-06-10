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

#ifndef __SHP_DS1820DIGITEMP_H__
#define __SHP_DS1820DIGITEMP_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-base1wire.h"

G_BEGIN_DECLS

#define SHP_DS1820DIGITEMP_TYPE (shp_ds1820digitemp_get_type ())
#define SHP_DS1820DIGITEMP(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_DS1820DIGITEMP_TYPE,ShpDs1820digitemp))
#define SHP_DS1820DIGITEMP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_DS1820DIGITEMP_TYPE, ShpDs1820digitempClass))
#define IS_SHP_DS1820DIGITEMP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_DS1820DIGITEMP_TYPE))
#define IS_SHP_DS1820DIGITEMP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_DS1820DIGITEMP_TYPE))
#define SHP_DS1820DIGITEMP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_DS1820DIGITEMP_TYPE, ShpDs1820digitempClass))

typedef struct _ShpDs1820digitemp ShpDs1820digitemp;
typedef struct _ShpDs1820digitempClass ShpDs1820digitempClass;

struct _ShpDs1820digitemp {
  ShpBase1wire parent;

  /*< protected >*/

  /*< private >*/
  GMainLoop *loop;
  GMainContext *context;
  GThread *thread;

  gchar *config_dir;
};

struct _ShpDs1820digitempClass {
  ShpBase1wireClass parent_class;

  /*< private >*/
  GPtrArray *readings;
  GMutex mutex;
  glong ts;
};

GType shp_ds1820digitemp_get_type (void);

G_END_DECLS

#endif /* __SHP_DS1820DIGITEMP_H__ */
