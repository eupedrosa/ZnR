
#include <iostream>

#include <znr/znr.h>
#include <znr/vendor/json.hpp>

int main(int argc, char* argv[])
{
    znr::init("listener");

    auto fn = znr::handle_string([](std::string_view& msg){
        std::cout << "Got message: " << msg << std::endl;
    });

    auto fn0 = znr::handle_json([](const znr::JSON& msg){
        std::cout << "Got message: " << msg << std::endl;
    });


    auto sub = znr::subscribe({"/msg"}, fn);

    znr::spin();
    return 0;
}
