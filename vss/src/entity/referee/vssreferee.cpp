#include "vssreferee.h"

QString VSSReferee::name(){
    return "VSSReferee";
}

VSSReferee::VSSReferee(VSSTeam *ourTeam) : Entity(ENT_REFEREE)
{
    // Creating and connecting to network
    _replacerSocket = new QUdpSocket();
    _client = new VSSClient(VSSConstants::refereePort(), VSSConstants::refereeAddress().toStdString());

    // Setting our team
    _ourTeam = ourTeam;
}

bool VSSReferee::connect(){
    /// Replacer Socket
    // Creating replacer socket
    _replacerSocket = new QUdpSocket();

    // Performing connection to send replacer commands
    if(_replacerSocket->isOpen()){
        _replacerSocket->close();
    }

    _replacerSocket->connectToHost(VSSConstants::refereeAddress(), static_cast<quint16>(VSSConstants::replacerPort()), QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);
    std::cout << "[VSSReferee] Conected to REPLACER socket in port " << VSSConstants::replacerPort() << " and address " << VSSConstants::refereeAddress().toStdString() << std::endl;


    /// Referee Socket
    // Creating referee socket
    _refereeSocket = new QUdpSocket();

    // Performing connection to send receive referee data
    if(_refereeSocket->bind(QHostAddress::AnyIPv4, static_cast<quint16>(VSSConstants::refereePort()), QUdpSocket::ShareAddress) == false){
        std::cout << "[ERROR] VSSReferee bind error =(" << std::endl;
        return false;
    }

    return 1;
}

void VSSReferee::initialization(){
    if(connect())
        std::cout << "[VSSReferee] Thread started." << std::endl;
    else
        return ;
}

void VSSReferee::loop(){
    VSSRef::ref_to_team::VSSRef_Command command;

    char buffer[65535];
    long long int len = 0;

    if(_enableTimer){
        _timer.stop();
        // wait for 2s to pos all
        if(_timer.timesec() >= 2.0){
            placeReceivedPackets();

            // Reseting
            _timer.start();
            _enableTimer = false;
        }
    }

    while(_refereeSocket->hasPendingDatagrams()){
        // Read
        len = _refereeSocket->readDatagram(buffer, 65535);

        // Parse protobuf structure
        if(command.ParseFromArray(buffer, int(len)) == false){
            std::cout << "[ERROR] Referee protubuf parsing error =(" << std::endl;
        }

        // Save in _lastCommand
        _commandMutex.lock();
        _lastCommand = command.foul();
        _commandMutex.unlock();

        // If received command, let's debug it
        std::cout << "[VSSReferee] Succesfully received an command from ref: " << getFoulNameById(command.foul()).toStdString() << " for team " << getTeamColorNameById(command.teamcolor()).toStdString() << std::endl;
        std::cout << "[VSSReferee] Quadrant: " << getQuadrantNameById(command.foulquadrant()).toStdString() << std::endl;

        // Showing timestamp
        std::cout << "[VSSReferee] Timestamp: " << command.timestamp() << std::endl;

        // Placing if is an valid foul (not stop or game on)
        if(!isStop() && !isGameOn()){
            _enableTimer = true;
            _timer.start();

            emit emitFoul(command.foul(), command.foulquadrant(), command.teamcolor());
        }
    }
}

bool VSSReferee::isStop(){
    _commandMutex.lock();
    VSSRef::Foul command = _lastCommand;
    _commandMutex.unlock();

    return (command == VSSRef::Foul::STOP);
}

bool VSSReferee::isGameOn(){
    _commandMutex.lock();
    VSSRef::Foul command = _lastCommand;
    _commandMutex.unlock();

    return (command == VSSRef::Foul::GAME_ON);
}

void VSSReferee::finalization(){
    // Closing client and socket
    _client->close();

    if(_replacerSocket->isOpen()){
        _replacerSocket->close();
    }

    std::cout << "[VSSReferee] Thread ended." << std::endl;
}

