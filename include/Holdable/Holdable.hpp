#pragma once

struct Holdable {
    virtual ~Holdable() = default;

    bool held{};
};
