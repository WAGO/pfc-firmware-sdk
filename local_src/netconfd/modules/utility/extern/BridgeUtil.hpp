#pragma once

#include <memory>

#include "Status.hpp"
#include "BaseTypes.hpp"

namespace netconf {

void SetMacLearning(::std::uint32_t if_index, MacLearning learning);
MacLearning GetMacLearning(::std::uint32_t if_index);

}  // namespace netconf
