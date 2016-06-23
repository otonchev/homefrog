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
 * This plugin provides REST API for managing devices remotely.
 */

#ifndef __SHP_REST_H__
#define __SHP_REST_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-slave-plugin.h"

G_BEGIN_DECLS

#define SHP_REST_TYPE (shp_rest_get_type ())
#define SHP_REST(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_REST_TYPE,ShpRest))
#define SHP_REST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_REST_TYPE, ShpRestClass))
#define IS_SHP_REST(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_REST_TYPE))
#define IS_SHP_REST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_REST_TYPE))
#define SHP_REST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_REST_TYPE, ShpRestClass))

typedef struct _ShpRest ShpRest;
typedef struct _ShpRestClass ShpRestClass;

struct _ShpRest {
  ShpSlavePlugin parent;

  /*< protected >*/

  /*< private >*/
  GHashTable *devices;
  GHashTable *history;
  gint port;
  GMutex mutex;
  ShpHttp *http;
  gchar *config_file;
};

struct _ShpRestClass {
  ShpSlavePluginClass parent_class;

  /*< private >*/
  void (*add_device_path) (ShpRest * rest, gchar * path);
};

void shp_plugin_register (void);

GType shp_rest_get_type (void);

G_END_DECLS

#endif /* __SHP_REST_H__ */
