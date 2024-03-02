
#include "znr/subscriber.h"

namespace z = zenohc;

std::function<void(const z::Sample&)>
znr::handle_fn(std::function<void(std::string_view&)> fn) {
    return [=](const zenohc::Sample& sample){

        auto kind = sample.get_kind();
        if (kind == Z_SAMPLE_KIND_DELETE)
            return;

        auto encoding = sample.get_encoding();
        if (encoding != z::Encoding(Z_ENCODING_PREFIX_TEXT_PLAIN)){
            throw std::runtime_error("Expected text/plain message, received "
                                     + encoding.get_prefix());
        }

        auto payload = sample.get_payload().as_string_view();
        return fn(payload);
    };
}

znr::Subscriber::Subscriber(z::Subscriber& subscriber)
{
    zsub = new z::Subscriber(subscriber.take());
}

