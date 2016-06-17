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
 * Timer plugin emitting current time and date every 1s.
 */

#ifndef __SHP_TIMER_H__
#define __SHP_TIMER_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_TIMER_TYPE (shp_timer_get_type ())
#define SHP_TIMER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_TIMER_TYPE,ShpTimer))
#define SHP_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_TIMER_TYPE, ShpTimerClass))
#define IS_SHP_TIMER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_TIMER_TYPE))
#define IS_SHP_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_TIMER_TYPE))
#define SHP_TIMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_TIMER_TYPE, ShpTimerClass))

#define SHP_TIMER_INVALID_READING -256

typedef struct _ShpTimer ShpTimer;
typedef struct _ShpTimerClass ShpTimerClass;

struct _ShpTimer {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  GSource *dispatch_source;

  GMainLoop *loop;
  GMainContext *context;
  GThread *thread;
};

struct _ShpTimerClass {
  ShpPluginClass parent_class;

  /*< private >*/
};

GType shp_timer_get_type (void);

G_END_DECLS

#endif /* __SHP_TIMER_H__ */
