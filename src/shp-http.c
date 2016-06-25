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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "shp-http.h"

G_DEFINE_TYPE (ShpHttp, shp_http, G_TYPE_OBJECT);

#define DEFAULT_PORT 8080

enum
{
  PROP_0,
  PROP_PORT,
  PROP_LAST
};

struct _ShpHttpHandler {
  ShpHttpHandlerFunc func;
  gpointer user_data;
  GDestroyNotify notify;
  gchar *path;
};

struct _ShpHttpPrivate {
  guint port;
  GHashTable *handlers;
  GSList *wild_handlers;
  GSocketService *service;
};

static void shp_http_finalize (GObject * object);
static void shp_http_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_http_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
free_handler_cb (gpointer data)
{
  ShpHttpHandler *handler = (ShpHttpHandler *)data;

  if (handler->notify) {
    handler->notify (handler->user_data);
    handler->user_data = NULL;
  }
  g_free (handler->path);
  handler->path = NULL;

  g_free (handler);
}

static void
shp_http_class_init (ShpHttpClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpHttpPrivate));

  gobject_class->finalize = shp_http_finalize;
  gobject_class->set_property = shp_http_set_property;
  gobject_class->get_property = shp_http_get_property;

  g_object_class_install_property (gobject_class, PROP_PORT,
      g_param_spec_int ("port", "Port number",
          "Port to listen on for new connections",
          0, G_MAXINT, DEFAULT_PORT, G_PARAM_READWRITE));
}

static gboolean
path_equal (const gchar *path1, const gchar *path2)
{
  GRegex *regex;
  GMatchInfo *match_info;
  gboolean res = FALSE;

  g_debug ("http: matching %s and %s", path1, path2);

  regex = g_regex_new (path1, 0, 0, NULL);
  g_regex_match (regex, path2, 0, &match_info);
  if (g_match_info_matches (match_info))
    res = TRUE;

  g_match_info_free (match_info);
  g_regex_unref (regex);

  return res;
}

static void
shp_http_init (ShpHttp * self)
{
  ShpHttpPrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_HTTP_TYPE,
                                            ShpHttpPrivate);

  priv = self->priv;

  priv->port = DEFAULT_PORT;
  priv->handlers = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_handler_cb);
}

static void
shp_http_finalize (GObject * object)
{
  ShpHttpPrivate *priv;
  ShpHttp *http = SHP_HTTP (object);

  priv = http->priv;

  g_hash_table_unref (priv->handlers);
  g_slist_free_full (priv->wild_handlers, free_handler_cb);
}

static void
shp_http_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpHttpPrivate *priv;
  ShpHttp *http = SHP_HTTP (object);

  priv = http->priv;

  switch (propid) {
    case PROP_PORT:
      g_value_set_int (value, priv->port);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_http_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  ShpHttpPrivate *priv;
  ShpHttp *http = SHP_HTTP (object);

  priv = http->priv;

  switch (propid) {
    case PROP_PORT:
      priv->port = g_value_get_int (value);
      g_debug ("http: setting port: %d", priv->port);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

ShpHttp*
shp_http_new (guint port)
{
  return g_object_new (SHP_HTTP_TYPE, "port", port, NULL);
}

static void
send_error (GOutputStream * out, gint error_code, const gchar * reason)
{
  char *res;

  res = g_strdup_printf ("HTTP/1.0 %d %s\r\n\r\n"
                         "<html><head><title>%d %s</title></head>"
                         "<body>%s</body></html>",
                         error_code, reason,
                         error_code, reason,
                         reason);
  g_output_stream_write_all (out, res, strlen (res), NULL, NULL, NULL);
  g_free (res);
}

static gboolean
handler (GThreadedSocketService * service, GSocketConnection * connection,
    GSocketListener * listener, gpointer user_data)
{
  ShpHttpPrivate *priv;
  ShpHttp *self = SHP_HTTP (user_data);
  GOutputStream *out;
  GInputStream *in;
  GDataInputStream *data;
  gchar *line;
  gchar *escaped;
  gchar *path;
  gchar *query;
  gchar *tmp;
  ShpHttpHandler *handler;
  ShpHttpRequest request;

  g_debug ("http: incomming connection");

  priv = self->priv;
  if (priv);

  in = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  out = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  data = g_data_input_stream_new (in);

  line = g_data_input_stream_read_line (data, NULL, NULL, NULL);

  if (line == NULL) {
    send_error (out, 400, "Invalid request");
    goto out;
  }

  if (!g_str_has_prefix (line, "GET ") && !g_str_has_prefix (line, "POST ")) {
    send_error (out, 501, "Only GET and POST method is supported");
    goto out;
  }

  if (g_str_has_prefix (line, "GET ")) {
    request = SHP_HTTP_GET;
    escaped = line + 4; /* Skip "GET " */
  } else {
    request = SHP_HTTP_POST;
    escaped = line + 5; /* Skip "POST " */
  }

  tmp = strchr (escaped, ' ');
  if (tmp != NULL)
    *tmp = 0;

  query = strchr (escaped, '?');
  if (query != NULL)
    *query++ = 0;

  path = g_uri_unescape_string (escaped, NULL);
  g_debug ("http: path: %s, query: %s", path, query);

  handler = g_hash_table_lookup (priv->handlers, path);
  if (!handler) {
    GSList *tmp = priv->wild_handlers;

    while (tmp) {
      ShpHttpHandler *wild_handler = tmp->data;
      g_assert (g_strrstr (wild_handler->path, "*"));
      if (path_equal (wild_handler->path, path)) {
        handler = wild_handler;
        break;
      }
      tmp = g_slist_next (tmp);
    }

    if (!handler) {
      send_error (out, 400, "Invalid request");
      g_free (path);
      goto out;
    }
  }

  handler->func (request, path, query, connection, handler->user_data);
  g_free (path);

out:
  g_free (line);
  g_object_unref (data);
  return TRUE;
}

gboolean
shp_http_start (ShpHttp * self)
{
  ShpHttpPrivate *priv;
  GSocketService *service;
  GError *error = NULL;

  priv = self->priv;

  service = g_threaded_socket_service_new (50);
  if (!g_socket_listener_add_inet_port (G_SOCKET_LISTENER (service),
      priv->port, NULL, &error)) {
    g_warning ("rest: unable to start service: %s\n", error->message);
    g_clear_error (&error);
    g_socket_service_stop (service);
    g_object_unref (service);
    return FALSE;
  }

  g_debug ("http: Http server listening on port %d", priv->port);

  g_signal_connect (service, "run", G_CALLBACK (handler), self);

  priv->service = service;

  return TRUE;
}

gboolean
shp_http_stop (ShpHttp * http)
{
  ShpHttpPrivate *priv;

  priv = http->priv;

  g_socket_service_stop (priv->service);
  g_object_unref (priv->service);
  priv->service = NULL;

  return TRUE;
}

void
shp_http_add_path (ShpHttp *http, const gchar * path, ShpHttpHandlerFunc func,
    gpointer user_data, GDestroyNotify notify)
{
  ShpHttpPrivate *priv;
  ShpHttpHandler *handler;

  g_return_val_if_fail (IS_SHP_HTTP (http), NULL);

  priv = http->priv;

  handler = g_new0 (ShpHttpHandler, 1);
  handler->func = func;
  handler->user_data = user_data;
  handler->notify = notify;
  handler->path = g_strdup (path);

  if (g_strrstr (path, "*"))
    priv->wild_handlers = g_slist_append (priv->wild_handlers, handler);
  else
    g_hash_table_insert (priv->handlers, g_strdup (path), handler);
}
