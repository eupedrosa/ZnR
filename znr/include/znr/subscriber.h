
#pragma once

#include <functional>

#include <zenohc.hxx>

namespace znr {

std::function<void(const zenohc::Sample&)>
handle_fn(std::function<void(std::string_view&)> fn);

struct Subscriber {
    zenohc::Subscriber* zsub;

    Subscriber(zenohc::Subscriber& publisher);
    virtual ~Subscriber() { delete zsub; }
};

}// namespace znr
