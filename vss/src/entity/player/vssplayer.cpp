/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "vssplayer.h"
#include <GEARSystem/gearsystem.hh>
#include <src/entity/locations.h>
#include <src/entity/player/playeraccess.h>
#include <src/entity/controlmodule/vssteam.h>
#include <src/const/constants.h>
#include <src/entity/player/role/role.h>
#include <src/entity/player/navigation/navalgorithm.h>
#include <src/entity/player/navigation/navigation.h>

QString VSSPlayer::name(){
    QString teamColor = playerTeam()->teamColor() == Colors::BLUE ? "Blue" : "Yellow";
    return "Player " + QString::number(int(_playerId)) + " : Team " + teamColor;
}

VSSPlayer::VSSPlayer(quint8 playerId, VSSTeam *playerTeam, Controller *ctr, Role *defaultRole, PID *vwPID, NavigationAlgorithm *navAlg) : Entity(ENT_PLAYER)
{
    _playerId = playerId;
    _playerTeam = playerTeam;
    _ctr = ctr;
    _role = NULL;
    _defaultRole = defaultRole;
    _nav = new Navigation(this, navAlg, new VSSConstants());

    _vwPID = vwPID;

    _playerAccessSelf = new PlayerAccess(true, this, playerTeam->loc());
    _playerAccessBus = new PlayerAccess(false, this, playerTeam->loc());

    // Setting errors
    _lError = 0.015f;
    _aError = Angle::toRadians(4.0);

    // Idle control
    _idleCount = 0;
}

VSSPlayer::~VSSPlayer(){
    if(_playerAccessSelf != NULL)
        delete _playerAccessSelf;
    if(_playerAccessBus != NULL)
        delete _playerAccessBus;
}

/* Entity inherited methods */
void VSSPlayer::initialization(){
    QString teamColor = _playerTeam->teamColor() == Colors::BLUE ? "BLUE" : "YELLOW";
    printf("[TEAM %s PLAYER %d] Thread started.\n", teamColor.toStdString().c_str(), _playerId);
}

void VSSPlayer::loop(){
    if(position().isUnknown()){
        if(_idleCount < VSSConstants::threadFrequency()){
            _idleCount++;
            idle();
            setRole(NULL);
        }
    }
    else{
       _mutexRole.lock();
       if(_role != NULL){
           if(_role->isInitialized() == false){
               _role->initialize(_playerTeam, _playerTeam->opTeam(), _playerTeam->loc(), _playerTeam->ref());
           }
           _role->setPlayer(this, _playerAccessSelf);
           _role->runRole();
       }else if(_defaultRole != NULL){
           if(_defaultRole->isInitialized() == false){
               _defaultRole->initialize(_playerTeam, _playerTeam->opTeam(), _playerTeam->loc(), _playerTeam->ref());
           }
           _defaultRole->setPlayer(this, _playerAccessSelf);
           _defaultRole->runRole();
       }else{
           std::cout << "[ERROR] No role found for player #" << int(playerId()) << "!" << std::endl;
       }
       _mutexRole.unlock();
    }
}

void VSSPlayer::finalization(){
    printf("[TEAM %d PLAYER %d] Thread stopped.\n", _playerTeam->teamId(), _playerId);
}

/* Player control methods */
QString VSSPlayer::getRoleName() {
    _mutexRole.lock();
    QString roleName;
    if(_role == NULL)
        roleName = "UNKNOWN";
    else
        roleName = _role->name();
    _mutexRole.unlock();
    return roleName;
}

void VSSPlayer::setRole(Role* b) {
    // Check same behavior
    if(b==_role)
        return;
    // Set new
    _mutexRole.lock();
    _role = b;
    _mutexRole.unlock();
}

void VSSPlayer::idle(){
    /// TODO
    setSpeed(0.0, 0.0);
}

void VSSPlayer::setSpeed(float wl, float wr){
    if(_ctr != NULL){ // avoid to send packet for the other team
        _ctr->setSpeed(teamId(), playerId(), wl, wr, 0.0);
    }
}

