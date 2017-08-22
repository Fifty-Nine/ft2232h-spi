/* packet-detail.h
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
#ifndef FT2232H_SPI_PACKET_DETAIL_H
#define FT2232H_SPI_PACKET_DETAIL_H

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace ft2232h_spi {
namespace detail {

template<size_t Size>
size_t pack_impl(uint8_t *buffer)
{
    return 0;
}

template<size_t Size, class T, class... Args>
size_t pack_impl(uint8_t *buffer, T&& arg, Args&&... args)
{
    static_assert(
        std::is_pod<std::remove_reference<T>>(),
        "Cannot pack a non-POD into a buffer."
    );
    static_assert(sizeof(T) < Size, "Buffer overflow when expanding a pack.");

    memcpy(buffer, &arg, sizeof(T));
    return sizeof(T) + pack_impl<Size - sizeof(T)>(
        buffer + sizeof(T), std::forward<Args>(args)...
    );
}

template<size_t Size, class... Args>
size_t pack(uint8_t (&buffer)[Size], Args&&... args)
{
    return pack_impl<Size>(buffer, std::forward<Args>(args)...);
}

} /* namespace detail */
} /* namespace ft2232h_spi */

#endif /* FT2232H_SPI_PACKET_DETAIL_H */
