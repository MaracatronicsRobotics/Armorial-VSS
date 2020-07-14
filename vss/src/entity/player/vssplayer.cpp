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
#include <src/utils/utils.h>
#include <src/entity/locations.h>
#include <src/entity/player/playeraccess.h>
#include <src/entity/controlmodule/vssteam.h>
#include <src/const/constants.h>
#include <src/entity/player/role/role.h>

QString VSSPlayer::name(){
    QString teamColor = playerTeam()->teamColor() == Colors::BLUE ? "Blue" : "Yellow";
    return "Player " + QString::number((int)_playerId) + " : Team " + teamColor;
}

VSSPlayer::VSSPlayer(quint8 playerId, VSSTeam *playerTeam, Controller *ctr, Role *defaultRole) : Entity(ENT_PLAYER)
{
    _playerId = playerId;
    _playerTeam = playerTeam;
    _ctr = ctr;
    _role = NULL;
    _defaultRole = defaultRole;

    _playerAccessSelf = new PlayerAccess(true, this, playerTeam->loc());
    _playerAccessBus = new PlayerAccess(false, this, playerTeam->loc());

    // Setting errors
    _lError = 0.015;
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
    printf("[TEAM %s PLAYER %d] Thread started.\n", teamColor.toStdString().c_str(), (int) _playerId);
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
               _role->initialize(_playerTeam, _playerTeam->opTeam(), _playerTeam->loc());
           }
           _role->setPlayer(this, _playerAccessSelf);
           _role->runRole();
       }else if(_defaultRole != NULL){
           if(_defaultRole->isInitialized() == false){
               _defaultRole->initialize(_playerTeam, _playerTeam->opTeam(), _playerTeam->loc());
           }
           _defaultRole->setPlayer(this, _playerAccessSelf);
           _defaultRole->runRole();
       }else{
           std::cout << "[ERROR] No role found for player #" << (int)playerId() << "!" << std::endl;
       }
       _mutexRole.unlock();
    }
}

void VSSPlayer::finalization(){
    printf("[TEAM %d PLAYER %d] Thread stopped.\n", (int) _playerTeam->teamId(), (int) _playerId);
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
    float dif = fabs(WR::Utils::angleDiff(angle(), angle1));
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
