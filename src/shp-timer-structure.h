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
 * ShpStructureTimer — structure containing time-related fields of names and
 * values.
 */

#ifndef __SHP_STRUCTURE_TIMER_H__
#define __SHP_STRUCTURE_TIMER_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-structure.h"

G_BEGIN_DECLS

#define SHP_STRUCTURE_TIMER_TYPE (shp_structure_timer_get_type ())
#define SHP_STRUCTURE_TIMER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_STRUCTURE_TIMER_TYPE,ShpStructureTimer))
#define SHP_STRUCTURE_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_STRUCTURE_TIMER_TYPE, ShpStructureTimerClass))
#define IS_SHP_STRUCTURE_TIMER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_STRUCTURE_TIMER_TYPE))
#define IS_SHP_STRUCTURE_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_STRUCTURE_TIMER_TYPE))
#define SHP_STRUCTURE_TIMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_STRUCTURE_TIMER_TYPE, ShpStructureTimerClass))

typedef struct _ShpStructureTimer ShpStructureTimer;
typedef struct _ShpStructureTimerPrivate ShpStructureTimerPrivate;
typedef struct _ShpStructureTimerClass ShpStructureTimerClass;

struct _ShpStructureTimer {
  ShpStructure parent;

  /*< protected >*/

  /*< private >*/
  ShpStructureTimerPrivate *priv;
};

struct _ShpStructureTimerClass {
  ShpStructureClass parent_class;

  /*< private >*/
};

ShpStructureTimer* shp_structure_timer_new ();

GType shp_structure_timer_get_type (void);

G_END_DECLS

#endif /* __SHP_STRUCTURE_TIMER_H__ */
