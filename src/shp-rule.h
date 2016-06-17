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

#ifndef __SHP_RULE_H__
#define __SHP_RULE_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-condition.h"
#include "shp-scene.h"
#include "shp-message.h"

G_BEGIN_DECLS

#define SHP_RULE_TYPE (shp_rule_get_type ())
#define SHP_RULE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_RULE_TYPE,ShpRule))
#define SHP_RULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_RULE_TYPE, ShpRuleClass))
#define IS_SHP_RULE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_RULE_TYPE))
#define IS_SHP_RULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_RULE_TYPE))
#define SHP_RULE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_RULE_TYPE, ShpRuleClass))

typedef struct _ShpRule ShpRule;
typedef struct _ShpRulePrivate ShpRulePrivate;
typedef struct _ShpRuleClass ShpRuleClass;

struct _ShpRule {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpRulePrivate *priv;
};

struct _ShpRuleClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpRule * shp_rule_new ();
void shp_rule_add_condition (ShpRule * rule, ShpCondition * condition,
    const gchar * path);
void shp_rule_set_scene (ShpRule * rule, ShpScene * scene);
gboolean shp_rule_process_event (ShpRule * rule, ShpMessage * event);

GType shp_rule_get_type (void);

G_END_DECLS

#endif /* __SHP_RULE_H__ */
