
#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include "znr/vendor/json.hpp"
#include "znr/this_node.h"

namespace z = zenohc;
using JSON = nlohmann::json;

namespace /* anon */ {

static z::KeyExpr base_key("znr");
static z::KeyExpr base_admin_key("znr/@");
static z::KeyExpr key_namespace(nullptr);
static z::Session session(nullptr);

static z::Subscriber node_register(nullptr);

static z::Queryable resource_scout(nullptr);
static z::KeyExpr rsq_key(nullptr);

std::unordered_map<std::string, int> resource_ledger;

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

void on_resource_scout(const z::Query& query)
{
    z::QueryReplyOptions options;
    options.set_encoding(z::Encoding(Z_ENCODING_PREFIX_APP_JSON));

    JSON json(resource_ledger);

    query.reply(rsq_key, json.dump(), options);
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
    // hostname as namespace
    auto hostns = hostname();

    z::Config config;
    z::Session zs = z::expect<z::Session>(z::open(std::move(config)));
    session = z::Session(zs.take());

    base_admin_key = base_admin_key.join(hostns);
    key_namespace = z::KeyExpr(ns.data());

    // Advertise that a new node with name = ns is booting..
    // The registration is separated by hostname.
    auto reg_key = base_admin_key.join("register");
    z::PutOptions po; po.set_encoding(z::Encoding(Z_ENCODING_PREFIX_TEXT_PLAIN));
    session.put(reg_key, ns, po);
    // install: new node name register
    node_register = z::expect<z::Subscriber>(
        session.declare_subscriber(reg_key, on_node_register));

    // Advertise resource queryable
    rsq_key = base_admin_key.join(std::string(ns)).join("resources");
    resource_scout = z::expect<z::Queryable>(
        session.declare_queryable(rsq_key, on_resource_scout));
}

void znr::this_node::close_session()
{
    resource_scout.drop();
    node_register.drop();
    session.drop();
}

void znr::this_node::register_resource(const std::string& name)
{
    auto it = resource_ledger.find(name);
    if ( it == resource_ledger.end() ){
        resource_ledger.insert({name, 1});
    } else {
        it->second++;
    }
}

void znr::this_node::delete_resource(const std::string& name)
{
    auto it = resource_ledger.find(name);
    if ( it == resource_ledger.end() )
        return;

    it->second--;
    if (it->second == 0)
        resource_ledger.erase(it);
}

void znr::this_node::get_resources(std::vector<std::string>& resources)
{
    auto [send, recv] = z::reply_fifo_new(16);
    z::GetOptions opts;
    opts.set_target(Z_QUERY_TARGET_ALL);
    session.get("znr/@/*/*/resources", "", std::move(send), opts);

    z::Reply reply(nullptr);
    for (recv(reply); reply.check(); recv(reply)) {
        auto sample = z::expect<z::Sample>(reply.get());
        JSON json = nlohmann::json::parse(sample.get_payload().as_string_view());

        for (auto& [key, _] : json.items())
            resources.push_back(key);
    }
}

znr::Publisher znr::this_node::advertise(const std::string& topic,
                                         const PublisherOptions& options)
{
    auto key = resolve("topic", topic);
    z::Publisher zpub = z::expect<z::Publisher>(session.declare_publisher(key, options));

    register_resource(key);
    return Publisher(zpub);
}

znr::Subscriber znr::this_node::subscribe(const std::string& topic,
                                          SubscriberCallback&& callback,
                                          const SubscriberOptions& options)
{
    auto key = resolve("topic", topic);
    auto zsub = z::expect<z::Subscriber>(session.declare_subscriber(key, std::move(callback), options));

    register_resource(key);
    return Subscriber(zsub);
}
