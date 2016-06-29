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
 * ShpComplextypeTimer — complextype representing Time and Date. It allows
 * the framework to manage and compare times and dates.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-timer-complextype.h"
#include "../shp-complextype-compare.h"

static void shp_complextype_timer_interface_init (ShpComplextypeCompareInterface * iface);

G_DEFINE_TYPE_WITH_CODE (ShpComplextypeTimer, shp_complextype_timer,
    SHP_COMPLEXTYPE_TYPE, G_IMPLEMENT_INTERFACE (SHP_TYPE_COMPLEXTYPE_COMPARE,
        shp_complextype_timer_interface_init));

struct _ShpComplextypeTimerPrivate {
};

static void shp_complextype_timer_finalize (GObject * object);
static void shp_complextype_timer_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_complextype_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct {
  GValue value;
} _ShpValue;

static void
shp_complextype_timer_class_init (ShpComplextypeTimerClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
/*
  g_type_class_add_private (klass, sizeof (ShpComplextypeTimerPrivate));
*/
  gobject_class->finalize = shp_complextype_timer_finalize;
  gobject_class->set_property = shp_complextype_timer_set_property;
  gobject_class->get_property = shp_complextype_timer_get_property;
}

static void
shp_complextype_timer_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec)
{
  ShpComplextypeTimerPrivate *priv;
  ShpComplextypeTimer *complextype_timer = SHP_COMPLEXTYPE_TIMER (object);

  priv = complextype_timer->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_complextype_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpComplextypeTimerPrivate *priv;
  ShpComplextypeTimer *complextype_timer = SHP_COMPLEXTYPE_TIMER (object);

  priv = complextype_timer->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_complextype_timer_init (ShpComplextypeTimer * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_COMPLEXTYPE_TIMER_TYPE,
                                            ShpComplextypeTimerPrivate);
}

static void
shp_complextype_timer_finalize (GObject * object)
{
}

static ShpComplextypeCompareResult
compare (ShpComplextypeCompare * data1, ShpComplextypeCompare * data2)
{
  ShpComplextype *struct1;
  ShpComplextype *struct2;
  ShpComplextypeCompareResult comparison;

  g_return_val_if_fail (IS_SHP_COMPLEXTYPE_TIMER (data1),
      SHP_COMPLEXTYPE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_COMPLEXTYPE_TIMER (data2),
      SHP_COMPLEXTYPE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_COMPLEXTYPE_COMPARE (data1),
      SHP_COMPLEXTYPE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_COMPLEXTYPE_COMPARE (data2),
      SHP_COMPLEXTYPE_COMPARE_FAIL);

  struct1 = SHP_COMPLEXTYPE (data1);
  struct2 = SHP_COMPLEXTYPE (data2);

  if (shp_complextype_has_value (struct1, "week_day", G_TYPE_INT) &&
      shp_complextype_has_value (struct2, "week_day", G_TYPE_INT)) {
    gint weekday1;
    gint weekday2;

    shp_complextype_get_integer (struct1, "week_day", &weekday1);
    shp_complextype_get_integer (struct2, "week_day", &weekday2);

    if (weekday1 != weekday2) {
      comparison = SHP_COMPLEXTYPE_COMPARE_FAIL;
      g_debug ("timer-complextype: week days are different: %d, %d",
          weekday1, weekday2);
      goto out;
    }
    comparison = SHP_COMPLEXTYPE_COMPARE_EQ;
      g_debug ("timer-complextype: week days are same");
  } else
    comparison = SHP_COMPLEXTYPE_COMPARE_FAIL;
    /* let implementation below decide */

  if (shp_complextype_has_value (struct1, "hour", G_TYPE_INT) &&
      shp_complextype_has_value (struct1, "minutes", G_TYPE_INT) &&
      shp_complextype_has_value (struct2, "hour", G_TYPE_INT) &&
      shp_complextype_has_value (struct2, "minutes", G_TYPE_INT)) {
    gint hour1;
    gint minutes1;
    gint hour2;
    gint minutes2;

    gint time1;
    gint time2;

    shp_complextype_get_integer (struct1, "hour", &hour1);
    shp_complextype_get_integer (struct1, "minutes", &minutes1);
    shp_complextype_get_integer (struct2, "hour", &hour2);
    shp_complextype_get_integer (struct2, "minutes", &minutes2);

    time1 = hour1 * 60 + minutes1;
    time2 = hour2 * 60 + minutes2;

    if (time1 == time2) {
      comparison = SHP_COMPLEXTYPE_COMPARE_EQ;
      g_debug ("timer-complextype: times are same");
    } else if (time1 < time2) {
      comparison = SHP_COMPLEXTYPE_COMPARE_GT;
      g_debug ("timer-complextype: time1 < time2");
    } else {
      comparison = SHP_COMPLEXTYPE_COMPARE_LT;
      g_debug ("timer-complextype: time1 > time2");
    }
  }

out:
  return comparison;
}

static void
shp_complextype_timer_interface_init (ShpComplextypeCompareInterface * iface)
{
  iface->compare = compare;
}

/**
 * shp_complextype_timer_new:
 *
 * Creates a new instance of #ShpComplextypeTimer. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpComplextypeTimer
 */
ShpComplextypeTimer*
shp_complextype_timer_new ()
{
  return g_object_new (SHP_COMPLEXTYPE_TIMER_TYPE, NULL);
}
