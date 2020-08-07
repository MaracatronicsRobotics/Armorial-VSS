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

#include "skill.h"

Skill::Skill() {
    _player = NULL;
    _loc = NULL;
    _initialized = false;
}

Skill::~Skill(){

}

void Skill::initialize(Locations *loc){
    _loc = loc;
    //_vssConstants = vssConstants;
    _initialized = true;
}

void Skill::setPlayer(VSSPlayer *player){
    _player = player;
}

void Skill::runSkill(){
    // Skill implemented by child of this class
    run();
}

/*VSSConstants *Skill::getConstants() {
    if(_vssConstants==NULL)
        std::cout << VSSConstants::red << "[ERROR] " << VSSConstants::reset << name().toStdString() << ", requesting getConstants(), _mrcconstants not initialized!\n";
    return _vssConstants;
}*/
