
#include <znr/znr.h>
#include <znr/this_node.h>
#include <znr/subscriber.h>

#include <type_traits>
#include <iostream>

#include <functional>

int main(int argc, char* argv[])
{
    znr::init("listener");

    auto fn = znr::handle_fn([](std::string_view& msg){
        std::cout << "Got message: " << msg << std::endl;
    });

    auto sub = znr::this_node::subscribe({"/msg"}, fn);

    znr::spin();
    return 0;
}
