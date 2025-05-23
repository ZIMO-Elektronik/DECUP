// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// Utility
///
/// \file   decup/utility.hpp
/// \author Vincent Hamp
/// \date   24/10/2024

#pragma once

#include <cstdint>
#include <ztl/inplace_vector.hpp>
#include "packet.hpp"

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

/// Get pulse timeout in [µs] for specific packet
///
/// Different packets require different pulse timeouts.
///
/// \param  packet  Packet
/// \return Pulse timeout in [µs]
constexpr uint32_t packet2pulse_timeout(Packet const& packet) {
  switch (size(packet)) {
    // Timeout for security bytes used in ZSU update
    case 1uz:
      if (packet[0uz] == 0x55u || packet[0uz] == 0xAAu) return 5'000u;
      break;

    // Timeout for ZSU update packets
    case 34uz: [[fallthrough]];
    case 66uz: return 100'000u;
  }
  return 1'000u;
}

} // namespace decup
