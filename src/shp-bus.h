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
 * Asynchronous message bus subsystem
 */

#ifndef __SHP_BUS_H__
#define __SHP_BUS_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-message.h"

G_BEGIN_DECLS

#define SHP_BUS_TYPE (shp_bus_get_type ())
#define SHP_BUS(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_BUS_TYPE,ShpBus))
#define SHP_BUS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_BUS_TYPE, ShpBusClass))
#define IS_SHP_BUS(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_BUS_TYPE))
#define IS_SHP_BUS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_BUS_TYPE))
#define SHP_BUS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_BUS_TYPE, ShpBusClass))

#define SHP_BUS_EVENT_TYPE_ANY "*"

typedef struct _ShpBus ShpBus;
typedef struct _ShpBusPrivate ShpBusPrivate;
typedef struct _ShpBusClass ShpBusClass;
typedef struct _ShpBusMessageHandler ShpBusMessageHandler;

typedef void (*ShpBusMessageHandlerFunc) (ShpBus * bus, ShpMessage * message,
    gpointer user_data);

struct _ShpBus {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpBusPrivate *priv;
};

struct _ShpBusClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpBus* shp_bus_new ();
gboolean shp_bus_post (ShpBus *bus, ShpMessage *message);
void shp_bus_set_sync_handler (ShpBus *bus, ShpBusMessageHandlerFunc func,
    gpointer user_data, GDestroyNotify notify);
ShpBusMessageHandler* shp_bus_add_async_handler (ShpBus *bus,
    ShpBusMessageHandlerFunc func, gpointer user_data, GDestroyNotify notify,
    const gchar * source_path, const gchar * destination_path);
void shp_bus_remove_async_handler (ShpBus * bus,
    ShpBusMessageHandler * handler);
gboolean shp_bus_start (ShpBus *bus);
gboolean shp_bus_stop (ShpBus *bus);

GType shp_bus_get_type (void);

G_END_DECLS

#endif /* __SHP_BUS_H__ */
