
#include "znr/publisher.h"

namespace z = zenohc;

znr::Publisher::Publisher(z::Publisher& publisher)
    : zpub(publisher.take())
{}

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

