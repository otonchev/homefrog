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
 * ShpStructureTimer is the basic unit of passing data from plugins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-timer-structure.h"
#include "shp-structure-compare.h"

static void shp_structure_timer_interface_init (ShpStructureCompareInterface * iface);

G_DEFINE_TYPE_WITH_CODE (ShpStructureTimer, shp_structure_timer,
    SHP_STRUCTURE_TYPE, G_IMPLEMENT_INTERFACE (SHP_TYPE_STRUCTURE_COMPARE,
        shp_structure_timer_interface_init));

struct _ShpStructureTimerPrivate {
};

static void shp_structure_timer_finalize (GObject * object);
static void shp_structure_timer_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_structure_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct {
  GValue value;
} _ShpValue;

static void
shp_structure_timer_class_init (ShpStructureTimerClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpStructureTimerPrivate));

  gobject_class->finalize = shp_structure_timer_finalize;
  gobject_class->set_property = shp_structure_timer_set_property;
  gobject_class->get_property = shp_structure_timer_get_property;
}

static void
shp_structure_timer_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec)
{
  ShpStructureTimerPrivate *priv;
  ShpStructureTimer *structure_timer = SHP_STRUCTURE_TIMER (object);

  priv = structure_timer->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_structure_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpStructureTimerPrivate *priv;
  ShpStructureTimer *structure_timer = SHP_STRUCTURE_TIMER (object);

  priv = structure_timer->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_structure_timer_init (ShpStructureTimer * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_STRUCTURE_TIMER_TYPE,
                                            ShpStructureTimerPrivate);
}

static void
shp_structure_timer_finalize (GObject * object)
{
}

static ShpStructureCompareResult
compare (ShpStructureCompare * data1, ShpStructureCompare * data2)
{
  ShpStructure *struct1;
  ShpStructure *struct2;
  ShpStructureCompareResult comparison = SHP_STRUCTURE_COMPARE_FAIL;

  g_return_val_if_fail (IS_SHP_STRUCTURE_TIMER (data1),
      SHP_STRUCTURE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_STRUCTURE_TIMER (data2),
      SHP_STRUCTURE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_STRUCTURE_COMPARE (data1),
      SHP_STRUCTURE_COMPARE_FAIL);
  g_return_val_if_fail (IS_SHP_STRUCTURE_COMPARE (data2),
      SHP_STRUCTURE_COMPARE_FAIL);

  struct1 = SHP_STRUCTURE (data1);
  struct2 = SHP_STRUCTURE (data2);

  if (shp_structure_has_value (struct1, "hour", G_TYPE_INT) &&
      shp_structure_has_value (struct1, "minutes", G_TYPE_INT) &&
      shp_structure_has_value (struct2, "hour", G_TYPE_INT) &&
      shp_structure_has_value (struct2, "minutes", G_TYPE_INT)) {
    gint hour1;
    gint minutes1;
    gint hour2;
    gint minutes2;

    shp_structure_get_integer (struct1, "hour", &hour1);
    shp_structure_get_integer (struct1, "minutes", &minutes1);
    shp_structure_get_integer (struct2, "hour", &hour2);
    shp_structure_get_integer (struct2, "minutes", &minutes2);

    if (hour1 == hour2 && minutes1 == minutes2)
      comparison = SHP_STRUCTURE_COMPARE_EQ;
    else if (hour1 == hour2 && minutes1 > minutes2)
      comparison = SHP_STRUCTURE_COMPARE_GT;
    else if (hour1 > hour2)
      comparison = SHP_STRUCTURE_COMPARE_GT;
    else
      comparison = SHP_STRUCTURE_COMPARE_LT;
  }

  return comparison;
}

static void
shp_structure_timer_interface_init (ShpStructureCompareInterface * iface)
{
  iface->compare = compare;
}

/**
 * shp_structure_timer_new:
 *
 * Creates a new instance of #ShpStructureTimer. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpStructureTimer
 */
ShpStructureTimer*
shp_structure_timer_new ()
{
  return g_object_new (SHP_STRUCTURE_TIMER_TYPE, NULL);
}
