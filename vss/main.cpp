// QCoreApplication from Qt
#include <QApplication>

// ExitHandler and InstanceChecker
#include <src/exithandler.h>
#include <src/instancechecker.h>

// VSS app
#include <src/vss.h>

// Aux functions
Colors::Color validateTeamColor(const QString &input, bool *valid) {
    *valid = true;
    if(input.toLower()=="yellow")
        return Colors::YELLOW;
    else if(input.toLower()=="blue")
        return Colors::BLUE;
    else {
        *valid = false;
        return Colors::YELLOW; // return default
    }
}

FieldSide validateFieldSide(const QString &input, bool *valid) {
    *valid = true;
    if(input.toLower()=="right")
        return Sides::RIGHT;
    else if(input.toLower()=="left")
        return Sides::LEFT;
    else {
        *valid = false;
        return Sides::RIGHT; // return default
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Maracatronics-VSS");
    app.setApplicationVersion("0.0.1");

    // Duplicated instance checking
    //InstanceChecker::waitIfDuplicated(app.applicationName());

    // Command line parser, get arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("VSS application help.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("teamColor", "Sets the team color ('yellow' or 'blue', default='yellow').");
    parser.process(app);
    QStringList args = parser.positionalArguments();

    // Suassuna parameters (with default values)
    quint8 ourTeamId = 0;
    Colors::Color ourTeamColor = Colors::YELLOW;
    FieldSide ourFieldSide = Sides::RIGHT;

    /// Check arguments
    // Team color
    if(args.size() >= 1) {
        bool valid;
        ourTeamColor = validateTeamColor(args.at(0), &valid);
        if(valid==false) {
            std::cout << ">> Armorial VSS: Invalid team color argument '" << args.at(0).toStdString() << "'.\n>> Please check help below.\n\n";
            parser.showHelp();
            return EXIT_FAILURE;
        }
    }

    /* this won't be needed for now
    // Field side
    if(args.size() >= 2) {
        bool valid;
        ourFieldSide = validateFieldSide(args.at(1), &valid);
        if(valid==false) {
            std::cout << ">> Armorial VSS: Invalid field side argument '" << args.at(1).toStdString() << "'.\n>> Please check help below.\n\n";
            parser.showHelp();
            return EXIT_FAILURE;
        }
    }
    */

    ourTeamId = (ourTeamColor == Colors::YELLOW) ? 0 : 1;
    ourFieldSide = (ourTeamColor == Colors::YELLOW) ? Sides::RIGHT : Sides::LEFT;

    // Setup ExitHandler
    ExitHandler::setApplication(&app);
    ExitHandler::setup();

    // Create and start VSS app
    VSS vssApp(ourTeamId, ourTeamColor, ourFieldSide);
    vssApp.start();

    // Block main thread
    bool exec = app.exec();

    // Stop VSS app
    vssApp.stop();

    return exec;
}
