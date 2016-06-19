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
 * ShpCondition represents a state and functionality to check whether it is met.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-condition.h"
#include "shp-complextype.h"
#include "shp-complextype-compare.h"

G_DEFINE_TYPE (ShpCondition, shp_condition, G_TYPE_OBJECT);

#define DEFAULT_PATH "/empty path"

enum
{
  PROP_0,
  PROP_PATH,
  PROP_LAST
};

struct _ShpConditionPrivate {
  gchar *path;
  gboolean satisfied;
  GSList *options;
};

typedef struct {
  GType value_type;
  gchar *value_name;
  ShpConditionOperator op;
  GValue value;
} _Option;

static void shp_condition_finalize (GObject * object);
static void shp_condition_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_condition_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static void
shp_condition_class_init (ShpConditionClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpConditionPrivate));

  gobject_class->finalize = shp_condition_finalize;
  gobject_class->set_property = shp_condition_set_property;
  gobject_class->get_property = shp_condition_get_property;

  g_object_class_install_property (gobject_class, PROP_PATH,
      g_param_spec_string ("path", "Component path",
          "Path to the component", DEFAULT_PATH,
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
}

static void
shp_condition_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpConditionPrivate *priv;
  ShpCondition *condition = SHP_CONDITION (object);

  priv = condition->priv;

  switch (propid) {
    case PROP_PATH:
      g_value_set_string (value, priv->path);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_condition_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpConditionPrivate *priv;
  ShpCondition *condition = SHP_CONDITION (object);

  priv = condition->priv;

  switch (propid) {
    case PROP_PATH:
      g_free (priv->path);
      priv->path = g_strdup (g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_condition_init (ShpCondition * self)
{
  ShpConditionPrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_CONDITION_TYPE,
                                            ShpConditionPrivate);

  priv = self->priv;

  priv->path = g_strdup (DEFAULT_PATH);
}

static void
free_option (gpointer user_data)
{
  _Option * option = (_Option *)user_data;
  g_value_unset (&option->value);
  g_free (option->value_name);
  g_free (option);
}

static void
shp_condition_finalize (GObject * object)
{
  ShpConditionPrivate *priv;
  ShpCondition *self = SHP_CONDITION (object);

  priv = self->priv;

  if (priv->options != NULL) {
    g_slist_free_full (priv->options, free_option);
    priv->options = NULL;
  }

  g_free (priv->path);
  priv->path = NULL;
}

/**
 * shp_condition_new:
 *
 * Creates a new instance of #ShpCondition. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpCondition
 */
ShpCondition*
shp_condition_new (const gchar * path)
{
  return g_object_new (SHP_CONDITION_TYPE, "path", path, NULL);
}

static _Option*
option_new (GType type, const gchar * value_name, ShpConditionOperator op)
{
  _Option *option;

  option = g_new0 (_Option, 1);
  option->op = op;
  option->value_type = type;
  option->value_name = g_strdup (value_name);

  return option;
}

void
shp_condition_add_string_option (ShpCondition * condition,
    const gchar * value_name, const gchar * value, ShpConditionOperator op)
{
  ShpConditionPrivate *priv;
  _Option *option;

  g_return_if_fail (IS_SHP_CONDITION (condition));
  g_return_if_fail (op == SHP_CONDITION_OPERATOR_EQ);

  priv = condition->priv;

  option = option_new (G_TYPE_STRING, value_name, op);
  g_value_init (&option->value, G_TYPE_STRING);
  g_value_set_string (&option->value, value);

  priv->options = g_slist_append (priv->options, option);
}

void
shp_condition_add_complextype_option (ShpCondition * condition,
    const gchar * value_name, ShpComplextype * value, ShpConditionOperator op)
{
  ShpConditionPrivate *priv;
  _Option *option;

  g_return_if_fail (IS_SHP_CONDITION (condition));
  g_return_if_fail (op == SHP_CONDITION_OPERATOR_EQ);

  priv = condition->priv;

  option = option_new (SHP_COMPLEXTYPE_TYPE, value_name, op);
  g_value_init (&option->value, SHP_COMPLEXTYPE_TYPE);
  g_value_set_object (&option->value, value);

  priv->options = g_slist_append (priv->options, option);
}

void
shp_condition_add_double_option (ShpCondition * condition,
    const gchar * value_name, gdouble value, ShpConditionOperator op)
{
  ShpConditionPrivate *priv;
  _Option *option;

  g_return_if_fail (IS_SHP_CONDITION (condition));

  priv = condition->priv;

  option = option_new (G_TYPE_DOUBLE, value_name, op);
  g_value_init (&option->value, G_TYPE_DOUBLE);
  g_value_set_double (&option->value, value);

  priv->options = g_slist_append (priv->options, option);
}

void
shp_condition_add_integer_option (ShpCondition * condition,
    const gchar * value_name, gint value, ShpConditionOperator op)
{
  ShpConditionPrivate *priv;
  _Option *option;

  g_return_if_fail (IS_SHP_CONDITION (condition));

  priv = condition->priv;

  option = option_new (G_TYPE_INT, value_name, op);
  g_value_init (&option->value, G_TYPE_INT);
  g_value_set_int (&option->value, value);

  priv->options = g_slist_append (priv->options, option);
}

gboolean
check_event (ShpCondition * condition, const ShpMessage * event)
{
  ShpConditionPrivate *priv;
  GSList *options;
  gboolean result = TRUE;

  g_return_val_if_fail (IS_SHP_CONDITION (condition), FALSE);
  g_return_val_if_fail (IS_SHP_MESSAGE (event), FALSE);

  priv = condition->priv;

  options = priv->options;
  while (options != NULL) {
    _Option *option = (_Option *) (options->data);

    if (option->value_type == G_TYPE_STRING) {
      const gchar *event_value_str;
      const gchar *option_value_str;

      option_value_str = g_value_get_string (&option->value);
      event_value_str = shp_message_get_string ((ShpMessage *) event,
          option->value_name);
      if (g_strcmp0 (event_value_str, option_value_str))
        result = FALSE;
    } else if (option->value_type == G_TYPE_DOUBLE) {
      gdouble event_value_double;
      gdouble option_value_double;

      option_value_double = g_value_get_double (&option->value);
      if (!shp_message_get_double ((ShpMessage *) event,
          option->value_name, &event_value_double)) {
        result = FALSE;
      } else {

        g_debug ("condition: double, option value: %f, event value %f",
            option_value_double, event_value_double);

        switch (option->op) {
          case SHP_CONDITION_OPERATOR_EQ:
            if (option_value_double != event_value_double)
              result = FALSE;
            break;
          case SHP_CONDITION_OPERATOR_GT:
            if (option_value_double > event_value_double)
              result = FALSE;
            break;
          case SHP_CONDITION_OPERATOR_LT:
            if (option_value_double < event_value_double)
              result = FALSE;
            break;
          default:
            g_assert_not_reached ();
            result = FALSE;
            break;
        };

        g_debug ("condition: result: %d", result);
      }
    } else if (option->value_type == G_TYPE_INT) {
      gint event_value_int;
      gint option_value_int;

      option_value_int = g_value_get_int (&option->value);
      if (!shp_message_get_integer ((ShpMessage *) event,
          option->value_name, &event_value_int)) {
        result = FALSE;
      } else {
        switch (option->op) {
          case SHP_CONDITION_OPERATOR_EQ:
            if (option_value_int != event_value_int)
              result = FALSE;
            break;
          case SHP_CONDITION_OPERATOR_GT:
            if (option_value_int > event_value_int)
              result = FALSE;
            break;
          case SHP_CONDITION_OPERATOR_LT:
            if (option_value_int < event_value_int)
              result = FALSE;
            break;
          default:
            g_assert_not_reached ();
            result = FALSE;
            break;
        };
      }
    } else if (option->value_type == SHP_COMPLEXTYPE_TYPE) {
      const ShpComplextype *event_value_struct;
      const ShpComplextype *option_value_struct;
      ShpComplextypeCompareResult cmp_result;

      option_value_struct = g_value_get_object (&option->value);
      event_value_struct = shp_message_get_complextype ((ShpMessage *) event,
          option->value_name);
      cmp_result =
          shp_complextype_compare_compare (SHP_COMPLEXTYPE_COMPARE (option_value_struct),
          SHP_COMPLEXTYPE_COMPARE (event_value_struct));
      switch (option->op) {
        case SHP_CONDITION_OPERATOR_EQ:
          if (cmp_result != SHP_COMPLEXTYPE_COMPARE_EQ)
            result = FALSE;
          break;
        case SHP_CONDITION_OPERATOR_GT:
          if (cmp_result != SHP_COMPLEXTYPE_COMPARE_GT)
            result = FALSE;
          break;
        case SHP_CONDITION_OPERATOR_LT:
          if (cmp_result != SHP_COMPLEXTYPE_COMPARE_LT)
            result = FALSE;
          break;
        default:
          g_assert_not_reached ();
          result = FALSE;
          break;
      };
    } else {
      g_assert_not_reached ();
      result = FALSE;
      break;
    };

    if (!result)
      break;

    options = g_slist_next (options);
  }

  return result;
}

gboolean
shp_condition_process_event (ShpCondition * condition,
    const ShpMessage * event)
{
  ShpConditionPrivate *priv;

  g_return_val_if_fail (IS_SHP_CONDITION (condition), FALSE);

  priv = condition->priv;

  priv->satisfied = check_event (condition, event);

  return priv->satisfied;
}

gboolean
shp_condition_is_satisfied (ShpCondition * condition)
{
  ShpConditionPrivate *priv;

  g_return_val_if_fail (IS_SHP_CONDITION (condition), FALSE);

  priv = condition->priv;

  return priv->satisfied;
}

const gchar*
shp_condition_get_path (ShpCondition * condition)
{
  ShpConditionPrivate *priv;

  g_return_val_if_fail (IS_SHP_CONDITION (condition), FALSE);

  priv = condition->priv;

  return priv->path;
}
