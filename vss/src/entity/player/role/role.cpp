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

#include <src/entity/player/behaviour/behaviour.h>
#include "role.h"

#define COLLISION_TIME 500 //ms

Role::Role() {
    _behaviour = NULL;
    _player = NULL;
    _playerAccess = NULL;
    _loc = NULL;
    _initialized = false;
    _configureEnabled = true; // for set behaviours
    _actualBehaviour = -1;
    _wall = false;
    _isGK = false;
}

Role::~Role() {
    // Delete behaviours
     QHash<int, Behaviour*>:: iterator it;
     for(it = _behaviourList.begin(); it != _behaviourList.end(); it++){
         if((*it) == NULL)
             continue;
         else
             delete *it;
     }
     _behaviourList.clear();
}

Locations* Role::loc() {
    return _ourTeam->loc();
}

VSSReferee* Role::ref() {
    return _referee;
}

void Role::initialize(VSSTeam *ourTeam, VSSTeam *theirTeam, Locations *loc, VSSReferee *referee){
    _ourTeam = ourTeam;
    _theirTeam = theirTeam;
    _loc = loc;
    _referee = referee;

    // Configure Role
    _configureEnabled = true;
    initializeBehaviours();
    configure();
    _configureEnabled = false;

    //Initializa Global Behaviours
   _bh_gb = new Behaviour_GoBack();
   _bh_dn = new Behaviour_DoNothing();
   _retreated = true;

    // Initialize
    _initialized = true;

    canGoBack = false;
    _goBack = false;
}

void Role::setPlayer(VSSPlayer *player, PlayerAccess *playerAccess){
    _player = player;
    _playerAccess = playerAccess;
}

void Role::runRole(){
    if(_behaviourList.size() == 0){
        std::cout << "[ERROR] " << name().toStdString() << " has no behaviours set!\n";
        return ;
    }

    if(!canGoBack){
        timer.stop();
        if(timer.timesec() > 3){
            canGoBack = true;
        }
    }
    if(!ref()->isGameOn()){
        if(_bh_dn->isInitialized() == false)
            _bh_dn->initialize(_loc);

        _bh_dn->setPlayer(_player, _playerAccess);
        _bh_dn->runBehaviour();
    }
    else{
    if((!canMove() || !_retreated) && _goBack){
            if(_bh_gb->isInitialized() == false)
                _bh_gb->initialize(_loc);

            // Configure
            if(_wall) _bh_gb->setWall(true);
            else _bh_gb->setWall(false);
            _bh_gb->setPlayer(_player, _playerAccess);
            _bh_gb->runBehaviour();

        if(_retreated && canGoBack){
            canGoBack = false;
            timer.start();
            _bh_gb->setStart(true);
        }
        _retreated = _bh_gb->getDone();

    } else {
        // Run role (child)
        run();

            // Run Behaviour
            if(_behaviour->isInitialized() == false){
                _behaviour->initialize(_loc);
            }
            _behaviour->setPlayer(_player, _playerAccess);
            _behaviour->runBehaviour();
        }
    }
}

bool Role::canMove(){

    //Definindo cantos do campo
    Position topRightCorner(true,0.74f,0.64f,0.0f);
    Position bottomRightCorner(true,0.74f,-0.64f,0.0);
    Position topLeftCorner(true,-0.74f,0.64f,0.0f);
    Position bottomLeftCorner(true,-0.74f,-0.64f,0.0);

    //Lista com as 4 posições dos cantos do campo - Sentido horário
    QList<Position> cornersPosition;
    cornersPosition.append(topRightCorner);
    cornersPosition.append(bottomRightCorner);
    cornersPosition.append(bottomLeftCorner);
    cornersPosition.append(topLeftCorner);

    //Calcula a distancia do robo aos cantos do campo
    QList<float> distToCorners;
    for(int i = 0; i < 4; i++){
        distToCorners.append(WR::Utils::distance(player()->position(), cornersPosition[i]));
    }

    //Avalia a proximidade dos cantos do campo
    for(int i = 0; i < 4; i++){
        if(distToCorners[i] < 0.12f && (player()->velocity().x() < 0.1 && player()->velocity().y() < 0.1f)){
            _wall = true;
            return false;
        }else _wall = false;
    }

    //Avalia a proximidade com as paredes paralelas ao eixo x
    if(abs(player()->position().y()) > 0.6f){
        if (abs(player()->orientation().value() - 3 * GEARSystem::Angle::pi / 2) < 0.18f ||
                abs(player()->orientation().value() - GEARSystem::Angle::pi / 2) < 0.18f) {
            _wall = true;
            return false;
        } else _wall = false;
    }
    //Avalia a proximidade com as paredes paralelas ao eixo y
    if(abs(player()->position().x()) > 0.7f){
        if (player()->position().y() >= -0.2f && player()->position().y() <= 0.2f && _isGK) return true;
        if (abs(player()->orientation().value() - GEARSystem::Angle::pi) < 0.18f ||
                abs(player()->orientation().value()) < 0.18f) {
            _wall = true;
            return false;
        } else _wall = false;
    }
    /*
    //Avalia a proximidade com os jogadores adversarios
    for(quint8 i = 0; i < _ourTeam->opTeam()->avPlayersSize(); i++){
        Position obstPos = PlayerBus::theirPlayer(i)->position();
        float dist = WR::Utils::distance(player()->position(), obstPos);

        if(dist < 0.105f && (player()->velocity().x() < 0.1 && player()->velocity().y() < 0.1f) && !_isGK){
            return false;
        }
    }
    */
    return true;
}

QHash<int, Behaviour*> Role::getBehaviours(){
    return _behaviourList;
}

void Role::usesBehaviour(int id, Behaviour *behaviour){
    if(_configureEnabled == false){
        std::cout << "[WARNING] Blocked '" << name().toStdString() << "' setting Behaviour to use outside configure().\n";
        return ;
    }

    if(_behaviourList.contains(id) == false){
        // Add to list
        _behaviourList.insertMulti(id, behaviour);

        // If don't have initial behaviour, put it as initial behaviour
        if(_behaviour == NULL){
            _behaviour = behaviour;
            _actualBehaviour = id;
        }
    }else{
        std::cout << "[WARNING] Blocked '" << name().toStdString() << "' adding behaviours with same id!\n";
    }

}

int Role::getActualBehaviour(){
    return _actualBehaviour;
}

void Role::setBehaviour(int behaviour_id){
    if(_behaviourList.contains(behaviour_id)){
        _behaviour = _behaviourList.value(behaviour_id);
        _actualBehaviour = behaviour_id;
    }else{
        std::cout << "[ERROR] " << name().toStdString() << " setting behaviour with id '" << behaviour_id << "' that isn't at behaviours list.\n";
    }
}

PlayerAccess* Role::player(){
    if(_playerAccess == NULL){
        std::cout << "[ERROR] " << name().toStdString() << ", requesting player(), playerAccess not set!\n";
    }
    return _playerAccess;
}
