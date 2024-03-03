
#pragma once

#include <functional>

#include <zenohc.hxx>

#include "znr/vendor/json_fwd.hpp"

namespace znr {

using JSON = nlohmann::json;

std::function<void(const zenohc::Sample&)>
handle_string(std::function<void(std::string_view&)> fn);

std::function<void(const zenohc::Sample&)>
handle_json(std::function<void(const JSON&)> fn);

struct Subscriber {
    zenohc::Subscriber zsub;

    Subscriber(zenohc::Subscriber& publisher);
    virtual ~Subscriber();
};

}// namespace znr
