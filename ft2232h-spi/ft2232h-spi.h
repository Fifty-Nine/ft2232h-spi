/* ft2232h-spi.h
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
#ifndef FT2232H_SPI_H
#define FT2232H_SPI_H

#include <memory>
#include <vector>

#include "ft2232h-spi/exceptions.h"
#include "ft2232h-spi/util.h"

namespace ft2232h_spi
{

struct packet;
struct endpoint
{
    int vid;
    int pid;
    std::string manufacturer;
    std::string description;
    std::string serial;

    endpoint() = default;
    endpoint(const endpoint&) = default;
    endpoint(endpoint&&) = default;
    endpoint(
        int vid, int pid,
        const std::string& manufacturer,
        const std::string& description,
        const std::string& serial) :
            vid(vid),
            pid(pid),
            manufacturer(manufacturer),
            description(description),
            serial(serial)
    { }

    bool match(const endpoint& other) const {
        return equal_or_defaulted(vid, other.vid) &&
            equal_or_defaulted(pid, other.pid) &&
            equal_or_defaulted(manufacturer, other.manufacturer) &&
            equal_or_defaulted(description, other.description) &&
            equal_or_defaulted(serial, other.serial);
    }
};

std::vector<endpoint> getAvailableEndpoints(int vid, int pid);

struct spi
{
public:
    enum pins : uint8_t {
        dbus0 = 0x01,
        dbus1 = 0x02,
        dbus2 = 0x04,
        dbus3 = 0x08,
        dbus4 = 0x10,
        dbus5 = 0x20,
        dbus6 = 0x40,
        dbus7 = 0x80,

        sck = dbus0,
        sdata = dbus1
    };
    enum busses : uint8_t {
        bus_any = 0,
        bus_a = 1,
        bus_b = 2,
        bus_c = 3,
        bus_d = 4
    };

    virtual ~spi() noexcept(true);

    spi(pins cs_pin, const endpoint& ep, busses bus = bus_a);
    spi(const spi&) = delete;
    spi(spi&&) noexcept(true);

    spi& operator=(const spi&) = delete;
    spi& operator=(spi&&) noexcept(true);

    void transmit(const packet& p);

private:
    enum class opcodes : uint8_t {
        write                = 0x10,
        set_low_bits         = 0x80,
        set_high_bits        = 0x82,
        loopback_enable      = 0x84,
        loopback_disable     = 0x85,
        set_clkdiv           = 0x86,
        clkdiv_5_disable     = 0x8a,
        clkdiv_5_enable      = 0x8b,
        three_phase_enable   = 0x8c,
        three_phase_disable  = 0x8d,
        adaptive_clk_enable  = 0x96,
        adaptive_clk_disable = 0x97,
        bogus                = 0xab
    };


    struct impl;
    std::unique_ptr<impl> d;
};

} /* namespace ft2232h_spi */

#endif /* FT2232H_SPI_H */
