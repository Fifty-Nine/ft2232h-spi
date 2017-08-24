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

#include <cstddef>
#include <cstdint>

namespace ft2232h_spi
{

class packet
{
public:
    template<class... Args>
    packet(Args&&... args);

    void append(const packet& p);

    const uint8_t *data() const { return data_; }
    size_t size() const { return size_; }

private:
    uint8_t data_[16];
    uint8_t size_ = 0;
};

} /* namespace ft2232h_spi */

#define FT2232H_SPI_INCLUDE_PACKET_DETAIL_H
#include "packet-detail.h"
#undef FT2232H_SPI_INCLUDE_PACKET_DETAIL_H

#endif /* FT2232H_SPI_PACKET_H */
