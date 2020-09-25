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

#include "playeraccess.h"
#include <src/entity/player/vssplayer.h>

PlayerAccess::PlayerAccess(bool self, VSSPlayer *player, Locations *loc) {
    _self = self;
    _player = player;
    _loc = loc;
}

PlayerAccess::~PlayerAccess() {

}

quint8 PlayerAccess::playerId() const {
    return _player->playerId();
}

quint8 PlayerAccess::teamId() const {
    return _player->teamId();
}

quint8 PlayerAccess::opTeamId() const {
    return _player->opTeamId();
}

VSSTeam* PlayerAccess::team() const {
    return _player->playerTeam();
}

Position PlayerAccess::position() const {
    return _player->position();
}

Angle PlayerAccess::orientation() const {
    return _player->angle();
}

Velocity PlayerAccess::velocity() const {
    return _player->velocity();
}

bool PlayerAccess::isAtPosition(const Position &position) const {
    return _player->isAtPosition(position);
}

bool PlayerAccess::isNearbyPosition(const Position &pos, float error) const {
    return _player->isNearbyPosition(pos, error);
}

bool PlayerAccess::isLookingTo(const Position &position) const {
    return _player->isLookingTo(position);
}

bool PlayerAccess::isLookingTo(const Position &pos, float error) const {
    return _player->isLookingTo(pos, error);
}

bool PlayerAccess::hasBallPossession() const {
    return _player->hasBallPossession();
}

float PlayerAccess::distanceTo(const Position &pos) const {
    return _player->distanceTo(pos);
}

float PlayerAccess::distBall() const {
    return _player->distBall();
}

float PlayerAccess::distOurGoal() const {
    return _player->distOurGoal();
}

float PlayerAccess::distTheirGoal() const {
    return _player->distTheirGoal();
}

Angle PlayerAccess::angleTo(const Position &pos) const {
    return _player->angleTo(pos);
}

QString PlayerAccess::roleName() const {
    return _player->getRoleName();
}
