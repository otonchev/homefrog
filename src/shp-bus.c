/* The SmartHomeProject.org
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

struct _ShpMessageHandler {
  ShpMessageHandlerFunc func;
  gpointer data;
  GDestroyNotify notify;
  gchar *source_path;
};

struct _ShpBusPrivate {
  ShpMessageHandlerFunc func;
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
  ShpMessageHandler *handler = (ShpMessageHandler *)data;

  if (handler->data != NULL && handler->notify != NULL)
    handler->notify (handler->data);
  g_free (handler->source_path);
  g_slice_free (ShpMessageHandler, handler);
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

    while (handlers != NULL) {
      gboolean call_func = TRUE;
      ShpMessageHandler *handler = (ShpMessageHandler *)(handlers->data);

      /* check if source_path matches if present */
      if (handler->source_path) {
        const gchar *source_path = shp_message_get_source_path (msg);
        if (g_strcmp0 (handler->source_path, source_path))
          call_func = FALSE;
      }

      if (call_func)
        handler->func (bus, msg, handler->data);
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

  g_return_val_if_fail (IS_SHP_BUS (bus), FALSE);
  g_return_val_if_fail (IS_SHP_MESSAGE (message), FALSE);

  g_debug ("posted message: %s", shp_message_get_name (message));

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
shp_bus_set_sync_handler (ShpBus *bus, ShpMessageHandlerFunc func,
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

ShpMessageHandler*
shp_bus_add_async_handler (ShpBus *bus, ShpMessageHandlerFunc func,
    gpointer user_data, GDestroyNotify notify, const gchar * source_path)
{
  ShpBusPrivate *priv;
  ShpMessageHandler *handler;

  g_return_if_fail (IS_SHP_BUS (bus));

  priv = bus->priv;

  handler = g_slice_new (ShpMessageHandler);
  handler->func = func;
  handler->data = user_data;
  handler->notify = notify;
  handler->source_path = g_strdup (source_path);

  g_mutex_lock (&priv->mutex);
  priv->message_handlers = g_slist_append (priv->message_handlers, handler);
  g_mutex_unlock (&priv->mutex);

  return handler;
}

void
shp_bus_remove_async_handler (ShpBus * bus, ShpMessageHandler * handler)
{
  ShpBusPrivate *priv;

  g_return_if_fail (IS_SHP_BUS (bus));

  priv = bus->priv;

  g_mutex_lock (&priv->mutex);
  priv->message_handlers = g_slist_remove (priv->message_handlers, handler);
  g_mutex_unlock (&priv->mutex);
}
