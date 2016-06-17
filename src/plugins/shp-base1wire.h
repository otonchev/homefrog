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
 * This a is a baseclass for plugins obtaining readings from a 1-Wire Serial
 * network.
 * Read in .c file for detailed information on how to use it.
 */

#ifndef __SHP_BASE1WIRE_H__
#define __SHP_BASE1WIRE_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_BASE1WIRE_TYPE (shp_base1wire_get_type ())
#define SHP_BASE1WIRE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_BASE1WIRE_TYPE,ShpBase1wire))
#define SHP_BASE1WIRE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_BASE1WIRE_TYPE, ShpBase1wireClass))
#define IS_SHP_BASE1WIRE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_BASE1WIRE_TYPE))
#define IS_SHP_BASE1WIRE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_BASE1WIRE_TYPE))
#define SHP_BASE1WIRE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_BASE1WIRE_TYPE, ShpBase1wireClass))

#define SHP_BASE1WIRE_INVALID_READING -256

typedef struct _ShpBase1wire ShpBase1wire;
typedef struct _ShpBase1wireClass ShpBase1wireClass;

struct _ShpBase1wire {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  GSource *dispatch_source;

  GMainLoop *loop;
  GMainContext *context;
  GThread *thread;

  gchar *device_id;
  gboolean active;
};

struct _ShpBase1wireClass {
  ShpPluginClass parent_class;

  /*< private >*/
  gfloat (*read_sensor_data) (ShpBase1wire * self, const gchar * id);
};

GType shp_base1wire_get_type (void);

G_END_DECLS

#endif /* __SHP_BASE1WIRE_H__ */
