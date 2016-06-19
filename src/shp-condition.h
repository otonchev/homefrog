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

#ifndef __SHP_CONDITION_H__
#define __SHP_CONDITION_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-message.h"

G_BEGIN_DECLS

#define SHP_CONDITION_TYPE (shp_condition_get_type ())
#define SHP_CONDITION(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_CONDITION_TYPE,ShpCondition))
#define SHP_CONDITION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_CONDITION_TYPE, ShpConditionClass))
#define IS_SHP_CONDITION(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_CONDITION_TYPE))
#define IS_SHP_CONDITION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_CONDITION_TYPE))
#define SHP_CONDITION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_CONDITION_TYPE, ShpConditionClass))

typedef struct _ShpCondition ShpCondition;
typedef struct _ShpConditionPrivate ShpConditionPrivate;
typedef struct _ShpConditionClass ShpConditionClass;

typedef enum ShpConditionOperator {
    SHP_CONDITION_OPERATOR_EQ,
    SHP_CONDITION_OPERATOR_LT,
    SHP_CONDITION_OPERATOR_GT
} ShpConditionOperator;

struct _ShpCondition {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpConditionPrivate *priv;
};

struct _ShpConditionClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpCondition * shp_condition_new (const gchar * path);
const gchar *shp_condition_get_path (ShpCondition * condition);
void shp_condition_add_string_option (ShpCondition * condition,
    const gchar * value_name, const gchar * value, ShpConditionOperator op);
void shp_condition_add_double_option (ShpCondition * condition,
    const gchar * value_name, gdouble value, ShpConditionOperator op);
void shp_condition_add_integer_option (ShpCondition * condition,
    const gchar * value_name, gint value, ShpConditionOperator op);
void shp_condition_add_complextype_option (ShpCondition * condition,
    const gchar * value_name, ShpComplextype * value, ShpConditionOperator op);
gboolean shp_condition_is_satisfied (ShpCondition * condition);
gboolean shp_condition_process_event (ShpCondition * condition,
    const ShpMessage * event);

GType shp_condition_get_type (void);

G_END_DECLS

#endif /* __SHP_CONDITION_H__ */
