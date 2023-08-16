#include <memory>
#include <iostream>

#include "Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"
#include "ui/Workspace.h"
#include "context/Context.h"
#include <QApplication>

using namespace rgaa;

int main(int argc, char** argv) {

    CaptureDump();

    QApplication qApplication(argc, argv);

    Logger::Init("sailfish_server.log", false);

    auto context = std::make_shared<Context>();
    context->Init();

    Workspace workspace(context, nullptr);
    workspace.Show();

    return QApplication::exec();
}
