
#include <iostream>

#include <znr/znr.h>

int main(int argc, char* argv[])
{
    znr::init("listener");

    auto fn = znr::handle_fn([](std::string_view& msg){
        std::cout << "Got message: " << msg << std::endl;
    });

    auto sub = znr::subscribe({"/msg"}, fn);

    znr::spin();
    return 0;
}
