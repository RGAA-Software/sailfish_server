#include <memory>
#include <iostream>

#include "Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"
#include "ui/Workspace.h"

#include <QApplication>

using namespace rgaa;

int main(int argc, char** argv) {

    CaptureDump();

    QApplication qApplication(argc, argv);

    Logger::Init("sailfish_server.log", false);

//    auto app = std::make_shared<Application>();
//    app->Init();
//    app->Start();

    Workspace workspace(nullptr, nullptr);
    workspace.Show();

    return QApplication::exec();
}
