/* packet-tests.cpp
 * Copyright (C) 2017 Tim Prince
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/test/unit_test.hpp>
#include "ft2232h-spi/packet.h"

#include <cstdint>

using namespace ft2232h_spi;

BOOST_AUTO_TEST_SUITE(packet_tests)

BOOST_AUTO_TEST_CASE(construct_small)
{
    int32_t exp[] = { 0x11223344, 0x44332211 };
    packet p { 0x11223344, 0x44332211 };
    BOOST_REQUIRE_EQUAL(p.size, 8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p.buffer, (int32_t*)(p.buffer + p.size),
        exp, exp + 2
    );
}

BOOST_AUTO_TEST_CASE(construct_medium)
{
    int32_t exp[] = { 0, 1, 2, 3 };

    static_assert(sizeof(exp) == 16);
    packet p { 0, 1, 2, 3 };
    BOOST_REQUIRE_EQUAL(p.size, 16);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p.buffer, (int32_t*)(p.buffer + p.size),
        exp, exp + 4
    );
}

BOOST_AUTO_TEST_CASE(construct_ref)
{
    int8_t exp[] = {
        0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8,
        0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0
    };
    packet p {
        exp[ 0], exp[ 1], exp[ 2], exp[ 3],
        exp[ 4], exp[ 5], exp[ 6], exp[ 7],
        exp[ 8], exp[ 9], exp[10], exp[11],
        exp[12], exp[13], exp[14], exp[15]
    };
    BOOST_REQUIRE_EQUAL(p.size, 16);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        p.buffer, p.buffer + p.size,
        exp, exp + 16
    );
}

BOOST_AUTO_TEST_CASE(construct_homogenous)
{
    packet p { (uint8_t)0, (uint16_t)1, (uint32_t)2 };
    uint8_t exp[] = "\x00\x01\x00\x02\x00\x00\x00";

    BOOST_REQUIRE_EQUAL(p.size, 7);
    /*
     * This will fail on big endian platforms, but we don't
     * yet attempt to support them.
     */
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        p.buffer, p.buffer + 7,
        exp, exp + 7
    );
}

BOOST_AUTO_TEST_CASE(construct_homogenous2)
{
    packet p { (uint64_t)0, (uint32_t)1, (uint16_t)2, (uint8_t)3 };
    uint8_t exp[] =
        "\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x03";

    BOOST_REQUIRE_EQUAL(p.size, 15);
    /*
     * This will fail on big endian platforms, but we don't
     * yet attempt to support them.
     */
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        p.buffer, p.buffer + 15,
        exp, exp + 15
    );
}


BOOST_AUTO_TEST_CASE(append_small)
{
    packet p1 { 1 };
    packet p2 { 2 };
    int32_t exp[] { 1, 2 };

    BOOST_REQUIRE_EQUAL(p1.size, 4);
    BOOST_REQUIRE_EQUAL(p2.size, 4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p1.buffer, (int32_t*)(p1.buffer + p1.size),
        exp, exp + 1
    );
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p2.buffer, (int32_t*)(p2.buffer + p2.size),
        exp + 1, exp + 2
    );

    p1.append(p2);
    BOOST_REQUIRE_EQUAL(p1.size, 8);
    BOOST_REQUIRE_EQUAL(p2.size, 4);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p1.buffer, (int32_t*)(p1.buffer + p1.size),
        exp, exp + 2
    );
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p2.buffer, (int32_t*)(p2.buffer + p2.size),
        exp + 1, exp + 2
    );
}

BOOST_AUTO_TEST_CASE(append_medium)
{
    packet p1 { 0, 1 };
    packet p2 { 2, 3 };
    int32_t exp[] = { 0, 1, 2, 3 };

    BOOST_REQUIRE_EQUAL(p1.size, 8);
    BOOST_REQUIRE_EQUAL(p2.size, 8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p1.buffer, (int32_t*)(p1.buffer + p1.size),
        exp, exp + 2
    );
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p2.buffer, (int32_t*)(p2.buffer + p2.size),
        exp + 2, exp + 4
    );

    p1.append(p2);

    BOOST_REQUIRE_EQUAL(p1.size, 16);
    BOOST_REQUIRE_EQUAL(p2.size, 8);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p1.buffer, (int32_t*)(p1.buffer + p1.size),
        exp, exp + 4
    );
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        (int32_t*)p2.buffer, (int32_t*)(p2.buffer + p2.size),
        exp + 2, exp + 4
    );
}

BOOST_AUTO_TEST_SUITE_END()
