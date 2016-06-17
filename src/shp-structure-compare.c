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
#include "shp-structure-compare.h"

G_DEFINE_INTERFACE (ShpStructureCompare, shp_structure_compare, G_TYPE_OBJECT);

ShpStructureCompareResult
shp_structure_compare_compare (ShpStructureCompare * data1,
    ShpStructureCompare * data2)
{
  g_return_val_if_fail (SHP_IS_STRUCTURE_COMPARE (data1),
      SHP_STRUCTURE_COMPARE_FAIL);
  g_return_val_if_fail (SHP_IS_STRUCTURE_COMPARE (data2),
      SHP_STRUCTURE_COMPARE_FAIL);

  return SHP_STRUCTURE_COMPARE_GET_INTERFACE (data1)->compare (data2);
}

static void
shp_structure_compare_default_init (ShpStructureCompareInterface *structure_compare)
{
}
