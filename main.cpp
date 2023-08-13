#include <memory>
#include <iostream>

#include "Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"

using namespace rgaa;

int main() {

    CaptureDump();

    Logger::Init("sailfish_server.log", false);

    auto app = std::make_shared<Application>();
    app->Init();
    app->Start();

    return 0;
}