QString VSSReferee::getFoulNameById(VSSRef::Foul foul){
    switch(foul){
        case VSSRef::Foul::FREE_BALL:    return "FREE_BALL";
        case VSSRef::Foul::FREE_KICK:    return "FREE_KICK";
        case VSSRef::Foul::GOAL_KICK:    return "GOAL_KICK";
        case VSSRef::Foul::PENALTY_KICK: return "PENALTY_KICK";
        case VSSRef::Foul::KICKOFF:      return "KICKOFF";
        case VSSRef::Foul::STOP:         return "STOP";
        case VSSRef::Foul::GAME_ON:      return "GAME_ON";
        default:                         return "FOUL NOT IDENTIFIED";
    }
}

QString VSSReferee::getTeamColorNameById(VSSRef::Color color){
    switch(color){
        case VSSRef::Color::NONE:    return "NONE";
        case VSSRef::Color::BLUE:    return "BLUE";
        case VSSRef::Color::YELLOW:  return "YELLOW";
        default:                     return "COLOR NOT IDENTIFIED";
    }
}

QString VSSReferee::getQuadrantNameById(VSSRef::Quadrant quadrant){
    switch(quadrant){
        case VSSRef::Quadrant::NO_QUADRANT: return "NO QUADRANT";
        case VSSRef::Quadrant::QUADRANT_1:  return "QUADRANT 1";
        case VSSRef::Quadrant::QUADRANT_2:  return "QUADRANT 2";
        case VSSRef::Quadrant::QUADRANT_3:  return "QUADRANT 3";
        case VSSRef::Quadrant::QUADRANT_4:  return "QUADRANT 4";
        default:                            return "QUADRANT NOT IDENTIFIED";
    }
}

QString VSSReferee::getHalfNameById(VSSRef::Half half){
    switch(half){
        case VSSRef::Half::NO_HALF: return "NO_HALF";
        case VSSRef::Half::FIRST_HALF: return "FIRST HALF";
        case VSSRef::Half::SECOND_HALF: return "SECOND HALF";
        default: return "NO HALF DEFINED";
    }
}

void VSSReferee::placeReceivedPackets(){
    // First creating an placement command
    VSSRef::team_to_ref::VSSRef_Placement placementCommand;
    VSSRef::Frame *placementFrame = new VSSRef::Frame();
    placementFrame->set_teamcolor((_ourTeam->teamColor() == Colors::BLUE) ? VSSRef::Color::BLUE : VSSRef::Color::YELLOW);

    for(int i = 0; i < 5; i++){
        _positionMutex.lockForRead();
        if(_desiredMark[i]){
            VSSRef::Robot *robot = placementFrame->add_robots();
            robot->set_robot_id(static_cast<quint32>(i));
            robot->set_x(static_cast<double>(_desiredPlacement[i].first.x()));
            robot->set_y(static_cast<double>(_desiredPlacement[i].first.y()));
            robot->set_orientation(static_cast<double>(_desiredPlacement[i].second.value()));

            _desiredMark[i] = false;
        }
        _positionMutex.unlock();
    }

    // Setting frame in command
    placementCommand.set_allocated_world(placementFrame);

    // Sending placement command
    std::string placementMsg;
    placementCommand.SerializeToString(&placementMsg);
    if(_replacerSocket->write(placementMsg.c_str(), static_cast<qint64>(placementMsg.length())) == -1){
        std::cout << "[VSSReferee] Failed to write to replacer socket: " << _replacerSocket->errorString().toStdString() << std::endl;
    }
}

void VSSReferee::receivePosition(quint8 playerId, Position desiredPosition, Angle desiredOrientation){
    _positionMutex.lockForWrite();

    _desiredPlacement[playerId].first = desiredPosition;
    _desiredPlacement[playerId].second = desiredOrientation;
    _desiredMark[playerId] = true;

    _positionMutex.unlock();
}
