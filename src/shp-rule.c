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
 * ShpRule is a set of conditions and a scene to be activated when all
 * conditions are met.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-rule.h"

G_DEFINE_TYPE (ShpRule, shp_rule, G_TYPE_OBJECT);

struct _ShpRulePrivate {
  GHashTable *conditions;
  ShpScene *scene;
};

static void shp_rule_finalize (GObject * object);
static void shp_rule_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_rule_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
shp_rule_class_init (ShpRuleClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpRulePrivate));

  gobject_class->finalize = shp_rule_finalize;
  gobject_class->set_property = shp_rule_set_property;
  gobject_class->get_property = shp_rule_get_property;
}

static void
shp_rule_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
/*
  ShpRulePrivate *priv;
  ShpRule *rule = SHP_RULE (object);

  priv = rule->priv;
*/
  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_rule_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
/*
  ShpRulePrivate *priv;
  ShpRule *rule = SHP_RULE (object);

  priv = rule->priv;
*/
  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_rule_init (ShpRule * self)
{
  ShpRulePrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_RULE_TYPE,
                                            ShpRulePrivate);

  priv = self->priv;

  priv->conditions = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      g_object_unref);
}

static void
shp_rule_finalize (GObject * object)
{
  ShpRulePrivate *priv;
  ShpRule *self = SHP_RULE (object);

  priv = self->priv;

  if (priv->scene) {
    g_object_unref (priv->scene);
    priv->scene = NULL;
  }

  if (priv->conditions != NULL) {
    g_hash_table_unref (priv->conditions);
    priv->conditions = NULL;
  }
}

/**
 * shp_rule_new:
 *
 * Creates a new instance of #ShpRule. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpRule
 */
ShpRule*
shp_rule_new ()
{
  return g_object_new (SHP_RULE_TYPE, NULL);
}

void
shp_rule_add_condition (ShpRule * rule, ShpCondition * condition)
{
  ShpRulePrivate *priv;
  const gchar *path;

  g_return_if_fail (IS_SHP_RULE (rule));
  g_return_if_fail (IS_SHP_CONDITION (condition));

  priv = rule->priv;

  path = shp_condition_get_path (condition);
  g_hash_table_insert (priv->conditions, g_strdup (path), condition);
}

void
shp_rule_set_scene (ShpRule * rule, ShpScene * scene)
{
  ShpRulePrivate *priv;

  g_return_if_fail (IS_SHP_RULE (rule));
  g_return_if_fail (IS_SHP_SCENE (scene));

  priv = rule->priv;

  if (priv->scene)
    g_object_unref (priv->scene);
  priv->scene = scene;
}

static void
check_cond (gpointer key, gpointer value, gpointer user_data)
{
  gboolean *result = user_data;
  ShpCondition *condition = SHP_CONDITION (value);

  if (!shp_condition_is_satisfied (condition))
    *result = FALSE;
}

gboolean
shp_rule_process_event (ShpRule * rule, ShpMessage * event)
{
  ShpRulePrivate *priv;
  gboolean result = TRUE;
  const gchar *source_path;
  const gchar *destination_path;
  ShpCondition *condition;
  gboolean was_satisfied;

  g_return_val_if_fail (IS_SHP_RULE (rule), FALSE);
  g_return_val_if_fail (IS_SHP_MESSAGE (event), FALSE);

  priv = rule->priv;

  if (!priv->scene) {
    g_warning ("rule: no scene set");
    return FALSE;
  }

  source_path = shp_message_get_source_path (event);

  condition = SHP_CONDITION (g_hash_table_lookup (priv->conditions,
      source_path));
  if (!condition) {
    g_debug ("rule: path is of no interest %s, %s", source_path,
        destination_path);
    return FALSE;
  }

  g_debug ("rule: processing event from path: %s, %s", source_path,
      destination_path);

  was_satisfied = shp_condition_is_satisfied (condition);

  if (!shp_condition_process_event (condition, event)) {
    g_debug ("rule: condition not satisfied");
    return FALSE;
  }

  if (was_satisfied) {
    g_debug ("rule: condition was already satisfied, nothing new, ignoring");
    return FALSE;
  }

  g_hash_table_foreach (priv->conditions, check_cond, &result);

  if (result)
    shp_scene_activate (priv->scene);
  return (result);
}
