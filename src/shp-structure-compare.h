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
 * This is a common interface for comparisson of complex data types.
 */

#ifndef __SHP_STRUCTURE_COMPARE_H__
#define __SHP_STRUCTURE_COMPARE_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-structure-compare.h"

typedef struct _ShpStructureCompare ShpStructureCompare; /* dummy object */
typedef struct _ShpStructureCompareInterface ShpStructureCompareInterface;

#define SHP_TYPE_STRUCTURE_COMPARE         (shp_structure_compare_get_type ())
#define SHP_STRUCTURE_COMPARE(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), SHP_TYPE_STRUCTURE_COMPARE, ShpStructureCompare))
#define SHP_IS_STRUCTURE_COMPARE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SHP_TYPE_STRUCTURE_COMPARE))
#define SHP_STRUCTURE_COMPARE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), SHP_TYPE_STRUCTURE_COMPARE, ShpStructureCompareInterface))

/*
 * StructureStatus:
 * SHP_STRUCTURE_COMPARE_GT   - greater than
 * SHP_STRUCTURE_COMPARE_LT   - less then
 * SHP_STRUCTURE_COMPARE_EQ   - equal
 * SHP_STRUCTURE_COMPARE_FAIL - data cannot be compared
 */
typedef enum ShpStructureCompareResult
{
  SHP_STRUCTURE_COMPARE_GT,
  SHP_STRUCTURE_COMPARE_LT,
  SHP_STRUCTURE_COMPARE_EQ,
  SHP_STRUCTURE_COMPARE_FAIL
} ShpStructureCompareResult;

struct _ShpStructureCompareInterface {
  GTypeInterface iface;

  ShpStructureCompareResult (*compare) (ShpStructureCompare * data2);
};

ShpStructureCompareResult
    shp_structure_compare_compare (ShpStructureCompare * data1,
    ShpStructureCompare * data2);

GType shp_structure_compare_get_type (void);

#endif /* __SHP_STRUCTURE_COMPARE_H__ */
