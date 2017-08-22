/* packet.h
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
#ifndef FT2232H_SPI_PACKET_H
#define FT2232H_SPI_PACKET_H

#include "packet-detail.h"

namespace ft2232h_spi
{

struct packet
{
    template<class... Args>
    packet(Args&&... args) :
    size { (uint8_t)detail::pack(buffer, std::forward<Args>(args)...) }
    { }

    void append(const packet& p);

    uint8_t buffer[16];
    uint8_t size = 0;
};


} /* namespace ft2232h_spi */

#endif /* FT2232H_SPI_PACKET_H */
