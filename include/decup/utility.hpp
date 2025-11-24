// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// Utility
///
/// \file   decup/utility.hpp
/// \author Vincent Hamp
/// \date   24/10/2024

#pragma once

#include <algorithm>
#include <cstdint>
#include <ztl/inplace_vector.hpp>
#include "packet.hpp"
#include "timeouts.hpp"

namespace decup {

/// Get data size for specific decoder ID
///
/// Some decoder types only support 32B payloads.
///
/// \param  id  Decoder ID
/// \return Data size for decoder ID
constexpr size_t decoder_id2data_size(uint8_t decoder_id) {
  return decoder_id == 200u || (decoder_id >= 202u && decoder_id <= 205u)
           ? 32uz
           : 64uz;
}

/// Get bootloader size for specific decoder ID
///
/// \param  id  Decoder ID
/// \return Bootloader size for decoder ID
constexpr size_t decoder_id2bootloader_size(uint8_t decoder_id) {
  return decoder_id == 200u || (decoder_id >= 202u && decoder_id <= 205u)
           ? 256uz
           : 2048uz;
}

/// Get stop bit count for specifig decoder ID
///
/// Usually, 2 stop bits are used, but some decoder types respond directly after
/// the first stop bit.
///
/// \param decoder_id Decoder ID
/// \return Stop bit count for decoder ID
constexpr size_t decoder_id2stop_bit_count(uint8_t decoder_id) {
  return decoder_id == 200u || (decoder_id >= 202u && decoder_id <= 205u) ? 1uz
                                                                          : 2uz;
}

/// Get pulse timeout in [µs] for specific packet
///
/// Different packets require different pulse timeouts.
///
/// \param  packet  Packet
/// \return Pulse timeout in [µs]
constexpr uint32_t packet2timeout(Packet const& packet) {
  // ZSU security bytes
  if (auto const count{size(packet)};
      count == 1uz && (packet[0uz] == 0x55u || packet[0uz] == 0xAAu))
    return Timeouts::zsu_security_bytes;
  // ZPP flash erase
  else if (count == 4uz && packet[0uz] == 0x03u && packet[1uz] == 0x55u &&
           packet[2uz] == 0xFFu && packet[3uz] == 0xFFu)
    return Timeouts::zpp_flash_erase;
  // ZPP and ZSU flash writes
  else if (count >= 34uz) {
    static_assert(Timeouts::zpp_flash_write == Timeouts::zsu_blocks);
    return Timeouts::zpp_flash_write;
  } else if (count == 6uz && packet[0uz] == 0x06u && packet[1uz] == 0xAA)
    return Timeouts::zpp_cv_write;
  // Default
  else {
    static_assert(
      std::ranges::all_of(std::array{Timeouts::zpp_cv_read,
                                     Timeouts::zpp_decoder_id,
                                     Timeouts::zpp_crc_or_xor,
                                     Timeouts::zsu_decoder_id,
                                     Timeouts::zsu_block_count},
                          [](auto t) { return t == Timeouts::zpp_cv_read; }));
    return Timeouts::zpp_cv_read;
  }
}

} // namespace decup
