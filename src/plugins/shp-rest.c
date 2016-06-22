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
#include "../shp-http.h"
#include "shp-rest.h"
#include "../shp-json.h"

#define NAME "rest"

#define DEFAULT_PORT 8080

#define MAX_HISTORY_SIZE 100

enum
{
  SIGNAL_ADD_PATH,
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_HTTP,
  PROP_LAST
};

static void add_device_path (ShpRest * rest, gchar * path);

static gboolean shp_rest_plugin_start (ShpComponent * component);
static gboolean shp_rest_plugin_stop (ShpComponent * component);

static void handler (ShpHttpRequest type, const gchar * path,
    const gchar * query, GSocketConnection * connection, gpointer user_data);

static void message_received_any (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message);

static void shp_rest_finalize (GObject * object);
static void shp_rest_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_rest_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpRest, shp_rest, SHP_SLAVE_PLUGIN_TYPE);

static guint shp_rest_signals[LAST_SIGNAL] = { 0 };

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

  shp_rest_signals[SIGNAL_ADD_PATH] = g_signal_new ("add-device-path",
      G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (ShpRestClass, add_device_path),
      NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 1, G_TYPE_STRING);

  klass->add_device_path = add_device_path;

  SHP_SLAVE_PLUGIN_CLASS (klass)->message_received_any = message_received_any;

  g_object_class_install_property (gobject_class, PROP_HTTP,
      g_param_spec_object ("http", "The HTTP daemon",
          "The HTTP daemon", SHP_HTTP_TYPE, G_PARAM_READWRITE));
}

static void
shp_rest_init (ShpRest * self)
{
  self->port = DEFAULT_PORT;
  self->devices = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      g_object_unref);
  self->history = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      (GDestroyNotify) g_ptr_array_unref);
  g_mutex_init (&self->mutex);
}

static void
shp_rest_finalize (GObject * object)
{
  ShpRest *self = SHP_REST (object);
  g_hash_table_unref (self->devices);
  g_hash_table_unref (self->history);
  g_object_unref (self->http);
  g_mutex_clear (&self->mutex);
}

