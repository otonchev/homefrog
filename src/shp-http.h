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
 * Http daemon
 */

#ifndef __SHP_HTTP_H__
#define __SHP_HTTP_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define SHP_HTTP_TYPE (shp_http_get_type ())
#define SHP_HTTP(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_HTTP_TYPE,ShpHttp))
#define SHP_HTTP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_HTTP_TYPE, ShpHttpClass))
#define IS_SHP_HTTP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_HTTP_TYPE))
#define IS_SHP_HTTP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_HTTP_TYPE))
#define SHP_HTTP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_HTTP_TYPE, ShpHttpClass))

typedef enum ShpHttpRequest {
    SHP_HTTP_GET,
    SHP_HTTP_POST,
    SHP_HTTP_PUT,
    SHP_HTTP_DELETE
} ShpHttpRequest;

typedef struct _ShpHttp ShpHttp;
typedef struct _ShpHttpPrivate ShpHttpPrivate;
typedef struct _ShpHttpClass ShpHttpClass;
typedef struct _ShpHttpHandler ShpHttpHandler;

typedef void (*ShpHttpHandlerFunc) (ShpHttpRequest type, const gchar * path,
    const gchar * options, GSocketConnection * connection,
    gpointer user_data);

struct _ShpHttp {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpHttpPrivate *priv;
};

struct _ShpHttpClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpHttp* shp_http_new (guint port);

void shp_http_add_path (ShpHttp *http, const gchar * path,
    ShpHttpHandlerFunc func, gpointer user_data, GDestroyNotify notify);

gboolean shp_http_start (ShpHttp *http);
gboolean shp_http_stop (ShpHttp *http);

GType shp_http_get_type (void);

G_END_DECLS

#endif /* __SHP_HTTP_H__ */
