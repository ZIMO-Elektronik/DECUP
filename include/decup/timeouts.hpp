// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// Timeouts
///
/// \file   decup/timeouts.hpp
/// \author Vincent Hamp
/// \date   16/11/2025

#pragma once

namespace decup {

/// Response pulse timeouts in [µs]
struct Timeouts {
  static constexpr auto pulse_interval{200u}; // Interval between pulses
  static constexpr auto preamble{300u};       // No response

  // ZPP
  static constexpr auto zpp_preamble{preamble}; // No response
  static constexpr auto zpp_cv_read{1'000u};
  static constexpr auto zpp_cv_write{5'000u};
  static constexpr auto zpp_flash_erase{200u * 1'000u * 1'000u};
  static constexpr auto zpp_flash_write{100u * 1'000u};
  static constexpr auto zpp_decoder_id{1'000u};
  static constexpr auto zpp_crc_or_xor{1'000u};

  // ZSU
  static constexpr auto zsu_preamble{preamble}; // No response
  static constexpr auto zsu_decoder_id{1'000u};
  static constexpr auto zsu_block_count{1'000u};
  static constexpr auto zsu_security_bytes{5'000u};
  static constexpr auto zsu_blocks{100u * 1'000u};
};

} // namespace decup
