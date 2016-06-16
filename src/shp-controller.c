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

/*
 * A ShpController monitors the event bus for events and controls all
 * components.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-controller.h"

G_DEFINE_TYPE (ShpController, shp_controller, G_TYPE_OBJECT);

struct _ShpControllerPrivate {
  GSList *rules;
  ShpMessageHandler *handler;
};

static void shp_controller_finalize (GObject * object);
static gboolean shp_controller_start (ShpComponent * component);
static gboolean shp_controller_stop (ShpComponent * component);

static void
shp_controller_class_init (ShpControllerClass * klass)
{
  GObjectClass *gobject_class;
  ShpComponentClass *component_class;

  g_type_class_add_private (klass, sizeof (ShpControllerPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = shp_controller_finalize;

  component_class = SHP_COMPONENT_CLASS (klass);
  component_class->start = shp_controller_start;
  component_class->stop = shp_controller_stop;
}

static void
shp_controller_init (ShpController * self)
{
  ShpControllerPrivate *priv;

  priv = self->priv;

  if (priv);
}

static void
shp_controller_finalize (GObject * object)
{
  ShpControllerPrivate *priv;

  priv = SHP_CONTROLLER (object)->priv;

  if (priv->rules) {
    g_slist_free_full (priv->rules, g_object_unref);
    priv->rules = NULL;
  }
}

static void
message_handler (ShpBus * bus, ShpMessage * message, gpointer user_data)
{
  ShpController *controller = SHP_CONTROLLER (user_data);
  ShpControllerPrivate *priv;
  GSList *rules;

  priv = controller->priv;

  rules = priv->rules;
  while (rules) {
    ShpRule *rule = SHP_RULE (rules->data);

    if (!shp_rule_process_event (rule, message))
      g_debug ("controller: rule not ready yet");
    else
      g_debug ("controller: rule is ready");

    rules = g_slist_next (rules);
  }
}

static gboolean
shp_controller_stop (ShpComponent * component)
{
  ShpController *controller = SHP_CONTROLLER (component);
  ShpControllerPrivate *priv;
  ShpBus *bus;

  g_return_if_fail (IS_SHP_CONTROLLER (controller));

  priv = controller->priv;

  g_debug ("stopping controller");

  bus = shp_component_find_bus (component);
  if (!bus) {
    g_warning ("controller: unable to find bus");
    return FALSE;
  }

  if (priv->handler) {
    shp_bus_remove_async_handler (bus, priv->handler);
    priv->handler = NULL;
  }
  g_object_unref (bus);

  return TRUE;
}

static gboolean
shp_controller_start (ShpComponent * component)
{
  ShpController *controller = SHP_CONTROLLER (component);
  ShpControllerPrivate *priv;
  ShpBus *bus;

  g_return_val_if_fail (IS_SHP_CONTROLLER (controller), FALSE);

  priv = SHP_CONTROLLER (controller)->priv;

  g_debug ("starting controller");

  bus = shp_component_find_bus (component);
  if (!bus) {
    g_warning ("controller: unable to find bus");
    return FALSE;
  }

  priv->handler = shp_bus_add_async_handler (bus, message_handler,
      g_object_ref (component), g_object_unref, NULL);
  g_object_unref (bus);

  return TRUE;
}

/**
 * shp_controller_new:
 *
 * Creates a new instance of #ShpController. Free with g_object_unref() when
 * no-longer needed.
 *
 * Returns: a new instance of #ShpController
 */
ShpController*
shp_controller_new ()
{
  g_debug ("creating new controller");
  return g_object_new (SHP_CONTROLLER_TYPE, NULL);
}

void
shp_controller_add_rule (ShpController * controller, ShpRule * rule)
{
  ShpControllerPrivate *priv;

  g_return_if_fail (IS_SHP_CONTROLLER (controller));
  g_return_if_fail (IS_SHP_RULE (rule));

  priv = controller->priv;

  priv->rules = g_slist_append (priv->rules, rule);
}
