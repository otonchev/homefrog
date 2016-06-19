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
 * This is a common interface for comparison of complex data types.
 */

#ifndef __SHP_COMPLEXTYPE_COMPARE_H__
#define __SHP_COMPLEXTYPE_COMPARE_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-complextype-compare.h"

typedef struct _ShpComplextypeCompare ShpComplextypeCompare; /* dummy object */
typedef struct _ShpComplextypeCompareInterface ShpComplextypeCompareInterface;

#define SHP_TYPE_COMPLEXTYPE_COMPARE         (shp_complextype_compare_get_type ())
#define SHP_COMPLEXTYPE_COMPARE(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), SHP_TYPE_COMPLEXTYPE_COMPARE, ShpComplextypeCompare))
#define IS_SHP_COMPLEXTYPE_COMPARE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SHP_TYPE_COMPLEXTYPE_COMPARE))
#define SHP_COMPLEXTYPE_COMPARE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), SHP_TYPE_COMPLEXTYPE_COMPARE, ShpComplextypeCompareInterface))

/*
 * ComplextypeStatus:
 * SHP_COMPLEXTYPE_COMPARE_GT   - greater than
 * SHP_COMPLEXTYPE_COMPARE_LT   - less then
 * SHP_COMPLEXTYPE_COMPARE_EQ   - equal
 * SHP_COMPLEXTYPE_COMPARE_FAIL - data cannot be compared
 */
typedef enum ShpComplextypeCompareResult
{
  SHP_COMPLEXTYPE_COMPARE_GT,
  SHP_COMPLEXTYPE_COMPARE_LT,
  SHP_COMPLEXTYPE_COMPARE_EQ,
  SHP_COMPLEXTYPE_COMPARE_FAIL
} ShpComplextypeCompareResult;

struct _ShpComplextypeCompareInterface {
  GTypeInterface iface;

  ShpComplextypeCompareResult (*compare) (ShpComplextypeCompare * data1,
      ShpComplextypeCompare * data2);
};

ShpComplextypeCompareResult
    shp_complextype_compare_compare (ShpComplextypeCompare * data1,
    ShpComplextypeCompare * data2);

GType shp_complextype_compare_get_type (void);

#endif /* __SHP_COMPLEXTYPE_COMPARE_H__ */
