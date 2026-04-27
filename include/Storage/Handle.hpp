#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

template<typename T, typename Rep = uint32_t>
requires std::is_arithmetic_v<Rep> // ensure the Representation is a number
struct Handle {
    Rep ID = INVALID_ID;
    Rep GENERATION = INVALID_GEN;

    const Rep INVALID_ID = std::numeric_limits<Rep>::max();
    const Rep INVALID_GEN = std::numeric_limits<Rep>::max();

    bool isValid() const {
        return ID != INVALID_ID && GENERATION != INVALID_GEN;
    }

    bool operator==(const Handle<T, Rep>& other) {
        return this->ID == other.ID && this->GENERATION == other.GENERATION;
    }
};
