/* ft2232h-spi-libftdi.cpp
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

#include "ft2232h-spi.h"

#include <sstream>
#include <ftdi.h>

#include "packet.h"
#include "util.h"

namespace ft2232h_spi {

namespace {

ftdi_context *getContext()
{
    static std::unique_ptr<ftdi_context, decltype(&ftdi_free)> global_ctxt
    { nullptr, &ftdi_free };
    if (!global_ctxt) {
        global_ctxt = { ftdi_new(), &ftdi_free };
    }

    return global_ctxt.get();
}

constexpr uint16_t spi_clkdiv = 0x05db; /* 1 MHz */
constexpr uint8_t bad_opcode_reply = 0xfa;
}

struct spi::impl
{
    impl(pins cs_pin, busses bus) :
        ctxt(getContext()),
        cs_pin(cs_pin)
    {
    }

    packet csPacket(bool cs_high);
    void init(const endpoint& ep);
    void sendRaw(const packet& p);
    void sync();
    void expectResponse(const packet& p);
    void expectEmptyResponse();
    void onError(const std::string& when);

    struct ftdi_context *ctxt;
    pins cs_pin;
    busses bus;
};

spi::~spi()
{
}

spi::spi(
    pins cs, const endpoint& ep, busses bus)
    noexcept(false) :
    d(new impl { cs, bus })
{
    d->init(ep);
}

spi::spi(spi&& other) noexcept(true)
    : d(std::move(other.d))
{
}

spi& spi::operator=(spi&& other) noexcept(true)
{
    std::swap(d, other.d);
    return *this;
}

void spi::transmit(const packet& payload)
{
    if (payload.size() < 1) {
        throw error(WHEN("can't send a packet with <1 bytes."));
    }

    packet p;
    p.append(d->csPacket(false));
    p.append({ opcodes::write, uint16_t(payload.size() - 1) });
    p.append(payload);
    p.append(d->csPacket(true));
    d->sendRaw(p);
    d->expectEmptyResponse();
}

void spi::impl::sendRaw(const packet& p)
{
    if (ftdi_write_data(ctxt, const_cast<uint8_t*>(p.data()), p.size()) != p.size()) {
        onError(WHEN("ftdi_write_data"));
    }
}

packet spi::impl::csPacket(bool cs_high)
{
    uint8_t pin_state =
        spi::pins::sck |
        (cs_high ? cs_pin : 0);
    return {
        opcodes::set_low_bits,
        pin_state,
        uint8_t(pins::sck | pins::sdata | cs_pin)
    };
}

void spi::impl::init(const endpoint& ep)
{
    if (ftdi_set_interface(ctxt, (ftdi_interface)bus)) {
        onError(WHEN("ftdi_set_interface"));
    }

    auto descr = ep.description.empty() ? nullptr : ep.description.c_str();
    auto serial = ep.serial.empty() ? nullptr : ep.serial.c_str();

    if (ftdi_usb_open_desc(ctxt, ep.vid, ep.pid, descr, serial)) {
        onError(WHEN("ftdi_usb_open_desc"));
    }

    if (ftdi_set_bitmode(ctxt, 0, BITMODE_RESET)) {
        onError(WHEN("ftdi_set_bitmode"));
    }

    if (ftdi_set_bitmode(ctxt, 0, BITMODE_MPSSE)) {
        onError(WHEN("ftdi_set_bitmode"));
    }

    sync();

    /* Set up basic parameters. */
    sendRaw({
        opcodes::clkdiv_5_enable,
        opcodes::adaptive_clk_disable,
        opcodes::three_phase_disable
    });

    /* Set the baudrate. */
    sendRaw({
        opcodes::set_clkdiv, spi_clkdiv
    });

    /* Configure pin states. */
    sendRaw(csPacket(true));
    sendRaw({
        opcodes::set_high_bits,
        uint8_t(0),
        uint8_t(0)
    });
    expectEmptyResponse();
}

void spi::impl::sync()
{
    uint8_t buffer[2];
    sendRaw(opcodes::loopback_enable);
    expectEmptyResponse();

    sendRaw(opcodes::bogus);

    expectResponse({
        bad_opcode_reply,
        opcodes::bogus,
    });
    expectEmptyResponse();

    sendRaw(opcodes::loopback_disable);
}

void spi::impl::expectResponse(const packet& p)
{
    uint8_t buffer[p.size()];
    int rc = ftdi_read_data(ctxt, buffer, p.size());
    if (rc != p.size()) {
        std::ostringstream what;
        what << WHEN()
             << "expected " << p.size() << " byte reply but got "
             << rc << " bytes.";
        onError(what.str());
    }

    if (memcmp(p.data(), buffer, p.size()) != 0) {
        onError(WHEN("did not receive expected reply"));
    }
}

void spi::impl::expectEmptyResponse()
{
    expectResponse(packet {});
}

void spi::impl::onError(const std::string& when)
{
    throw error(when + ": " + ftdi_get_error_string(ctxt));
}

std::vector<endpoint> getAvailableEndpoints(int vid, int pid)
{
    auto ctxt = getContext();
    ftdi_device_list *devlist = nullptr;
    auto rc = ftdi_usb_find_all(ctxt, &devlist, vid, pid);

    if (rc < 0) {
        throw error {
            std::string { WHEN("getAvailableEndpoints: ") } + ftdi_get_error_string(ctxt)
        };
    }

    scope_guard guard { [devlist]() { ftdi_list_free2(devlist); } };

    std::vector<endpoint> result;
    result.reserve(rc);

    std::array<char, 32> mfg_buff;
    std::array<char, 32> descr_buff;
    std::array<char, 32> serial_buff;

    auto curr_dev = devlist;
    for (int i = 0; i < rc; ++i, curr_dev = curr_dev->next) {
        if (ftdi_usb_get_strings(
            ctxt, curr_dev->dev,
            mfg_buff.data(), mfg_buff.size(),
            descr_buff.data(), descr_buff.size(),
            serial_buff.data(), serial_buff.size()))
        {
            throw error {
                std::string { WHEN("ftdi_usb_get_strings2: ") } + ftdi_get_error_string(ctxt)
            };
        }

        result.emplace_back(
            vid, pid,
            std::string { mfg_buff.data() },
            std::string { descr_buff.data() },
            std::string { serial_buff.data() }
        );
    }
    return result;
}

} /* namespace ft2232h_spi */