float VSSPlayer::getRotateAngle(Position targetPosition){
    /*float robot_x, robot_y, angleOriginToRobot = angle().value();
    robot_x = position().x();
    robot_y = position().y();
    // Define a velocidade angular do robô para visualizar a bola
    float vectorRobotToTargetX = (targetPosition.x() - robot_x);
    float vectorRobotToTargetY = (targetPosition.y() - robot_y);
    float modVectorRobotToTarget = sqrtf(powf(vectorRobotToTargetX, 2) + powf(vectorRobotToTargetY, 2));
    if (modVectorRobotToTarget == 0.0f) return 0.0f;
    vectorRobotToTargetX = vectorRobotToTargetX / modVectorRobotToTarget;
    float angleOriginToTarget;   //Ângulo do robô entre o alvo e o eixo x do campo
    float angleRobotToTarget;       //Ângulo do robô entre o alvo e o eixo x do robô
    if(vectorRobotToTargetY < 0){ //terceiro e quarto quadrante
        angleOriginToTarget = float(2*M_PI) - acosf(vectorRobotToTargetX); //Ângulo do robô entre o alvo e o eixo x do campo
    }else{ //primeiro e segundo quadrante
        angleOriginToTarget = acosf(vectorRobotToTargetX); //Ângulo do robô entre o alvo e o eixo x do campo
    }
    angleRobotToTarget = angleOriginToTarget - angleOriginToRobot;
    if(angleRobotToTarget > float(M_PI)) angleRobotToTarget -= 2.0f * float(M_PI);
    if(angleRobotToTarget < float(-M_PI)) angleRobotToTarget += 2.0f * float(M_PI);
    return angleRobotToTarget;*/

    float rotateAngle = WR::Utils::getAngle(position(), targetPosition) - orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}

float VSSPlayer::getVxToTarget(Position targetPosition){
    // Salvando posicao do robo pra os calculos
    float robot_x = position().x(), robot_y = position().y();

    // Define a velocidade do robô para chegar na bola
    float dx = (targetPosition.x() - robot_x);
    float dy = (targetPosition.y() - robot_y);

    // Pegando modulo do vetor distancia
    float distanceMod = sqrtf(powf(dx, 2.0) + powf(dy, 2.0));

    return distanceMod;
}

float VSSPlayer::getRotateSpeed(float angleRobotToTarget){
    float ori = angle().value();
    if(ori > float(M_PI)) ori -= 2.0f * float(M_PI);
    if(ori < float(-M_PI)) ori += 2.0f * float(M_PI);

    bool swapSpeed = false;
    if(angleRobotToTarget > float(M_PI) / 2.0f){
        angleRobotToTarget -= float(M_PI);
        swapSpeed = true;
    }
    else if(angleRobotToTarget < float(-M_PI) / 2.0f){
        angleRobotToTarget += float(M_PI);
        swapSpeed = true;
    }

    if(swapSpeed){
        if(ori > float(M_PI) / 2.0f){
            ori -= float(M_PI);
        }
        else if(ori < float(-M_PI) / 2.0f){
            ori += float(M_PI);
        }
    }

    float angleToTarget = angleRobotToTarget + ori;
    float speed = _vwPID->calculate(angleToTarget, ori);

    return speed;
}

void VSSPlayer::rotateTo(Position targetPosition, bool rot, float angleTarget){
    float speed;
    if(rot){
        float angleDif = angleTarget - angle().value();
        speed = getRotateSpeed(angleDif);
    }
    else{
        float angleRobotToTarget = getRotateAngle(targetPosition);
        speed = getRotateSpeed(angleRobotToTarget);
    }

    // rotate (tem q tratar)
    // x = vx desejada
    // y = angular desejada

    // equações pra transformar em vr e vl
    /*
     * Onde:
     * L       = distancia entre as rodas
     * r       = raio da roda
     * VLinear = vx desejada
     * w       = velocidade angular desejada
     *
     * (2 * VLinear)/r = Wr + Wl
     * (w * L)/r       = Wr - Wl
     * Wr              = (2 * Vlin + w * L)/(2 * r)
     * Wl              = Wr - (w * L) / r
    */

    float L = 0.075f;
    float r = 0.0325f;
    float velLinear = 0.0f;
    float velAngular = speed;
    float wr = ((2.0f * velLinear) + (L * velAngular)) / (2.0f * r);
    float wl = wr - ((L * velAngular) / r);

    setSpeed(wl, wr);
}

