
#include "znr/this_node.h"
#include "znr/publisher.h"

namespace z = zenohc;

znr::Publisher::Publisher(z::Publisher& publisher)
    : zpub(publisher.take())
{}

znr::Publisher::~Publisher()
{
    if (not zpub.check())
        return;
    this_node::delete_resource(std::string(zpub.get_keyexpr().as_string_view()));
}

void znr::Publisher::publish(const char message[])
{
    return publish(std::string(message));
}

void znr::Publisher::publish(const std::string& message)
{
    z::PublisherPutOptions options;
    options.set_encoding(Z_ENCODING_PREFIX_TEXT_PLAIN);

    zpub.put(message, options);
}


