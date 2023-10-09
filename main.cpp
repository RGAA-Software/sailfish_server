#include <memory>
#include <iostream>

#include "Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"
#include "ui/MainPanel.h"
#include "context/Context.h"
#include "context/Settings.h"

#include <QApplication>
#include <QFontDatabase>

using namespace rgaa;

int main(int argc, char** argv) {

    CaptureDump();

    QApplication qApplication(argc, argv);

    // font family : ScreenMatrix
    int id = QFontDatabase::addApplicationFont(":/resources/font/matrix.ttf");
    qDebug() << "font family : " << QFontDatabase::applicationFontFamilies(id) ;
    id = QFontDatabase::addApplicationFont(":/resources/font/LOVE-Queen-2.ttf");
    qDebug() << "font family : " << QFontDatabase::applicationFontFamilies(id) ;

    Logger::Init("sailfish_server.log", false);

    auto context = std::make_shared<Context>();
    context->Init();

    MainPanel workspace(context, nullptr);
    workspace.Show();

    return QApplication::exec();
}
