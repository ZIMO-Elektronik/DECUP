// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// Packet
///
/// \file   decup/packet.hpp
/// \author Vincent Hamp
/// \date   12/08/2024

#pragma once

#include <cstdint>
#include <ztl/inplace_vector.hpp>

namespace decup {

using Packet = ztl::inplace_vector<uint8_t, DECUP_MAX_PACKET_SIZE>;

}  // namespace decup