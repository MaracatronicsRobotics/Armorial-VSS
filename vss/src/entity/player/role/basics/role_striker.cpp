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
#include "role_striker.h"

QString Role_Striker::name(){
    return "Role_Striker";
}
Role_Striker::Role_Striker()
{
    _bh_at = nullptr;
}
void Role_Striker::initializeBehaviours(){
    usesBehaviour(BHV_AT, _bh_at = new Behaviour_Attacker());
}
void Role_Striker::configure(){

}

void Role_Striker::run(){
    setBehaviour(BHV_AT);
}

void Role_Striker::receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor){
    if(isInitialized())
        emit emitPosition(player()->playerId(), Position(true, 0.0, 0.0, 0.0), Angle(true, 0.0));
}
