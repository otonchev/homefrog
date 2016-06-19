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
 * ShpComplextypeTimer — complextype containing time-related fields of names and
 * values.
 */

#ifndef __SHP_COMPLEXTYPE_TIMER_H__
#define __SHP_COMPLEXTYPE_TIMER_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-complextype.h"

G_BEGIN_DECLS

#define SHP_COMPLEXTYPE_TIMER_TYPE (shp_complextype_timer_get_type ())
#define SHP_COMPLEXTYPE_TIMER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_COMPLEXTYPE_TIMER_TYPE,ShpComplextypeTimer))
#define SHP_COMPLEXTYPE_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_COMPLEXTYPE_TIMER_TYPE, ShpComplextypeTimerClass))
#define IS_SHP_COMPLEXTYPE_TIMER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_COMPLEXTYPE_TIMER_TYPE))
#define IS_SHP_COMPLEXTYPE_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_COMPLEXTYPE_TIMER_TYPE))
#define SHP_COMPLEXTYPE_TIMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_COMPLEXTYPE_TIMER_TYPE, ShpComplextypeTimerClass))

typedef struct _ShpComplextypeTimer ShpComplextypeTimer;
typedef struct _ShpComplextypeTimerPrivate ShpComplextypeTimerPrivate;
typedef struct _ShpComplextypeTimerClass ShpComplextypeTimerClass;

struct _ShpComplextypeTimer {
  ShpComplextype parent;

  /*< protected >*/

  /*< private >*/
  ShpComplextypeTimerPrivate *priv;
};

struct _ShpComplextypeTimerClass {
  ShpComplextypeClass parent_class;

  /*< private >*/
};

ShpComplextypeTimer* shp_complextype_timer_new ();

GType shp_complextype_timer_get_type (void);

G_END_DECLS

#endif /* __SHP_COMPLEXTYPE_TIMER_H__ */
