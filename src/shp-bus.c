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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-bus.h"

G_DEFINE_TYPE (ShpBus, shp_bus, G_TYPE_OBJECT);

struct _ShpBusMessageHandler {
  ShpBusMessageHandlerFunc func;
  gpointer data;
  GDestroyNotify notify;
  gchar *source_path;
  gchar *destination_path;
};

struct _ShpBusPrivate {
  ShpBusMessageHandlerFunc func;
  gpointer data;
  GDestroyNotify notify;
  GCond message_cond;
  GMutex message_mutex;
  GThread *thread;
  gboolean thread_stop;
  GQueue *message_queue;
  GSList *message_handlers;
  GMutex mutex;
};

static void shp_bus_finalize (GObject * object);

static void
shp_bus_class_init (ShpBusClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpBusPrivate));

  gobject_class->finalize = shp_bus_finalize;
}

static void
shp_bus_init (ShpBus * self)
{
  ShpBusPrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_BUS_TYPE,
                                            ShpBusPrivate);

  priv = self->priv;

  priv->message_queue = g_queue_new ();

  g_mutex_init (&priv->mutex);
  g_mutex_init (&priv->message_mutex);
  g_cond_init (&priv->message_cond);
}

static void
message_handler_free (gpointer data)
{
  ShpBusMessageHandler *handler = (ShpBusMessageHandler *)data;

  if (handler->data != NULL && handler->notify != NULL)
    handler->notify (handler->data);
  g_free (handler->source_path);
  g_free (handler->destination_path);
  g_slice_free (ShpBusMessageHandler, handler);
}

static void
shp_bus_finalize (GObject * object)
{
  ShpBusPrivate *priv;
  ShpBus *bus = SHP_BUS (object);

  priv = bus->priv;

  if (priv->data != NULL && priv->notify != NULL) {
    priv->notify (priv->data);
    priv->data = NULL;
  }

  if (priv->message_handlers != NULL) {
    g_slist_free_full (priv->message_handlers, message_handler_free);
    priv->message_handlers = NULL;
  }

  g_mutex_clear (&priv->mutex);
  g_queue_free_full (priv->message_queue, g_object_unref);
  g_mutex_clear (&priv->message_mutex);
  g_cond_clear (&priv->message_cond);
}

ShpBus*
shp_bus_new ()
{
  return g_object_new (SHP_BUS_TYPE, NULL);
}

static gpointer
thread_func (gpointer data)
{
  ShpBusPrivate *priv;
  ShpBus *bus = SHP_BUS (data);

  priv = bus->priv;

  while (TRUE) {
    ShpMessage *msg;
    GSList *handlers;
    gchar *msg_str;

    g_mutex_lock (&priv->message_mutex);
    while (!priv->thread_stop &&
        g_queue_get_length (priv->message_queue) == 0) {
      g_cond_wait (&priv->message_cond, &priv->message_mutex);
    }
    if (priv->thread_stop) {
      g_mutex_unlock (&priv->message_mutex);
      break;
    }
    msg = SHP_MESSAGE (g_queue_pop_head (priv->message_queue));
    g_mutex_unlock (&priv->message_mutex);

    g_mutex_lock (&priv->mutex);
    handlers = priv->message_handlers;

    msg_str = shp_message_to_string (msg);
    g_debug ("bus: find handlers for message: %s", msg_str);
    g_free (msg_str);

    /* iterate all handlers and find ones interested in this event */
    while (handlers != NULL) {
      gboolean call_func = TRUE;
      ShpBusMessageHandler *handler = (ShpBusMessageHandler *)(handlers->data);

      g_debug ("bus: checking handler: source: %s, destination: %s",
          handler->source_path, handler->destination_path);

      /* check if source_path is present and supported by current handler */
      if (handler->source_path) {
        const gchar *source_path = shp_message_get_source_path (msg);
        g_debug ("bus: source handler path: %s, source event path: %s",
            handler->source_path, source_path);
        if (!source_path ||
            (g_strcmp0 (handler->source_path, SHP_BUS_EVENT_TYPE_ANY) &&
            g_strcmp0 (handler->source_path, source_path)))
          call_func = FALSE;
      } else if (shp_message_get_source_path (msg) != NULL)
        call_func = FALSE;

      /* check if destination_path is present and supported by current
       * handler */
      if (handler->destination_path) {
        const gchar *destination_path = shp_message_get_destination_path (msg);
        g_debug ("bus: destination handler path: %s, destination event path: "
            "%s", handler->destination_path, destination_path);
        if (!destination_path ||
            (g_strcmp0 (handler->destination_path, SHP_BUS_EVENT_TYPE_ANY) &&
            g_strcmp0 (handler->destination_path, destination_path)))
          call_func = FALSE;
      } else if (shp_message_get_destination_path (msg))
        call_func = FALSE;

      if (call_func) {
        g_debug ("bus: handler supports paths: %s, %s", handler->source_path,
            handler->destination_path);
        handler->func (bus, msg, handler->data);
      } else
        g_debug ("bus: handler does not support paths: %s, %s",
            handler->source_path, handler->destination_path);

      handlers = g_slist_next (handlers);
    }

    g_mutex_unlock (&priv->mutex);
    g_object_unref (msg);
  }

  g_object_unref (bus);
  return NULL;
}