std::pair<Angle,float> VSSPlayer::getNavDirectionDistance(const Position &destination, const Angle &positionToLook, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea) {
    _nav->setGoal(destination, positionToLook, avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);
    Angle direction = _nav->getDirection();
    float distance = _nav->getDistance();

    std::pair<Angle,float> movement = std::make_pair(direction, distance);
    movement.first.setValue(movement.first.value() - orientation().value());
    return movement;
}

void VSSPlayer::goTo(Position targetPosition, float velocityNeeded, float velocityFactor, float minVel, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea, bool pathActivated){
    // Taking orientation from path planning
    Angle anglePP;
    float help = getRotateAngle(targetPosition);

    if(targetPosition.isUnknown())
        anglePP = Angle(false, 0.0);
    else
        anglePP = Angle(true, help);

    std::pair<Angle, float> a = getNavDirectionDistance(targetPosition, anglePP, avoidTeammates, avoidOpponents, avoidBall, avoidOurGoalArea, avoidTheirGoalArea);

    // Verificando se o lado de trás pra movimentação é a melhor escolha
    help = a.first.value();
    if(help > float(M_PI)) help -= 2.0f * float(M_PI);
    if(help < float(-M_PI)) help += 2.0f * float(M_PI);

    bool swapSpeed = false;
    if(help > float(M_PI) / 2.0f){
        help -= float(M_PI);
        swapSpeed = true;
    }
    else if(help < float(-M_PI) / 2.0f){
        help += float(M_PI);
        swapSpeed = true;
    }

    float rotateSpeed = getRotateSpeed(help);
    float vel;

    //if (pathActivated) vel = a.second;
    //else vel = getVxToTarget(targetPosition);
    vel = getVxToTarget(targetPosition);

    // Adjusting to minVel if lower
    if(vel <= minVel){
        // Adjusting absolute value of velocity
        vel = minVel;
    }
    // Se escolheu o lado de trás, inverte o vx
    if(swapSpeed) vel *= -1;

    float dist = WR::Utils::distance(position(), targetPosition);

    /*
    //Vel_teste = 1.0f;
    if ( abs(help) > GEARSystem::Angle::toRadians(80)) {
        setSpeed(0.0, rotateSpeed);
    } else {
        if(dist <= 0.1f){ // se estiver a 10cm ou menos do alvo
            if(abs(help) >= GEARSystem::Angle::toRadians(15)){ // se a diferença for maior que 15 deg
                setSpeed(0.0f*k, rotateSpeed); // zera a linear e espera girar
            }else{
                setSpeed(Vel_teste * vel, rotateSpeed); // caso esteja de boa, gogo
            }
        }
        else if(dist > 0.1f && dist <= 0.5f){ // se estiver entre 10cm a 50cm do alvo
            if(abs(help) >= GEARSystem::Angle::toRadians(25)){ // se a diferença for maior que 25 deg
                setSpeed(Vel_teste * vel, Fr_factor*rotateSpeed); // linear * 0.3 e gira
            }else{
                setSpeed(Vel_teste * vel, rotateSpeed); // caso esteja de boa, gogo
            }
        }
        else if(dist > 0.5f){ // se estiver a mais de 50cm do alvo
            if(abs(help) >= GEARSystem::Angle::toRadians(35)){ // se a diferença for maior que 35 deg
                setSpeed(Vel_teste * vel, Fr_factor*rotateSpeed); // linear * 0.5 e gira
            }else{
                setSpeed(Vel_teste * vel, rotateSpeed); // caso esteja de boa, gogo
            }
        }
    }
    */

    // x = vx desejada
    // y = angular desejada

    // equações pra transformar em vr e vl
    /*
     * Onde:
     * L       = distancia entre as rodas
     * r       = raio da roda
     * VLinear = vx desejada
     * w       = velocidade angular desejada
     *
     * (2 * VLinear)/r = Wr + Wl
     * (w * L)/r       = Wr - Wl
     * Wr              = (2 * Vlin + w * L)/(2 * r)
     * Wl              = Wr - (w * L) / r
    */

    float L = 0.075f;
    float r = 0.0325f;
    float velLinear = velocityFactor * vel;
    float velAngular = rotateSpeed;
    float wr = ((2.0f * velLinear) + (L * velAngular)) / (2.0f * r);
    float wl = wr - ((L * velAngular) / r);

    setSpeed(wl, wr);
}

