
#include <csignal> // signal
#include <cstdlib> // atexit

#include <mutex>
#include <condition_variable>

#include "znr/init.h"
#include "znr/this_node.h"

namespace /* anon */ {

static std::mutex znr_start_mutex;
static std::mutex znr_shutdown_mutex;

static std::condition_variable znr_wait_shutdown;

static bool znr_started = false;
static bool znr_initialized = false;
static bool znr_shutting_down = false;
static bool znr_ok = false;

void atexitShutdown(){

    if (znr_ok) {
        znr::shutdown();
    }
}

void simpleSIGINT(int){
    znr::shutdown();
}

}// namespace *anon*

void znr::init(const std::string& ns)
{
    std::scoped_lock lock(znr_start_mutex);
    if (znr_started)
        return;

    znr_started = true;
    znr_ok = true;

    this_node::open_session(ns);

    // install a basic interrupt handler (e.g. ctrl-c)
    signal(SIGINT, simpleSIGINT);

    // make sure we have a clean exit
    atexit(atexitShutdown);

    znr_initialized = true;
}

bool znr::isInitialized()
{
    return znr_initialized;
}

bool znr::ok()
{
    return znr_ok;
}

void znr::spin()
{
    std::unique_lock lk(znr_shutdown_mutex);
    znr_wait_shutdown.wait(lk, []{ return znr_ok == false; });
}

void znr::shutdown()
{
    std::scoped_lock lock(znr_shutdown_mutex);
    if (znr_shutting_down)
        return;

    this_node::close_session();

    znr_shutting_down = true;
    znr_started = false;
    znr_ok = false;
    znr_wait_shutdown.notify_all();
}
