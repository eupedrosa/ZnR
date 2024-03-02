
#include <thread>
#include <chrono>
#include <cstdlib>

#include <iostream>

#include <znr/znr.h>

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    znr::init("talker");

    auto pub = znr::advertise({"/msg"});

    unsigned int i = 0;
    while ( znr::ok() ) {
        auto msg = "hello world [" + std::to_string(i++) + "]";
        std::cout << "sending message: " << msg << std::endl;
        pub.publish(msg);

        std::this_thread::sleep_for(1s);
    }

    return 0;
}
