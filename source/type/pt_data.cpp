#include "pt_data.h"

namespace navitia{namespace type {


PT_Data& PT_Data::operator=(PT_Data&& other){
#define COPY_FROM_OTHER(type_name, collection_name) collection_name = other.collection_name; collection_name##_map = other.collection_name##_map;
    ITERATE_NAVITIA_PT_TYPES(COPY_FROM_OTHER)

    journey_pattern_point_connections = other.journey_pattern_point_connections;
    stop_times = other.stop_times;

    // First letter
    stop_area_autocomplete = other.stop_area_autocomplete;
    city_autocomplete = other.city_autocomplete;
    stop_point_autocomplete = other.stop_point_autocomplete;    

    // Proximity list
    stop_area_proximity_list = other.stop_area_proximity_list;
    stop_point_proximity_list = other.stop_point_proximity_list;
    city_proximity_list = other.city_proximity_list;

    return *this;
}


std::vector<idx_t> PT_Data::get_target_by_source(Type_e source, Type_e target, std::vector<idx_t> source_idx) const {
    std::vector<idx_t> result;
    result.reserve(source_idx.size());
    for(idx_t idx : source_idx) {
        std::vector<idx_t> tmp;
        tmp = get_target_by_one_source(source, target, idx);
        result.insert(result.end(), tmp.begin(), tmp.end());

    }
    return result;
}

std::vector<idx_t> PT_Data::get_target_by_one_source(Type_e source, Type_e target, idx_t source_idx) const {
    std::vector<idx_t> result;
    if(source_idx == invalid_idx)
        return result;
    if(source == target){
        result.push_back(source_idx);
        return result;
    }
    switch(source) {
#define GET_INDEXES(type_name, collection_name) case Type_e::type_name: result = collection_name[source_idx].get(target, *this);break;
    ITERATE_NAVITIA_PT_TYPES(GET_INDEXES)
        default: break;
    }
    return result;
}

std::vector<idx_t> PT_Data::get_all_index(Type_e type) const {
    size_t num_elements = 0;
    switch(type){
#define GET_NUM_ELEMENTS(type_name, collection_name) case Type_e::type_name: num_elements = collection_name.size();break;
    ITERATE_NAVITIA_PT_TYPES(GET_NUM_ELEMENTS)
    default:  break;
    }
    std::vector<idx_t> indexes(num_elements);
    for(size_t i=0; i < num_elements; i++)
        indexes[i] = i;
    return indexes;
}

#define GET_DATA(type_name, collection_name)\
    template<> std::vector<type_name> & PT_Data::get_data<type_name>() {return collection_name;}\
    template<> std::vector<type_name> const & PT_Data::get_data<type_name>() const {return collection_name;}
ITERATE_NAVITIA_PT_TYPES(GET_DATA)


void PT_Data::build_autocomplete(const std::map<std::string, std::string> & map_alias){
    for(const StopArea & sa : this->stop_areas){
        if(sa.city_idx < this->cities.size())
            this->stop_area_autocomplete.add_string(sa.name + " " + cities[sa.city_idx].name, sa.idx,type::Type_e::StopArea, map_alias);
        else
            this->stop_area_autocomplete.add_string(sa.name, sa.idx,type::Type_e::StopArea, map_alias);
    }

    this->stop_area_autocomplete.build();

    for(const StopPoint & sp : this->stop_points){
        if(sp.city_idx < this->cities.size())
            this->stop_point_autocomplete.add_string(sp.name + " " + cities[sp.city_idx].name, sp.idx, type::Type_e::StopPoint, map_alias);
        else
            this->stop_point_autocomplete.add_string(sp.name, sp.idx, type::Type_e::StopPoint, map_alias);
    }

    this->stop_point_autocomplete.build();


    for(const City & city : cities){
        this->city_autocomplete.add_string(city.name, city.idx, type::Type_e::City, map_alias);
    }
    this->city_autocomplete.build();
}

void PT_Data::build_proximity_list() {
    for(const City & city : this->cities){
        this->city_proximity_list.add(city.coord, city.idx);
    }
    this->city_proximity_list.build();

    for(const StopArea &stop_area : this->stop_areas){
        this->stop_area_proximity_list.add(stop_area.coord, stop_area.idx);
    }
    this->stop_area_proximity_list.build();

    for(const StopPoint & stop_point : this->stop_points){
        this->stop_point_proximity_list.add(stop_point.coord, stop_point.idx);
    }
    this->stop_point_proximity_list.build();
}

void PT_Data::build_uri() {
#define NORMALIZE_EXT_CODE(type_name, collection_name) normalize_extcode<type_name>(collection_name##_map);
    ITERATE_NAVITIA_PT_TYPES(NORMALIZE_EXT_CODE)
}


void PT_Data::build_connections() {
    stop_point_connections.resize(stop_points.size());
    for(Connection &conn : this->connections){
        const StopPoint & dep = this->stop_points[conn.departure_stop_point_idx];
        const StopPoint & arr = this->stop_points[conn.destination_stop_point_idx];
        if(dep.stop_area_idx == arr.stop_area_idx)
            conn.connection_type = eStopAreaConnection;
        else
            conn.connection_type = eWalkingConnection;
        stop_point_connections[dep.idx].push_back(conn);
    }
}

}}
