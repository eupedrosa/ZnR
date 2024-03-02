
#include "znr/this_node.h"

#include <iostream>

namespace z = zenohc;

namespace /* anon */ {

static z::KeyExpr base_key("znr");
static z::KeyExpr key_namespace("");
static z::Session* session = nullptr;

// Resolve key using the following rules:
//  1. @name starts with '/' -> znr/@prefix/@name
//  2. @name starts with '~' -> znr/@prefix/@key_namespace/@name[1:]
//  3. otherwise             -> znr/@prefix/@key_namespace/@name
//
//  returns the resolved key string.
auto resolve(const std::string& prefix, const std::string& name) {
    auto base = base_key.as_keyexpr_view();
    auto ns = key_namespace.as_keyexpr_view();
    auto key = base.join(prefix);


    if (name[0] == '/') {
        key = key.concat(name);
    } else if (name[0] == '~') {
        key = key.join(ns).join(name.substr(1));
    } else {
        key = key.join(ns).join(name);
    }

    return std::string(key.as_string_view());
}

}// namespace *anon*

void znr::this_node::open_session(const std::string_view& ns)
{
    z::Config config;
    z::Session zs = z::expect<z::Session>(z::open(std::move(config)));
    session = new z::Session(zs.take());

    key_namespace = z::KeyExpr(ns.data());
}

void znr::this_node::close_session()
{
    session->drop();
    delete session;
}

znr::Publisher znr::this_node::advertise(const std::string& topic,
                                         const PublisherOptions& options)
{
    auto key = resolve("topic", topic);
    z::Publisher zpub = z::expect<z::Publisher>(session->declare_publisher(key, options));
    return Publisher(zpub);
}

znr::Subscriber znr::this_node::subscribe(const std::string& topic,
                                          SubscriberCallback&& callback,
                                          const SubscriberOptions& options)
{
    auto key = resolve("topic", topic);
    auto zsub = z::expect<z::Subscriber>(session->declare_subscriber(key, std::move(callback), options));
    return Subscriber(zsub);
}
