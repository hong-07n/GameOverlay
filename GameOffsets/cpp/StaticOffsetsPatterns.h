#pragma once

#include "Pattern.h"
#include <array>

namespace GameOffsets {

    /**
     * @brief A struct containing static pattern definitions for game memory scanning
     */
    struct StaticOffsetsPatterns {
        /// Array of predefined patterns for memory scanning
        static const std::array<Pattern, 6> Patterns;
    };

} // namespace GameOffsets