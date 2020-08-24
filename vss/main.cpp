// QCoreApplication from Qt
#include <QApplication>

// ExitHandler and InstanceChecker
#include <src/exithandler.h>
#include <src/instancechecker.h>

// VSS app
#include <src/vss.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Maracatronics-VSS");
    app.setApplicationVersion("0.0.1");

    // Duplicated instance checking
    InstanceChecker::waitIfDuplicated(app.applicationName());

    // Setup ExitHandler
    ExitHandler::setApplication(&app);
    ExitHandler::setup();

    // Suassuna parameters (with default values)
    quint8 ourTeamId = 0;
    Colors::Color ourTeamColor = Colors::YELLOW;
    FieldSide ourFieldSide = Sides::RIGHT;

    // Create and start VSS app
    VSS vssApp(ourTeamId, ourTeamColor, ourFieldSide);
    vssApp.start();

    // Block main thread
    bool exec = app.exec();

    // Stop VSS app
    vssApp.stop();

    return exec;
}
