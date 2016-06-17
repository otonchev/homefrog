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
 * A ShpGroup represents a group of components, allowing them to be managed as a
 * group.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-group.h"
#include "shp-plugin.h"

G_DEFINE_TYPE (ShpGroup, shp_group, G_TYPE_OBJECT);

struct _ShpGroupPrivate {
  GSList *components;
  GMutex mutex;
};

static void shp_group_finalize (GObject * object);
static gboolean shp_group_start (ShpComponent * component);
static gboolean shp_group_stop (ShpComponent * component);

static void
shp_group_class_init (ShpGroupClass * klass)
{
  GObjectClass *gobject_class;
  ShpComponentClass *component_class;

  g_type_class_add_private (klass, sizeof (ShpGroupPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = shp_group_finalize;

  component_class = SHP_COMPONENT_CLASS (klass);
  component_class->start = shp_group_start;
  component_class->stop = shp_group_stop;
}

static void
shp_group_init (ShpGroup * self)
{
  ShpGroupPrivate *priv;

  priv = self->priv;

  g_mutex_init (&priv->mutex);
}

static void
shp_group_finalize (GObject * object)
{
  ShpGroupPrivate *priv;

  priv = SHP_GROUP (object)->priv;

  if (priv->components != NULL) {
    g_slist_free_full (priv->components, g_object_unref);
    priv->components = NULL;
  }

  g_mutex_clear (&priv->mutex);
}

static gboolean
shp_group_stop (ShpComponent * component)
{
  ShpGroup *group = SHP_GROUP (component);
  ShpGroupPrivate *priv;
  GSList *components;
  gboolean result = TRUE;

  g_return_if_fail (IS_SHP_GROUP (group));

  priv = group->priv;

  g_debug ("stopping group");

  /* stop all components first */
  g_mutex_lock (&priv->mutex);
  components = priv->components;
  while (components != NULL) {
    ShpComponent *component = SHP_COMPONENT (components->data);
    if (!shp_component_stop (component)) {
      g_warning ("unable to stop component");
      result = FALSE;
      break;
    }
    components = g_slist_next (components);
  }
  g_mutex_unlock (&priv->mutex);

  return result;
}

static gboolean
shp_group_start (ShpComponent * component)
{
  ShpGroup *group = SHP_GROUP (component);
  ShpGroupPrivate *priv;
  GSList *components;
  gboolean result = TRUE;

  g_return_val_if_fail (IS_SHP_GROUP (group), FALSE);

  priv = SHP_GROUP (group)->priv;

  g_debug ("starting group");

  /* start all sub-components first */
  g_mutex_lock (&priv->mutex);
  components = priv->components;
  while (components != NULL) {
    ShpComponent *component = SHP_COMPONENT (components->data);
    if (!shp_component_start (component)) {
      g_warning ("unable to start component");
      result = FALSE;
      break;
    }
    components = g_slist_next (components);
  }

  /* send status updates from all contained plugins */
  components = priv->components;
  while (components != NULL) {
    ShpComponent *component = SHP_COMPONENT (components->data);
    if (IS_SHP_PLUGIN (component)) {
      shp_plugin_status_update (SHP_PLUGIN (component));
    }
    components = g_slist_next (components);
  }
  g_mutex_unlock (&priv->mutex);

  return result;
}

/**
 * shp_group_new:
 *
 * Creates a new instance of #ShpGroup. Free with g_object_unref() when
 * no-longer needed.
 *
 * Returns: a new instance of #ShpGroup
 */
ShpGroup*
shp_group_new (ShpBus * bus)
{
  g_debug ("creating new group");
  return g_object_new (SHP_GROUP_TYPE, "bus", bus, NULL);
}

/**
 * shp_group_add_plugin:
 * @group: a #ShpGroup
 * @plugin: a #ShpPlugin
 *
 * Adds @plugin to @group. Note that plugins will be executted in the order they
 * are added to the group.
 *
 * This function takes ownership of @plugin.
 *
 * Returns: TRUE on success and FALSE otherwise
 */
gboolean
shp_group_add (ShpGroup * group, ShpComponent * component)
{
  ShpGroupPrivate *priv;

  g_return_val_if_fail (IS_SHP_GROUP (group), FALSE);
  g_return_val_if_fail (IS_SHP_COMPONENT (component), FALSE);

  g_debug ("adding component to group");

  priv = SHP_GROUP (group)->priv;

  g_mutex_lock (&priv->mutex);
  priv->components = g_slist_append (priv->components, component);
  g_mutex_unlock (&priv->mutex);

  return TRUE;
}
