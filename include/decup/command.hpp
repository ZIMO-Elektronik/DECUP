// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// Command
///
/// \file   decup/command.hpp
/// \author Vincent Hamp
/// \date   12/08/2024

#pragma once

#include <cstdint>

namespace decup {

/// Command codes
enum class Command : uint8_t {
  CvRead = 1u,
  CvWrite = 2u,
  DeleteFlash = 3u,
  ReadDecoderType = 4u,
  XORData = 3u,  // WTF
  CRCData = 4u,  // ?
  CRCorXORQuery = 7u,
  Preamble = 0xEFu,
};

}  // namespace decup