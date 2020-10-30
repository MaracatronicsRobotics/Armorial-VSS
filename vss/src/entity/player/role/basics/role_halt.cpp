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

#include "role_halt.h"

QString Role_Halt::name(){
    return "Role_Halt";
}

Role_Halt::Role_Halt() {

}

void Role_Halt::initializeBehaviours(){
    usesBehaviour(BHV_DONOTHING, _bh_dn = new Behaviour_DoNothing());
    usesBehaviour(BHV_PUSHBALL, _bh_pb = new Behaviour_PushBall());
    usesBehaviour(BHV_GK, _bh_gk = new Behaviour_Goalkeeper());
    usesBehaviour(BHV_ASSISTANT, _bh_as = new Behaviour_Assistant());
    usesBehaviour(BHV_ATTACKER, _bh_at = new Behaviour_Attacker());
}

void Role_Halt::configure(){
    isGK(false);
}

void Role_Halt::run(){
    setBehaviour(BHV_ATTACKER);
    //setBehaviour(BHV_ASSISTANT);
    //setBehaviour(BHV_DONOTHING);
    //setBehaviour(BHV_PUSHBALL);
    //setBehaviour(BHV_GK);
}

void Role_Halt::receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor){
    if(isInitialized())
        emit emitPosition(player()->playerId(), Position(true, 0.0, 0.0, 0.0), Angle(true, 0.0));
}
