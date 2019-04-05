/* Copyright © 2001-2019, Canal TP and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Canal TP (www.canaltp.fr).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!

LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Stay tuned using
twitter @navitia
IRC #navitia on freenode
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#pragma once

#include <string>
#include <vector>

#include "type/type.h"
#include "type/pb_converter.h"

namespace navitia {
namespace equipment {

using StopAreasPerLines = std::map<type::Line*, std::vector<type::StopArea*>>;
using ForbiddenUris = std::vector<std::string>;

StopAreasPerLines get_stop_areas_per_lines(const type::Data& data,
                                           const std::string& filter,
                                           const ForbiddenUris& forbidden_uris = {});

void equipment_reports(PbCreator& pb_creator, int depth, int start_page,
                       int count, const std::string& filter,
                       const ForbiddenUris& forbidden_uris);

} // namespace equipment
} // namespace navitia


