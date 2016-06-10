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
#ifndef __SHP_MYSQL_H__
#define __SHP_MYSQL_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_MYSQL_TYPE (shp_mysql_get_type ())
#define SHP_MYSQL(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_MYSQL_TYPE,ShpMysql))
#define SHP_MYSQL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_MYSQL_TYPE, ShpMysqlClass))
#define IS_SHP_MYSQL(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_MYSQL_TYPE))
#define IS_SHP_MYSQL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_MYSQL_TYPE))
#define SHP_MYSQL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_MYSQL_TYPE, ShpMysqlClass))

typedef struct _ShpMysql ShpMysql;
typedef struct _ShpMysqlClass ShpMysqlClass;

struct _ShpMysql {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  GMainLoop *loop;
  GMainContext *context;
  GThread *thread;
  GMutex mutex;
  GPtrArray *readings;

  gchar *database;
  gchar *username;
  gchar *password;
  gchar *table;
};

struct _ShpMysqlClass {
  ShpPluginClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_mysql_get_type (void);

G_END_DECLS

#endif /* __SHP_MYSQL_H__ */
