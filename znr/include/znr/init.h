
#pragma once

#include <string>

namespace znr {

void init(const std::string& ns);
bool isInitialized();
bool ok();
void spin();

void shutdown();

}// namespace znr
