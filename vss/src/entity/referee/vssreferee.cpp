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

int VSSReferee::connect(){
    // Performing connection to send replacer commands
    if(_replacerSocket->isOpen()){
        _replacerSocket->close();
    }

    _replacerSocket->connectToHost(VSSConstants::refereeAddress(), VSSConstants::replacerPort(), QIODevice::WriteOnly, QAbstractSocket::IPv4Protocol);
    std::cout << "[VSSReferee] Conected to REPLACER socket in port " << VSSConstants::replacerPort() << " and address " << VSSConstants::refereeAddress().toStdString() << std::endl;

    // Performing connection to receive Referee foul commands
    if(_client->open(true))
        std::cout << "[VSSReferee] Listening to referee system on port " << VSSConstants::refereePort() << " and address = " << VSSConstants::refereeAddress().toStdString() << ".\n";
    else{
        std::cout << "[VSSReferee] Cannot listen to referee system on port " << VSSConstants::refereePort() << " and address = " << VSSConstants::refereeAddress().toStdString() << ".\n";
        return 0;
    }

    return 1;
}

void VSSReferee::initialization(){
    connect();
    std::cout << "[VSSReferee] Thread started." << std::endl;
}

void VSSReferee::loop(){
    VSSRef::ref_to_team::VSSRef_Command command;

    if(_client->receive(command) && !isInterruptionRequested()){
        // Saving command
        _commandMutex.lock();
        _lastCommand = command.foul();
        _commandMutex.unlock();

        // If received command, let's debug it
        std::cout << "[VSSReferee] Succesfully received an command from ref: " << getFoulNameById(command.foul()).toStdString() << " for team " << getTeamColorNameById(command.teamcolor()).toStdString() << std::endl;
        std::cout << "[VSSReferee] Quadrant: " << getQuadrantNameById(command.foulquadrant()).toStdString() << std::endl;

        // Showing timestamp
        std::cout << "[VSSReferee] Timestamp: " << command.timestamp() << std::endl;

        // Placing
        placeByCommand(command.foul(), command.foulquadrant(), command.teamcolor());
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
        default: "NO HALF DEFINED";
    }
}

void VSSReferee::placeByCommand(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color team){
    // First creating an placement command for the blue team
    VSSRef::team_to_ref::VSSRef_Placement placementCommand;
    VSSRef::Frame *placementFrame = new VSSRef::Frame();
    placementFrame->set_teamcolor((_ourTeam->teamColor() == Colors::BLUE) ? VSSRef::Color::BLUE : VSSRef::Color::YELLOW);

    /* Galera do VSS, aqui o processo é o seguinte:
     *
     * Primeiro, você vai criar um switch que vai abranger todas as faltas
     * Dentro de cada falta, você tem que verificar o time (caso seja necessaŕio).
     * Essas verificações de time geralmente são para Kickoff, Penalty e Goal_Kick
     * Para a falta de FREE_BALL vocês tem que ficar atento apenas ao quadrante!
     *
     * Feito essas verificações, vocês terão que fazer o posicionamento como bem entenderem!
     * Levem sempre em consideração o *LADO* que vocês estão para poder definir essas posições,
     * e lembrem-se sempre de se posicionarem RESPEITANDO as regras! O VSSReferee irá verificar
     * esse tipo de ato e possivelmente pode resultar em problemas caso não respeitemos.
     *
     * Para o posicionamento, vocês podem seguir o seguinte modelo:
     * for(int x = 0; x < 3; x++){
     *      VSSRef::Robot *robot = placementFrame->add_robots();
     *      robot->set_robot_id(x);
     *      robot->set_x(0.5);
     *      robot->set_y(-0.2 + (0.2 * x));
     *      robot->set_orientation(0.0);
     * }
     *
     * Explicando o que o código acima faz (lembrem-se que é um template):
     * Vocês vão criar um robô e adicioná-lo ao frame:
     * Vocês especificam o ID do robô de vocês
     * Vocês especificam x, y e orientação desejadas!
     *
     * Lembrem-se que o código acima é um exemplo: vocês podem obter informações sobre os jogadores
     * de vocês usando o ponteiro de VSSTeam que eu coloquei aqui no referee.
     * Isso pode permitir a vocês pegarem dados importantes sobre quem é o goleiro (da pra acessar
     * a role e descobrir essas coisas), o atacante e o suporte. Isso evita que vocês posicionem
     * os caras indevidamente em campo, cuidado com isso (ou façam tratamentos pra ele arrumar as
     * roles de acordo com o posicionamento do cara em campo, fica totalmente a critério).
     *
     * Feito esse posicionamento, o VSSReferee vai automaticamente tratar as colisões, então não
     * se preocupem com eventuais problemas de teleporte =)
     */

    switch(foul){
        // content here
    }

    if(!isGameOn() && !isStop()){
        // Pra testarem =))
        // Se lembrem que se o comando for game on ou stop n precisa posicionar!
        for(int x = 0; x < 3; x++){
            VSSRef::Robot *robot = placementFrame->add_robots();
            robot->set_robot_id(x);
            robot->set_x(0.5);
            robot->set_y(-0.2 + (0.2 * x));
            robot->set_orientation(0.0);
        }
    }


    // Setting frame in command
    placementCommand.set_allocated_world(placementFrame);

    // Sending placement command
    std::string placementMsg;
    placementCommand.SerializeToString(&placementMsg);
    if(_replacerSocket->write(placementMsg.c_str(), placementMsg.length()) == -1){
        std::cout << "[VSSReferee] Failed to write to replacer socket: " << _replacerSocket->errorString().toStdString() << std::endl;
    }
}