gboolean
shp_bus_start (ShpBus * bus)
{
  ShpBusPrivate *priv;

  g_return_val_if_fail (IS_SHP_BUS (bus), FALSE);

  priv = bus->priv;

  if (priv->thread != NULL) {
    g_warning ("bus already started");
    return FALSE;
  }

  priv->thread_stop = FALSE;
  priv->thread = g_thread_new (NULL, thread_func, g_object_ref (bus));

  return TRUE;
}

gboolean
shp_bus_stop (ShpBus * bus)
{
  ShpBusPrivate *priv;

  g_return_val_if_fail (IS_SHP_BUS (bus), FALSE);

  priv = bus->priv;

  if (priv->thread == NULL) {
    g_warning ("bus not started");
    return FALSE;
  }

  g_mutex_lock (&priv->message_mutex);
  priv->thread_stop = TRUE;
  g_cond_signal (&priv->message_cond);
  g_mutex_unlock (&priv->message_mutex);

  g_thread_join (priv->thread);
  priv->thread = NULL;

  return TRUE;
}

gboolean
shp_bus_post (ShpBus *bus, ShpMessage *message)
{
  ShpBusPrivate *priv;
  gchar *msg_str;

  g_return_val_if_fail (IS_SHP_BUS (bus), FALSE);
  g_return_val_if_fail (IS_SHP_MESSAGE (message), FALSE);

  msg_str = shp_message_to_string (message);
  g_debug ("posted message: %s", msg_str);
  g_free (msg_str);

  priv = bus->priv;

  if (priv->thread == NULL) {
    g_warning ("bus not started yet, ignoring message");
    g_object_unref (message);
    return FALSE;
  }

  if (priv->func != NULL) {
    priv->func (bus, message, priv->data);
  }

  g_mutex_lock (&priv->message_mutex);
  g_queue_push_tail (priv->message_queue, message);
  g_cond_signal (&priv->message_cond);
  g_mutex_unlock (&priv->message_mutex);

  return TRUE;
}

void
shp_bus_set_sync_handler (ShpBus *bus, ShpBusMessageHandlerFunc func,
    gpointer user_data, GDestroyNotify notify)
{
  ShpBusPrivate *priv;

  g_return_if_fail (IS_SHP_BUS (bus));

  priv = bus->priv;

  if (priv->data != NULL && priv->notify != NULL) {
    priv->notify (priv->data);
    priv->data = NULL;
  }
  priv->func = func;
  priv->data = user_data;
  priv->notify = notify;
}

ShpBusMessageHandler*
shp_bus_add_async_handler (ShpBus *bus, ShpBusMessageHandlerFunc func,
    gpointer user_data, GDestroyNotify notify, const gchar * source_path,
    const gchar * destination_path)
{
  ShpBusPrivate *priv;
  ShpBusMessageHandler *handler;

  g_return_if_fail (IS_SHP_BUS (bus));

  priv = bus->priv;

  g_debug ("bus: registering handler for paths: %s, %s", source_path,
      destination_path);

  handler = g_slice_new (ShpBusMessageHandler);
  handler->func = func;
  handler->data = user_data;
  handler->notify = notify;
  handler->source_path = g_strdup (source_path);
  handler->destination_path = g_strdup (destination_path);

  g_mutex_lock (&priv->mutex);
  priv->message_handlers = g_slist_append (priv->message_handlers, handler);
  g_mutex_unlock (&priv->mutex);

  return handler;
}

void
shp_bus_remove_async_handler (ShpBus * bus, ShpBusMessageHandler * handler)
{
  ShpBusPrivate *priv;

  g_return_if_fail (IS_SHP_BUS (bus));

  priv = bus->priv;

  g_mutex_lock (&priv->mutex);
  priv->message_handlers = g_slist_remove (priv->message_handlers, handler);
  g_mutex_unlock (&priv->mutex);
}