static void
shp_rest_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpRest *self = SHP_REST (object);

  switch (propid) {
    case PROP_HTTP:
      g_value_set_object (value, self->http);
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
    case PROP_HTTP:
      if (self->http)
        g_object_unref (self->http);
      self->http = g_value_get_object (value);
      if (self->http)
        shp_http_add_path (self->http, "/home*", handler, g_object_ref (self),
            g_object_unref);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
add_device_path (ShpRest * self, gchar * path)
{
  g_return_if_fail (IS_SHP_REST (self));

  g_debug ("rest: adding path: %s", path);

  if (self->http)
    shp_http_add_path (self->http, path, handler, g_object_ref (self),
        g_object_unref);
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
  ShpJsonNode *node;
};

static void
json_builder_add_event_elements (const gchar * name, const GValue * value,
    gpointer user_data)
{
  ShpJsonNode *node = (ShpJsonNode *)user_data;
  ShpJsonNode *child = NULL;

  switch (G_VALUE_TYPE (value)) {
    case G_TYPE_INT:
      child = shp_json_node_new_number (name, g_value_get_int (value));
      break;
    case G_TYPE_DOUBLE:
      child = shp_json_node_new_number (name, g_value_get_double (value));
      break;
    case G_TYPE_BOOLEAN:
      child = shp_json_node_new_boolean (name, g_value_get_boolean (value));
      break;
    case G_TYPE_STRING:
      child = shp_json_node_new_string (name, g_value_get_string (value));
      break;
    default:
      g_debug ("rest: not supported type: %s",
          g_type_name (G_VALUE_TYPE (value)));
      break;
  }

  if (child)
    shp_json_node_append_element (node, child);
}

static void
json_builder_add_device_objects (gpointer key, gpointer val, gpointer data)
{
  ResponseData *response_data = (ResponseData *)data;
  gchar *path = key;
  ShpMessage *event = SHP_MESSAGE (val);
  ShpJsonNode *node;

  if (!g_str_has_prefix (path, response_data->path))
    return;

  node = shp_json_node_new_object (shp_message_get_source_path (event));
  shp_json_node_append_element (response_data->node, node);

  shp_message_foreach (event, json_builder_add_event_elements, node);
}

static void
handler (ShpHttpRequest type, const gchar * path, const gchar * query,
    GSocketConnection * connection, gpointer user_data)
{
  ShpRest *self = SHP_REST (user_data);
  GOutputStream *out;
  GString *output;
  gchar *output_str;

  g_debug ("rest: incomming connection");

  out = g_io_stream_get_output_stream (G_IO_STREAM (connection));

  g_debug ("rest: path: %s, query: %s", path, query);

  output = g_string_new (NULL);

  if (query == NULL) {
    /* no query, just collect data for all sensors belonging to requested
     * path */
    ShpJsonNode *node;
    ResponseData *response_data;

    g_string_append (output, "HTTP/1.0 200 OK\r\n");
    g_string_append (output, "Content-Type: application/json\r\n\r\n");

    node = shp_json_node_new_object (NULL);

    response_data = g_new0 (ResponseData, 1);
    response_data->node = node;
    response_data->path = (gchar *)path;

    g_mutex_lock (&self->mutex);
    g_hash_table_foreach (self->devices, json_builder_add_device_objects,
        response_data);
    g_mutex_unlock (&self->mutex);

    g_free (response_data);

    g_string_append (output, shp_json_node_to_string (node));
    shp_json_node_free (node);
  } else {
    /* create request based on input command */
    ShpMessage *event;

    /* first check if requested path is valid and we know about it */
    g_mutex_lock (&self->mutex);
    event = g_hash_table_lookup (self->devices, path);
    if (event)
      event = g_object_ref (event);
    g_mutex_unlock (&self->mutex);

    if (!event) {
      send_error (out, 400, "Invalid request");
      goto out;
    }

    if (!g_strcmp0 (query, "history")) {
      ShpJsonNode *node;
      ShpJsonNode *array_node;
      ShpJsonNode *object;
      GPtrArray *arr;

      g_string_append (output, "HTTP/1.0 200 OK\r\n");
      g_string_append (output, "Content-Type: application/json\r\n\r\n");

      node = shp_json_node_new_object (NULL);
      array_node = shp_json_node_new_array (path);
      shp_json_node_append_element (node, array_node);

      object = shp_json_node_new_object (NULL);
      shp_message_foreach (event, json_builder_add_event_elements, object);
      shp_json_node_append_element (array_node, object);

      g_object_unref (event);

      arr = g_hash_table_lookup (self->history, path);
      if (arr) {
        gint i;
        for (i = 0; i < arr->len; i++) {
          event = g_ptr_array_index (arr, i);
          object = shp_json_node_new_object (NULL);
          shp_message_foreach (event, json_builder_add_event_elements, object);
          shp_json_node_append_element (array_node, object);
        }
      }

      g_string_append (output, shp_json_node_to_string (node));
      shp_json_node_free (node);
    } else {
      gchar *event_str;

      g_object_unref (event);

      event = shp_message_new_command_from_string (path, query);
      if (!event) {
        send_error (out, 400, "Invalid request");
        goto out;
      }

      g_string_append (output, "HTTP/1.0 200 OK\r\n");
      g_string_append (output, "Content-Type: text/plain\r\n\r\n");

      event_str = shp_message_to_string (event);
      g_debug ("rest: about to post: %s", event_str);
      g_free (event_str);

      shp_component_post_message (SHP_COMPONENT (self), event);
    }
  }

  output_str = g_string_free (output, FALSE);
  g_output_stream_write_all (out, output_str, strlen (output_str),
      NULL, NULL, NULL);
  g_free (output_str);
out:
  return;
}

static gboolean
shp_rest_plugin_start (ShpComponent * component)
{
  ShpRest *self;
  ShpComponentClass *klass;

  self = SHP_REST (component);
  klass = SHP_COMPONENT_CLASS (shp_rest_parent_class);

  if (!klass->start (component))
    return FALSE;

  shp_http_start (self->http);

  /* chain up to parent now */
  return TRUE;
}

static gboolean
shp_rest_plugin_stop (ShpComponent * component)
{
  ShpRest *self;
  ShpComponentClass *klass;

  self = SHP_REST (component);
  klass = SHP_COMPONENT_CLASS (shp_rest_parent_class);

  shp_http_stop (self->http);

  /* chain up to parent now */
  return klass->stop (component);
}

static void
message_received_any (ShpSlavePlugin * plugin, ShpBus * bus,
      ShpMessage * message)
{
  ShpRest *self = SHP_REST (plugin);
  const gchar *source_path;
  ShpMessage *new_event;
  ShpMessage *old_event;
  gchar *timestamp;
  time_t t = time (NULL);
  struct tm *tm_struct = localtime (&t);

  g_debug ("rest: received event");

  source_path = shp_message_get_source_path (message);
  if (!source_path) {
    g_debug ("rest: command event, not of interest");
    return;
  }

  g_debug ("rest: storing event");

  /* copy event and add timestamp to it */
  new_event = shp_message_copy (message);
  timestamp = g_strdup_printf ("%d-%02d-%02d %02d-%02d-%02d",
      tm_struct->tm_year + 1900, tm_struct->tm_mon + 1, tm_struct->tm_mday,
      tm_struct->tm_hour, tm_struct->tm_min, tm_struct->tm_sec);
  shp_message_add_string (new_event, "rest.timestamp", timestamp);
  g_free (timestamp);

  g_mutex_lock (&self->mutex);

  /* remember old event if present */
  old_event = g_hash_table_lookup (self->devices, source_path);
  if (old_event) {
    GPtrArray *arr;

    if (!g_hash_table_contains (self->history, source_path)) {
      arr = g_ptr_array_new ();
      g_hash_table_insert (self->history, g_strdup (source_path), arr);
    } else
      arr = g_hash_table_lookup (self->history, source_path);

    g_ptr_array_insert (arr, 0, g_object_ref (old_event));
    if (arr->len > MAX_HISTORY_SIZE)
      g_ptr_array_set_size (arr, MAX_HISTORY_SIZE);
  }

  /* replace olf event with new one */
  g_hash_table_insert (self->devices, g_strdup (source_path),
      new_event);

  g_mutex_unlock (&self->mutex);
}

static void
plugin_register (void)
{
  g_debug ("%s: loading plugin", NAME);
  shp_plugin_factory_register (NAME, SHP_REST_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