/* Player info methods */
PlayerAccess* VSSPlayer::access() const {
    return _playerAccessBus;
}

quint8 VSSPlayer::playerId() const{
    return _playerId;
}

VSSTeam* VSSPlayer::playerTeam(){
    return _playerTeam;
}

quint8 VSSPlayer::teamId() const{
    return _playerTeam->teamId();
}

quint8 VSSPlayer::opTeamId() const{
    return _playerTeam->teamId() ? 0 : 1;
}

bool VSSPlayer::hasBallPossession() const{
    return _playerTeam->wm()->ballPossession(_playerTeam->teamId(), _playerId);
}

Position VSSPlayer::position() const{
    return _playerTeam->wm()->playerPosition(_playerTeam->teamId(), _playerId);
}

Velocity VSSPlayer::velocity() const{
    return _playerTeam->wm()->playerVelocity(_playerTeam->teamId(), _playerId);
}

Angle VSSPlayer::angle() const{
    return _playerTeam->wm()->playerOrientation(_playerTeam->teamId(), _playerId);
}

AngularSpeed VSSPlayer::angularSpeed() const{
    return _playerTeam->wm()->playerAngularSpeed(_playerTeam->teamId(), _playerId);
}

bool VSSPlayer::isLookingTo(const Position &pos) const{
    return this->isLookingTo(pos, _aError);
}

bool VSSPlayer::isLookingTo(const Position &pos, float error) const{
    Angle angle1(true, WR::Utils::getAngle(position(), pos));

    // Calc diff
    float dif = abs(WR::Utils::angleDiff(angle(), angle1));
    return (dif <= error);
}

bool VSSPlayer::isAtPosition(const Position &pos) const{
    // Get distance
    const float distance = WR::Utils::distance(position(), pos);
    return (distance <= _lError);
}

bool VSSPlayer::isNearbyPosition(const Position &pos, float error) const{
    const float distance = WR::Utils::distance(position(), pos);

    return (distance <= error);
}

float VSSPlayer::distanceTo(const Position &pos) const{
    return WR::Utils::distance(position(), pos);
}

float VSSPlayer::distBall() const{
    return WR::Utils::distance(position(), _playerTeam->loc()->ball());
}

float VSSPlayer::distOurGoal() const{
    return WR::Utils::distance(position(), _playerTeam->loc()->ourGoal());
}

float VSSPlayer::distTheirGoal() const{
    return WR::Utils::distance(position(), _playerTeam->loc()->theirGoal());
}

Angle VSSPlayer::angleTo(const Position &pos) const{
    return Angle(true, WR::Utils::getAngle(position(), pos));
}

Angle VSSPlayer::orientation() const{
    return _playerTeam->wm()->playerOrientation(_playerTeam->teamId(), _playerId);
}

void VSSPlayer::setGoal(Position pos) const{
    _nav->setGoal(pos, orientation(), true, true, true, true, true);
}

QLinkedList<Position> VSSPlayer::getPath() const {
    return _nav->getPath();
}
