
#pragma once

#include <zenohc.hxx>

#include "znr/publisher.h"
#include "znr/subscriber.h"

namespace znr {

using PublisherOptions = zenohc::PublisherOptions;
using SubscriberOptions = zenohc::SubscriberOptions;
using SubscriberCallback = zenohc::ClosureSample;

namespace this_node {

std::string hostname();

void open_session(const std::string_view& ns);
void close_session();

void register_resource(const std::string& name);
void delete_resource(const std::string& name);

void get_resources(std::vector<std::string>& resources);

Publisher advertise(const std::string& topic,
                    const PublisherOptions& options = {});

Subscriber subscribe(const std::string& topic,
                     SubscriberCallback&& callback,
                     const SubscriberOptions& options = {});

}}// namespace znr::this_node
