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

#include "strategystate.h"
#include <src/entity/controlmodule/coach/playersdistribution.h>
#include <src/entity/controlmodule/vssteam.h>
#include <src/entity/player/vssplayer.h>
#include <src/entity/locations.h>
#include <src/entity/controlmodule/playbook/playbook.h>
#include <src/gui/vssgui.h>

StrategyState::StrategyState() {
    _initialized = false;
    _lastNumOurPlayers = 0;
    _configureEnabled = false;
    _dist = NULL;
    _kickerId = NULL;
}

StrategyState::~StrategyState() {
    setCurrPlaybookToOld();
    clearOldPlaybook();
}

Locations* StrategyState::loc() const {
    return _ourTeam->loc();
}

void StrategyState::initialize(VSSTeam *ourTeam, VSSTeam *theirTeam, CoachUtils *utils, PlayersDistribution *dist, qint8 *kickerId) {
    _ourTeam = ourTeam;
    _theirTeam = theirTeam;
    _utils = utils;
    _dist = dist;
    _kickerId = kickerId;
    _initialized = true;
}

void StrategyState::runStrategyState() {
    // don't make changes when timeout or halt is enabled
    QList<Playbook*>::iterator it;
    // Run configurate(numOurPlayers) if num of players changed
    QHash<quint8,VSSPlayer*> avPlayers = _ourTeam->avPlayers();
    if(_lastNumOurPlayers != avPlayers.size()) {
        _lastNumOurPlayers = avPlayers.size();
        setCurrPlaybookToOld();
        _configureEnabled = true;
        configure(_lastNumOurPlayers);
        _configureEnabled = false;
    }

    // For each playbook, initialize (if necessary) and clear old players
    for(it = _playbookList.begin(); it != _playbookList.end(); it++) {
        Playbook *playbook = *it;
        // Initialize playbooks (initialized before run() because it modifies
        // players in playbook and already needs it with available players lists)
        if(playbook->isInitialized()==false)
            playbook->initialize(_ourTeam, _theirTeam, _utils, _kickerId);
        // Clear old players added
        playbook->clearPlayers();
    }

    // Reset players distribution
    _dist->clear();
    QList<VSSPlayer*> players = avPlayers.values();
    QList<VSSPlayer*>::iterator it2;
    for(it2=players.begin(); it2!=players.end(); it2++)
        _dist->insert((*it2)->playerId());

    // Run state implemented by child
    // It set players to playbooks created on configure()
    run(_lastNumOurPlayers);

    // Effectivelly run playbook
    for(it = _playbookList.begin(); it != _playbookList.end(); it++) {
        (*it)->runPlaybook(name());
    }

    // If still players on players distribution, set them to DoNothing
    while(_dist->hasPlayersAvailable()) {
        quint8 id = _dist->getPlayer();
        VSSPlayer* player = avPlayers.value(id);
        player->setRole(NULL);
        std::cout << "[WARNING] " << name().toStdString() << ", player #" << (int)id << " wasn't allocated in a Playbook!\n";
    }

    // Cleanup playbook old roles
    for(it = _playbookList.begin(); it != _playbookList.end(); it++) {
        if((*it)->numPlayers()!=0)
            (*it)->clearOldRoles();
    }

    // Clear old playbook
    clearOldPlaybook();

    if(VSSGui::_isEnabled)
        VSSGui::updateTree(this);

}

void StrategyState::setCurrPlaybookToOld() {
    while(!_playbookList.isEmpty())
        _oldPlaybook.push_back(_playbookList.takeFirst());
}

void StrategyState::clearOldPlaybook() {
    int sz = _oldPlaybook.size();
    for(int x = 0; x < sz; x++){
        if(_oldPlaybook.at(x) == NULL){
            continue;
        }
        else
            delete _oldPlaybook.at(x);
    }

    _oldPlaybook.clear();
}

void StrategyState::usesPlaybook(Playbook *playbook) {
    // Check if call is inside configure()
    if(_configureEnabled==false) {
        std::cout <<"[WARNING] Blocked strategy state '" << name().toStdString() << "' setting Playbook to use outside configure().\n";
        return;
    }
    // Check null pointer
    if(playbook==NULL)
        return;
    // Add playbook to list
    if(_playbookList.contains(playbook)==false)
        _playbookList.push_back(playbook);
}

Colors::Color StrategyState::teamColor(){
    return _ourTeam->teamColor();
}

