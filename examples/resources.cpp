
#include <iostream>

#include <znr/znr.h>
#include <znr/vendor/json.hpp>

int main(int argc, char* argv[])
{
    std::cout << "gathering ..." << std::flush;
    znr::init("resources");

    std::vector<std::string> resources;
    znr::get_resources(resources);

    std::cout << "\33[2K\r" << std::flush;
    for (auto& res : resources)
        std::cout << res << std::endl;

    znr::shutdown();
    return 0;
}
