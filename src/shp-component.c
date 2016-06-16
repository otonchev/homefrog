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
 * Abstract base class for all components.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-component.h"

#define DEFAULT_NAME "<no_name>"
#define DEFAULT_PATH "/empty path"

enum
{
  PROP_0,
  PROP_BUS,
  PROP_NAME,
  PROP_PATH,
  PROP_LAST
};

static void shp_component_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_component_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpComponent, shp_component, G_TYPE_OBJECT);

struct _ShpComponentPrivate {
  ShpComponent *parent;
  ShpBus *bus;
  gboolean started;
  gchar *name;
  gchar *path;
};

static void shp_component_finalize (GObject * object);

static void
shp_component_class_init (ShpComponentClass * klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ShpComponentPrivate));

  gobject_class = G_OBJECT_CLASS (klass);

  g_object_class_install_property (gobject_class, PROP_BUS,
      g_param_spec_object ("bus", "The Bus",
          "Event Bus used by components for exchanging messages",
          SHP_BUS_TYPE, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_NAME,
      g_param_spec_string ("name", "Component name",
          "The name of the component", DEFAULT_NAME,
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_PATH,
      g_param_spec_string ("path", "Component path",
          "Path to the component", DEFAULT_PATH,
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  gobject_class->finalize = shp_component_finalize;
  gobject_class->set_property = shp_component_set_property;
  gobject_class->get_property = shp_component_get_property;
}

static void
shp_component_init (ShpComponent * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_COMPONENT_TYPE,
                                            ShpComponentPrivate);
  self->priv->name = g_strdup (DEFAULT_NAME);
  self->priv->path = g_strdup (DEFAULT_PATH);
}

static void
shp_component_finalize (GObject * object)
{
  ShpComponentPrivate *priv;

  priv = SHP_COMPONENT (object)->priv;

  if (priv->bus) {
    g_object_unref (priv->bus);
    priv->bus = NULL;
  }

  if (priv->parent != NULL) {
    g_object_unref (priv->parent);
    priv->parent = NULL;
  }

  g_free (priv->name);
  priv->name = NULL;
  g_free (priv->path);
  priv->path = NULL;
}

static void
shp_component_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec)
{
  ShpComponent *component = SHP_COMPONENT (object);

  switch (propid) {
    case PROP_BUS:
      g_value_set_object (value, component->priv->bus);
      break;
    case PROP_NAME:
      g_value_set_string (value, component->priv->name);
      break;
    case PROP_PATH:
      g_value_set_string (value, component->priv->path);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_component_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpComponent *component = SHP_COMPONENT (object);

  switch (propid) {
    case PROP_BUS:
      if (component->priv->bus)
        g_object_unref (component->priv->bus);
      component->priv->bus = g_value_get_object (value);
      break;
    case PROP_NAME:
      g_free (component->priv->name);
      component->priv->name = g_strdup (g_value_get_string (value));
      break;
    case PROP_PATH:
      g_free (component->priv->path);
      component->priv->path = g_strdup (g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

/**
 * shp_component_stop:
 * @component: a #ShpComponent
 *
 * De-activates a component previously started with shp_component_start().
 *
 * Returns: TRUE on success and FALSE otherwise
 */
gboolean
shp_component_stop (ShpComponent * component)
{
  ShpComponentPrivate *priv;
  ShpComponentClass *klass;
  gboolean res;

  priv = component->priv;

  if (!priv->started) {
    g_warning ("component not started");
    return FALSE;
  }

  klass = SHP_COMPONENT_GET_CLASS (component);

  if (klass->stop == NULL)
    return FALSE;

  if (priv->bus && !shp_bus_start (priv->bus)) {
    g_warning ("could not start bus");
    return FALSE;
  }

  res = klass->stop (component);
  if (!res)
    shp_bus_stop (priv->bus);

  priv->started = !(res == TRUE);
  return res;
}

/**
 * shp_component_start:
 * @component: a #ShpComponent
 *
 * Activates a component. De-activate with shp_component_stop()
 *
 * Returns: TRUE on success and FALSE otherwise
 */
gboolean
shp_component_start (ShpComponent * component)
{
  ShpComponentPrivate *priv;
  ShpComponentClass *klass;
  gboolean res;

  priv = component->priv;

  if (priv->started) {
    g_warning ("component already started");
    return FALSE;
  }

  klass = SHP_COMPONENT_GET_CLASS (component);

  if (klass->start == NULL)
    return FALSE;

  if (priv->bus)
    shp_bus_stop (priv->bus);

  res = klass->start (component);
  priv->started = (res == TRUE);
  return res;
}

/**
 * shp_component_new:
 *
 * Creates a new instance of #ShpComponent. Free with g_object_unref() when
 * no-longer needed.
 *
 * Returns: a new instance of #ShpComponent
 */
ShpComponent*
shp_component_new (ShpBus * bus)
{
  g_return_val_if_fail (IS_SHP_BUS (bus), NULL);

  g_debug ("creating new component");

  return g_object_new (SHP_COMPONENT_TYPE, "bus", bus, NULL);
}

ShpBus*
shp_component_get_bus (ShpComponent * component)
{
  ShpComponentPrivate *priv;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), FALSE);

  priv = component->priv;

  if (!priv->bus)
    return NULL;
  return g_object_ref (priv->bus);
}

ShpComponent*
shp_component_get_parent (ShpComponent * component)
{
  ShpComponentPrivate *priv;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), NULL);

  priv = component->priv;

  if (!priv->parent)
    return NULL;
  return g_object_ref (priv->parent);
}

void
shp_component_set_parent (ShpComponent * component, ShpComponent * parent)
{
  ShpComponentPrivate *priv;

  g_return_if_fail (IS_SHP_COMPONENT (component));
  g_return_if_fail (IS_SHP_COMPONENT (parent));

  priv = component->priv;

  if (priv->parent != NULL)
    g_object_unref (priv->parent);
  priv->parent = g_object_ref (parent);
}

const gchar*
shp_component_get_name (ShpComponent * component)
{
  ShpComponentPrivate *priv;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), NULL);

  priv = component->priv;

  return priv->name;
}

const gchar*
shp_component_get_path (ShpComponent * component)
{
  ShpComponentPrivate *priv;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), NULL);

  priv = component->priv;

  return priv->path;
}

ShpBus*
shp_component_find_bus (ShpComponent * component)
{
  ShpComponent *current_component = g_object_ref (component);
  ShpBus *bus;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), NULL);

  g_debug ("finding message bus");

  while (!(bus = shp_component_get_bus (current_component))) {
    ShpComponent *parent;

    parent = shp_component_get_parent (current_component);
    g_object_unref (current_component);
    if (!parent) {
      g_warning ("plugin has no parent, could not find bus");
      return FALSE;
    }

    current_component = parent;
  }

  return bus;
}

gboolean
shp_component_post_message (ShpComponent * component,
    ShpMessage * message)
{
  ShpBus *bus;

  g_return_val_if_fail (IS_SHP_COMPONENT (component), FALSE);
  g_return_val_if_fail (IS_SHP_MESSAGE (message), FALSE);

  g_debug ("component posting message");

  bus = shp_component_find_bus (component);
  if (!bus)
    return FALSE;

  if (!shp_bus_post (bus, message)) {
    g_warning ("could not post message on bus");
    g_object_unref (bus);
    return FALSE;
  }

  g_object_unref (bus);
  return TRUE;
}
