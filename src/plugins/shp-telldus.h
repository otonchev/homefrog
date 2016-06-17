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
#ifndef __SHP_TELLDUS_H__
#define __SHP_TELLDUS_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-slave-plugin.h"

G_BEGIN_DECLS

#define SHP_TELLDUS_TYPE (shp_telldus_get_type ())
#define SHP_TELLDUS(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_TELLDUS_TYPE,ShpTelldus))
#define SHP_TELLDUS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_TELLDUS_TYPE, ShpTelldusClass))
#define IS_SHP_TELLDUS(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_TELLDUS_TYPE))
#define IS_SHP_TELLDUS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_TELLDUS_TYPE))
#define SHP_TELLDUS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_TELLDUS_TYPE, ShpTelldusClass))

typedef struct _ShpTelldus ShpTelldus;
typedef struct _ShpTelldusClass ShpTelldusClass;

struct _ShpTelldus {
  ShpSlavePlugin parent;

  /*< protected >*/

  /*< private >*/
  gint device_id;
};

struct _ShpTelldusClass {
  ShpSlavePluginClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_telldus_get_type (void);

G_END_DECLS

#endif /* __SHP_TELLDUS_H__ */
