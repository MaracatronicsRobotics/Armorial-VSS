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

#include "playersdistribution.h"
#include <src/utils/utils.h>
#include <src/entity/controlmodule/vssteam.h>
#include <src/entity/player/vssplayer.h>
#include <src/entity/locations.h>

PlayersDistribution::PlayersDistribution(VSSTeam *team, qint8 *kickerId) {
    _team = team;
    _kickerId = kickerId;
}

quint8 PlayersDistribution::getPlayer() {
    return getOneKNN(Position(false, 0, 0, 0));
}

quint8 PlayersDistribution::getOneKNN(const Position &pos) {
    return _players.empty()? DIST_INVALID_ID : getKNN(1, pos).first();
}

QList<quint8> PlayersDistribution::getPlayers(int n) {
    // Empty
    if(_players.empty())
        return QList<quint8>();
    // Create list of random players
    QList<quint8> retn;
    int size = _players.size();
    for(int i=0; i<(n>size? size:n); i++)
        retn.push_back(getPlayer());
    return retn;
}

QList<quint8> PlayersDistribution::getAllPlayers() {
    // Empty
    if(_players.empty())
        return QList<quint8>();
    // Return all players
    QList<quint8> retn;
    int size = _players.size();
    for(int i=0; i<size; i++)
        retn.push_back(getPlayer());
    return retn;
}

QList<quint8> PlayersDistribution::getKNN(int k, const Position &pos) {
    // Empty
    if(k==0 ||_players.empty())
        return QList<quint8>();
    // Sort
    sortByDistanceTo(pos);
    // Return 'k' players
    int available = _players.size();
    QList<quint8> retn;
    for(int i=0; i<(k>available?available:k); i++)
        retn.push_back(_players.takeFirst().id);
    return retn;
}

QList<quint8> PlayersDistribution::getKNNandY(int k, const Position &pos) {
    // Empty
    if(k==0 ||_players.empty())
        return QList<quint8>();
    // Sort
    sortByDistanceTo(pos);
    sortByY(k);
    // Return 'k' players
    int available = _players.size();
    QList<quint8> retn;
    for(int i=0; i<(k>available?available:k); i++)
        retn.push_back(_players.takeFirst().id);
    return retn;
}

QList<quint8> PlayersDistribution::getAllKNN(const Position &pos) {
    return getKNN(_players.size(), pos);
}

QList<quint8> PlayersDistribution::getAllY() {
    return getKNNandY(_players.size(), Position(false, 0.0, 0.0, 0.0));
}

QList<quint8> PlayersDistribution::getAllKNNandY(const Position &pos) {
    return getKNNandY(_players.size(), pos);
}

void PlayersDistribution::removePlayer(quint8 id) {
    remove(id);
}

bool PlayersDistribution::contains(quint8 id) const {
    for(int i=0; i<_players.size(); i++) {
        if(_players.at(i).id==id)
            return true;
    }
    return false;
}

int PlayersDistribution::playersAvailable() const {
    return _players.size();
}

bool PlayersDistribution::hasPlayersAvailable() const {
    return (_players.empty()==false);
}

void PlayersDistribution::clear() {
    _players.clear();
}

void PlayersDistribution::insert(quint8 id) {
    playerInfo ins;
    ins.id = id;
    _players.push_back(ins);
}

void PlayersDistribution::remove(quint8 id) {
    QList<playerInfo>::iterator it;
    for(it=_players.begin(); it!=_players.end(); it++) {
        playerInfo info = *it;
        if(info.id==id) {
            _players.erase(it);
            break;
        }
    }
}

void PlayersDistribution::print() {
    std::cout << "Players distribution:";
    for(QList<playerInfo>::iterator it=_players.begin(); it!=_players.end(); it++)
        std::cout << " " << (int)(*it).id;
    std::cout << "\n";
}

void PlayersDistribution::updateDistancesTo(const Position &pos) {
    if(pos.isUnknown())
        return;
    // Update distances
    for(int i=0; i<_players.size(); i++) {
        playerInfo player = _players.at(i);
        Position playerPos;
        if(player.id!=DIST_INVALID_ID)
            playerPos = _team->avPlayers().value(player.id)->position();
        if(playerPos.isUnknown() || player.id==DIST_INVALID_ID)
            player.distance = 999;
        else
            player.distance = WR::Utils::distance(pos, playerPos);
        _players.replace(i, player);
    }
}

void PlayersDistribution::updateDistancesToY() {
    for(int i=0; i<_players.size(); i++) {
        playerInfo player = _players.at(i);
        Position playerPos;
        if(player.id!=DIST_INVALID_ID)
            playerPos = _team->avPlayers().value(player.id)->position();
        if(playerPos.isUnknown() || player.id==DIST_INVALID_ID)
            player.distance = 999;
        else
            player.distance = playerPos.y();
        _players.replace(i, player);
    }
}

void PlayersDistribution::sortByDistanceTo(const Position &pos) {
    const int N = _players.size();
    // Check players list
    if(N==0)
        return;
    // Update distances
    updateDistancesTo(pos);
    // Quick-sort
    quickSort(0, N-1);
}

void PlayersDistribution::sortByY(int numToSort) {
    const int N = _players.size();
    // Check players list
    if(N==0)
        return;
    // Sort only 'numToSort' players
    int NTosort = (numToSort==0)? N : numToSort;
    if(NTosort>N)
        NTosort = N;
    // Update distances to Y
    updateDistancesToY();
    // Quick-sort
    quickSort(0, NTosort-1);
}

float PlayersDistribution::medianValue(float a, float b, float c) {
    if ((a <= b && b <= c) || (c <= b && b <= a))
            return b;
    if ((b <= a && a <= c) || (c <= a && a <= b))
            return a;
    if ((a <= c && c <= b) || (b <= c && c <= a))
            return c;
    else
        return -1;
}

void PlayersDistribution::quickSort(int ini, int end) {
    int i = ini, j = end, mid;
    float pivo;
    mid = ini + (end - ini)/2;
    pivo = medianValue(_players.at(i).distance, _players.at(j).distance, _players.at(mid).distance);
    while (i < j) {
        while (_players.at(i).distance < pivo && i <= end)
            i++;
        while (_players.at(j).distance > pivo && j >= 0)
            j--;
        if (i <= j) {
            _players.swap(i, j);
            i++;
            j--;
        }
    }
    if (j > ini)
        quickSort (ini, j);
    if (i < end)
        quickSort (i, end);
}
