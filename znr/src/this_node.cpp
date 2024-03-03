
#include <unistd.h>

#include <iostream>

#include "znr/this_node.h"

namespace z = zenohc;

namespace /* anon */ {

static z::KeyExpr base_key("znr");
static z::KeyExpr key_namespace(nullptr);
static z::Session session(nullptr);

static z::Subscriber node_register(nullptr);

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

void on_node_register(const z::Sample& sample){
    auto kind = sample.get_kind();
    if (kind == Z_SAMPLE_KIND_DELETE)
        return;

    auto encoding = sample.get_encoding();
    if (encoding != z::Encoding(Z_ENCODING_PREFIX_TEXT_PLAIN)){
        throw std::runtime_error("Expected text/plain message, received "
                                 + encoding.get_prefix());
    }

    auto payload = sample.get_payload().as_string_view();
    if (payload == key_namespace.as_string_view()){
        std::cerr << "A new node with the same name was launched. Exiting..." << std::endl;
        exit(0);
    }
}

}// namespace *anon*

std::string znr::this_node::hostname()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);

    return std::string(name);
}

void znr::this_node::open_session(const std::string_view& ns)
{
    z::Config config;
    z::Session zs = z::expect<z::Session>(z::open(std::move(config)));
    session = z::Session(zs.take());

    key_namespace = z::KeyExpr(ns.data());

    // Advertise that a new node with name = ns is booting..
    // The registration is separated by hostname.
    auto reg_key = "znr/@/" + hostname() + "/register";
    z::PutOptions po; po.set_encoding(z::Encoding(Z_ENCODING_PREFIX_TEXT_PLAIN));
    session.put(reg_key, ns, po);
    // install: new node name register
    node_register = z::expect<z::Subscriber>(
        session.declare_subscriber(reg_key, on_node_register));
}

void znr::this_node::close_session()
{
    node_register.drop();
    session.drop();
}

znr::Publisher znr::this_node::advertise(const std::string& topic,
                                         const PublisherOptions& options)
{
    auto key = resolve("topic", topic);
    z::Publisher zpub = z::expect<z::Publisher>(session.declare_publisher(key, options));
    return Publisher(zpub);
}

znr::Subscriber znr::this_node::subscribe(const std::string& topic,
                                          SubscriberCallback&& callback,
                                          const SubscriberOptions& options)
{
    auto key = resolve("topic", topic);
    auto zsub = z::expect<z::Subscriber>(session.declare_subscriber(key, std::move(callback), options));
    return Subscriber(zsub);
}
