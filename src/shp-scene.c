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
 * ShpScene groups events so that thay can be activated simultaneously.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-scene.h"

enum
{
  PROP_0,
  PROP_BUS,
  PROP_LAST
};

G_DEFINE_TYPE (ShpScene, shp_scene, G_TYPE_OBJECT);

struct _ShpScenePrivate {
  ShpBus *bus;
  GSList *events;
};

static void shp_scene_finalize (GObject * object);
static void shp_scene_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_scene_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
shp_scene_class_init (ShpSceneClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpScenePrivate));

  gobject_class->finalize = shp_scene_finalize;
  gobject_class->set_property = shp_scene_set_property;
  gobject_class->get_property = shp_scene_get_property;

  g_object_class_install_property (gobject_class, PROP_BUS,
      g_param_spec_object ("bus", "The Bus",
          "Event Bus used by components for exchanging messages",
          SHP_BUS_TYPE, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
}

static void
shp_scene_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpScenePrivate *priv;
  ShpScene *scene = SHP_SCENE (object);

  priv = scene->priv;

  switch (propid) {
    case PROP_BUS:
      g_value_set_object (value, priv->bus);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_scene_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  ShpScenePrivate *priv;
  ShpScene *scene = SHP_SCENE (object);

  priv = scene->priv;

  switch (propid) {
    case PROP_BUS:
      if (priv->bus)
        g_object_unref (priv->bus);
      priv->bus = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_scene_init (ShpScene * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_SCENE_TYPE,
                                            ShpScenePrivate);
}

static void
shp_scene_finalize (GObject * object)
{
  ShpScenePrivate *priv;
  ShpScene *self = SHP_SCENE (object);

  priv = self->priv;

  if (priv->bus) {
    g_object_unref (priv->bus);
    priv->bus = NULL;
  }

  if (priv->events != NULL) {
    g_slist_free_full (priv->events, g_object_unref);
    priv->events = NULL;
  }
}

/**
 * shp_scene_new:
 *
 * Creates a new instance of #ShpScene. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpScene
 */
ShpScene*
shp_scene_new (ShpBus * bus)
{
  return g_object_new (SHP_SCENE_TYPE, "bus", bus, NULL);
}

void
shp_scene_add_event (ShpScene * scene, ShpMessage * event)
{
  ShpScenePrivate *priv;

  g_return_if_fail (IS_SHP_SCENE (scene));
  g_return_if_fail (IS_SHP_MESSAGE (event));

  priv = scene->priv;

  priv->events = g_slist_append (priv->events, event);
}

gboolean
shp_scene_activate (ShpScene * scene)
{
  ShpScenePrivate *priv;
  GSList *events;
  gboolean result = TRUE;

  g_return_val_if_fail (IS_SHP_SCENE (scene), FALSE);

  priv = scene->priv;

  if (!priv->bus) {
    g_warning ("scene: no bus installed");
    return FALSE;
  }

  events = priv->events;
  while (events != NULL) {
    ShpMessage *event = SHP_MESSAGE (events->data);
    if (!shp_bus_post (priv->bus, g_object_ref (event))) {
      g_warning ("unable to post event, scene will not be complete");
      result = FALSE;
      /* keep trying with other event from the scene */
    }
    events = g_slist_next (events);
  }

  return result;
}

const GSList*
shp_scene_get_events (ShpScene * scene)
{
  ShpScenePrivate *priv;

  g_return_val_if_fail (IS_SHP_SCENE (scene), NULL);

  priv = scene->priv;

  return priv->events;
}
