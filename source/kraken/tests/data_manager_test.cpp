/* Copyright © 2001-2014, Canal TP and/or its affiliates. All rights reserved.

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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE data_manager_test

#include "kraken/data_manager.h"

#include <boost/test/unit_test.hpp>
#include <atomic>

namespace test {

//mock of Data class
class Data {
    public:

        void load_nav(const std::string& filename){}
        void load_disruptions(const std::string& database,
                              const std::vector<std::string>& contributors = {}){}
        void build_raptor(size_t cache_size = 10){}
        mutable std::atomic<bool> loading;
        mutable std::atomic<bool> is_connected_to_rabbitmq;
        static bool load_status;
        static bool last_load;
        static bool destructor_called;
        size_t data_identifier;

        Data(size_t data_identifier=0):
            data_identifier(data_identifier)
        {is_connected_to_rabbitmq = false;}

        ~Data(){Data::destructor_called = true;}
};
bool Data::load_status = true;
bool Data::last_load = true;
bool Data::destructor_called = false;

} // namespace test

struct fixture{
    fixture(){
        test::Data::load_status = true;
        test::Data::destructor_called = false;
    }
};

BOOST_FIXTURE_TEST_CASE(get_data, fixture) {
    DataManager<test::Data> data_manager;
    auto data = data_manager.get_data();
    BOOST_REQUIRE(data);
    BOOST_CHECK_EQUAL(test::Data::destructor_called, false);
}

BOOST_FIXTURE_TEST_CASE(load, fixture) {
    DataManager<test::Data> data_manager;
    BOOST_CHECK(data_manager.get_data());
    auto first_data = data_manager.get_data();

    // Same pointer
    BOOST_CHECK_EQUAL(first_data, data_manager.get_data());

    // True because fake data don't failed
    BOOST_CHECK(data_manager.load("fake path"));

    // Load OK, so the internal shared pointer change.
    // Data has changed.
    BOOST_CHECK_NE(first_data, data_manager.get_data());
}

BOOST_FIXTURE_TEST_CASE(destructor_called, fixture) {
    DataManager<test::Data> data_manager;
    {
        auto first_data = data_manager.get_data();
        // True because fake data don't failed
        BOOST_CHECK(data_manager.load("fake path"));
        // Load OK, so the internal shared pointer change.
        // Data has changed.
        BOOST_CHECK_NE(first_data, data_manager.get_data());
    }
    // test::Data destructor is called because when load function is called,
    // new shared pointer is not used.
    BOOST_CHECK_EQUAL(test::Data::destructor_called, true);
    BOOST_CHECK(data_manager.get_data());
}

BOOST_FIXTURE_TEST_CASE(destructor_not_called, fixture) {
    DataManager<test::Data> data_manager;
    {
        auto first_data = data_manager.get_data();
        BOOST_CHECK_EQUAL(first_data, data_manager.get_data());
    }
    // type::Data destructor is not called.
    BOOST_CHECK_EQUAL(test::Data::destructor_called, false);
    BOOST_CHECK(data_manager.get_data());
}
