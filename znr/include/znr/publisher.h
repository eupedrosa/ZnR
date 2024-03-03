
#pragma once

#include <zenohc.hxx>

namespace znr {

struct Publisher {
    zenohc::Publisher zpub;

    Publisher(zenohc::Publisher& publisher);
    virtual ~Publisher();

    void publish(const char message[]);
    void publish(const std::string& message);

    template <typename M>
    void publish(const M& message);
};

}// namespace znr
