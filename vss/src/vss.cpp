#include "vss.h"

VSS::VSS(quint8 teamId, Colors::Color teamColor, FieldSide teamSide) :
    _teamId(teamId), _teamColor(teamColor), _teamSide(teamSide)
{
    // Creating controller
    _ctr = new Controller();

    // Setting server adress and port
    setServerAddress("localhost", 0);
}

void VSS::setServerAddress(QString ipAddress, int port){
    // Setting server info
    _serverAddress = ipAddress;
    _serverPort = port;
}

bool VSS::connectToServer(){
    // Try to connect
    if(!_ctr->connect(_serverAddress, _serverPort))
        return false;

    // Check if Sensor is connected (2 teams connected)
    if(_ctr->teams().size() < 2){
        std::cout << "[ERROR] No teams on controller, sensor is not connected!\n";
        return false;
    }

    // If all succeed, return true
    return true;
}

bool VSS::start(){
    // Parse opTeam info
    quint8 opTeamId = (_teamId) ? 0 : 1;
    Colors::Color opTeamColor = (_teamColor == Colors::YELLOW) ? Colors::BLUE : Colors::YELLOW;
    FieldSide opTeamSide = (_teamSide.isRight()) ? Sides::LEFT : Sides::RIGHT;

    // Connect to server
    if(!connectToServer()){
        std::cout << "[ERROR] Not possible to connect to controller, check your network.\n";
        return false;
    }

    // Create World
    _world = new World(_ctr, new Fields::VSSField());

    // Setup teams in world
    setupTeams(opTeamId, opTeamColor, opTeamSide);
    _world->setTeams(_ourTeam, _opTeam);

    // Setup players to team
    setupOurPlayers();
    setupOpPlayers(opTeamId);

    // Create coach
    _coach = new Coach(_ourTeam, _opTeam);
    _world->setControlModule(_coach);

    // Setup strategy for coach
    Strategy *strategy = NULL;
    strategy = new MRCStrategy();
    _coach->setStrategy(strategy);

    // Setup GUI
    _gui = new VSSGui(_ourTeam, _opTeam);
    _world->addEntity(_gui, 3);

    // Start world
    _world->start();

    // If all succeed, return true
    return true;
}

void VSS::stop(){
    // Stop world entities and wait for it
    _world->stopEntity();
    _world->wait();

    // Delete teams
    delete _ourTeam;
    delete _opTeam;

    // Delete modules
    delete _world;

    // Disconnect controller
    _ctr->disconnect();
}

void VSS::setupTeams(quint8 opTeamId, Colors::Color opTeamColor, FieldSide opTeamSide){
    // Create teams
    _ourTeam = new VSSTeam(_teamId, _world->getWorldMap());
    _opTeam = new VSSTeam(opTeamId, _world->getWorldMap());

    // Set opponent teams
    _ourTeam->setOpponentTeam(_opTeam);
    _opTeam->setOpponentTeam(_ourTeam);

    // Set our team info
    _ourTeam->setTeamColor(_teamColor);
    _ourTeam->setTeamSide(_teamSide);

    // Set op team info
    _opTeam->setTeamColor(opTeamColor);
    _opTeam->setTeamSide(opTeamSide);
}

void VSS::setupOurPlayers(){
    // Create our players
    QList<quint8> playerList = _world->getWorldMap()->players(_teamId);
    for(quint8 i = 0; i < playerList.size() && i < VSSConstants::qtPlayers(); i++){
        // Create player pointer
        VSSPlayer *player = new VSSPlayer(playerList.at(i), _ourTeam, _ctr, new Role_Halt());

        // Enable player
        player->enable(true);

        // Add to team
        _ourTeam->addPlayer(player);

        // Add to world
        _world->addEntity(player, 2);
    }
}

void VSS::setupOpPlayers(quint8 opTeamId){
    // Create op players
    QList<quint8> playerList = _world->getWorldMap()->players(opTeamId);
    for(quint8 i = 0; i < playerList.size() && i < VSSConstants::qtPlayers(); i++){
        // Create player pointer
        VSSPlayer *player = new VSSPlayer(playerList.at(i), _opTeam, NULL, new Role_Halt());

        // Enable player
        player->enable(false);

        // Add to team
        _opTeam->addPlayer(player);

        // Add to world
        _world->addEntity(player, 2);
    }
}
