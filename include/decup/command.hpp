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
enum struct Command : uint8_t {
  CvRead = 1u,
  CvWriteDeprecated = 2u,
  DeleteFlash = 3u,
  ReadDecoderType = 4u,
  XORData = 3u,
  CRCData = 4u,
  CvWrite = 6u,
  CRCorXORQuery = 7u,
  CvSet = 9u,
  Preamble0 = 0xBFu,
  Preamble1 = 0xEFu,
};

/// CV-Set subcommand codes
enum struct CvSetSubcommand : uint8_t {
  CvWrite = 4u,
  CvWriteStart = 8u,
  CvWriteEnd = 12u,
  FeatureRequest = 16u,
  ChangePage = 20u,
};

} // namespace decup
