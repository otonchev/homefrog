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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>

#include "../shp-message.h"
#include "../shp-plugin-factory.h"
#include "shp-rest.h"

#define NAME "rest"

#define DEFAULT_PORT 8080

enum
{
  PROP_0,
  PROP_PORT,
  PROP_LAST
};

static gboolean shp_rest_plugin_start (ShpComponent * component);
static gboolean shp_rest_plugin_stop (ShpComponent * component);

static void message_received_any (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message);

static void shp_rest_finalize (GObject * object);
static void shp_rest_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_rest_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpRest, shp_rest, SHP_SLAVE_PLUGIN_TYPE);

static void
shp_rest_class_init (ShpRestClass * klass)
{
  GObjectClass *gobject_class;
  ShpComponentClass *component_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_rest_finalize;
  gobject_class->set_property = shp_rest_set_property;
  gobject_class->get_property = shp_rest_get_property;

  component_class = SHP_COMPONENT_CLASS (klass);
  component_class->start = shp_rest_plugin_start;
  component_class->stop = shp_rest_plugin_stop;

  SHP_SLAVE_PLUGIN_CLASS (klass)->message_received_any = message_received_any;

  g_object_class_install_property (gobject_class, PROP_PORT,
      g_param_spec_int ("port", "Port number",
          "Port to listen on for new connections",
          0, G_MAXINT, DEFAULT_PORT, G_PARAM_READWRITE));
}

static void
shp_rest_init (ShpRest * self)
{
  self->port = DEFAULT_PORT;
  self->devices = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  g_mutex_init (&self->mutex);
}

static void
shp_rest_finalize (GObject * object)
{
  ShpRest *self = SHP_REST (object);
  g_hash_table_unref (self->devices);
  g_mutex_clear (&self->mutex);
}

static void
shp_rest_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpRest *self = SHP_REST (object);

  switch (propid) {
    case PROP_PORT:
      g_value_set_int (value, self->port);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_rest_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpRest *self = SHP_REST (object);

  switch (propid) {
    case PROP_PORT:
      self->port = g_value_get_int (value);
      g_debug ("rest: setting port: %d", self->port);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
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

typedef struct _ResponseData ResponseData;
struct _ResponseData
{
  gchar *path;
  GString *output;
};

static void
build_response (gpointer key, gpointer val, gpointer data)
{
  ResponseData *response_data = (ResponseData *)data;
  gchar *path = key;
  ShpMessage *event = SHP_MESSAGE (val);
  gchar *event_str;

  if (!g_str_has_prefix (path, response_data->path))
    return;

  event_str = shp_message_to_string (event);
  g_string_append_printf (response_data->output, "%s\n\r", event_str);
  g_free (event_str);
}

static gboolean
handler (GThreadedSocketService * service, GSocketConnection * connection,
    GSocketListener * listener, gpointer user_data)
{
  ShpRest *self = SHP_REST (user_data);
  GOutputStream *out;
  GInputStream *in;
  GDataInputStream *data;
  gchar *line;
  gchar *escaped;
  gchar *unescaped;
  gchar *query;
  gchar *tmp;
  GString *output;
  gchar *output_str;

  g_debug ("rest: incomming connection");

  in = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  out = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  data = g_data_input_stream_new (in);

  line = g_data_input_stream_read_line (data, NULL, NULL, NULL);

  if (line == NULL) {
    send_error (out, 400, "Invalid request");
    goto out;
  }

  if (!g_str_has_prefix (line, "GET ")) {
    send_error (out, 501, "Only GET method is supported");
    goto out;
  }

  escaped = line + 4; /* Skip "GET " */

  tmp = strchr (escaped, ' ');
  if (tmp != NULL)
    *tmp = 0;

  query = strchr (escaped, '?');
  if (query != NULL)
    *query++ = 0;

  unescaped = g_uri_unescape_string (escaped, NULL);
  g_debug ("rest: path: %s, query: %s", unescaped, query);

  output = g_string_new (NULL);

  if (query == NULL) {
    /* no query, just collect data for all sensors belonging to requested
     * path */
    ResponseData *response_data;

    g_string_append (output, "HTTP/1.0 200 OK\r\n\r\n");

    response_data = g_new0 (ResponseData, 1);
    response_data->output = output;
    response_data->path = unescaped;

    g_mutex_lock (&self->mutex);
    g_hash_table_foreach (self->devices, build_response, response_data);
    g_mutex_unlock (&self->mutex);

    g_free (response_data);
  } else {
    /* create request based on input command */
    ShpMessage *event;
    gchar *event_str;

    /* first check if requested path is valid and we know about it */
    if (!g_hash_table_contains (self->devices, unescaped)) {
      send_error (out, 400, "Invalid request");
      goto out;
    }

    event = shp_message_new_command_from_string (unescaped, query);
    if (!event) {
      send_error (out, 400, "Invalid request");
      goto out;
    }

    g_string_append (output, "HTTP/1.0 200 OK\r\n\r\n");

    event_str = shp_message_to_string (event);
    g_debug ("rest: about to post: %s", event_str);
    g_free (event_str);

    shp_component_post_message (SHP_COMPONENT (self), event);
  }

  g_free (unescaped);
  g_free (line);

  output_str = g_string_free (output, FALSE);
  g_output_stream_write_all (out, output_str, strlen (output_str),
      NULL, NULL, NULL);
  g_free (output_str);

out:
  g_object_unref (data);
  return TRUE;
}

static gboolean
start_service (ShpRest * self)
{
  GSocketService *service;
  GError *error = NULL;

  service = g_threaded_socket_service_new (10);
  if (!g_socket_listener_add_inet_port (G_SOCKET_LISTENER (service),
      self->port, NULL, &error)) {
    g_warning ("rest: unable to start service: %s\n", error->message);
    g_clear_error (&error);
    g_socket_service_stop (service);
    g_object_unref (service);
    return FALSE;
  }

  g_debug ("rest: Http server listening on port %d", self->port);

  g_signal_connect (service, "run", G_CALLBACK (handler), self);

  self->service = service;

  return TRUE;
}

static void
stop_service (ShpRest * self)
{
  g_socket_service_stop (self->service);
  g_object_unref (self->service);
  self->service = NULL;
}

static gboolean
shp_rest_plugin_start (ShpComponent * component)
{
  ShpRest *self;
  ShpComponentClass *klass;

  self = SHP_REST (component);
  klass = SHP_COMPONENT_CLASS (shp_rest_parent_class);

  if (!start_service (self))
    return FALSE;

  /* chain up to parent now */
  return klass->start (component);
}

static gboolean
shp_rest_plugin_stop (ShpComponent * component)
{
  ShpRest *self;
  ShpComponentClass *klass;

  self = SHP_REST (component);
  klass = SHP_COMPONENT_CLASS (shp_rest_parent_class);

  stop_service (self);

  /* chain up to parent now */
  return klass->stop (component);
}

static void
message_received_any (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message)
{
  ShpRest *self = SHP_REST (plugin);
  const gchar *source_path;

  g_debug ("rest: received event");

  if (self);

  source_path = shp_message_get_source_path (message);
  if (!source_path) {
    g_debug ("rest: command event, not of interest");
    return;
  }

  g_debug ("rest: storing event");
  g_mutex_lock (&self->mutex);
  g_hash_table_insert (self->devices, g_strdup (source_path),
      g_object_ref (message));
  g_mutex_unlock (&self->mutex);
}

static void
plugin_register (void)
{
  g_debug ("%s: loading plugin", NAME);
  shp_plugin_factory_register (NAME, SHP_REST_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
