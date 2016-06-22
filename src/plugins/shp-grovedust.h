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

#ifndef __SHP_GROVEDUST_H__
#define __SHP_GROVEDUST_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_GROVEDUST_TYPE (shp_grovedust_get_type ())
#define SHP_GROVEDUST(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_GROVEDUST_TYPE,ShpGrovedust))
#define SHP_GROVEDUST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_GROVEDUST_TYPE, ShpGrovedustClass))
#define IS_SHP_GROVEDUST(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_GROVEDUST_TYPE))
#define IS_SHP_GROVEDUST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_GROVEDUST_TYPE))
#define SHP_GROVEDUST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_GROVEDUST_TYPE, ShpGrovedustClass))

typedef struct _ShpGrovedust ShpGrovedust;
typedef struct _ShpGrovedustClass ShpGrovedustClass;

struct _ShpGrovedust {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  gint pin;
  LNGPIOPinMonitor *monitor;
  guint last_reading;

  GMutex mutex;
  unsigned long starttime;
  unsigned long lowpulseoccupancy;
  struct timeval t_low;
  struct timeval t_high;
};

struct _ShpGrovedustClass {
  ShpPluginClass parent_class;

  /*< private >*/
};

GType shp_grovedust_get_type (void);

G_END_DECLS

#endif /* __SHP_GROVEDUST_H__ */
