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

#include "role_goalkeeper.h"

QString Role_Goalkeeper::name(){
    return "Role_Goalkeeper";
}

Role_Goalkeeper::Role_Goalkeeper() {
    _bh_gk = nullptr;
}

void Role_Goalkeeper::initializeBehaviours(){
    usesBehaviour(BHV_GK, _bh_gk = new Behaviour_Goalkeeper());
}

void Role_Goalkeeper::configure(){

}

void Role_Goalkeeper::run(){
    std::cout << "Irineu\n";
    setBehaviour(BHV_GK);
}
