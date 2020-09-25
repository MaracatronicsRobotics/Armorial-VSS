#include "vssgui.h"

bool VSSGui::_isEnabled = false;
MainWindow* VSSGui::_mainWindow = nullptr;

QString VSSGui::name(){
    return "VSSGui";
}

VSSGui::VSSGui(VSSTeam *ourTeam, VSSTeam *theirTeam) : Entity(ENT_GUI)
{
    _mainWindow = new MainWindow();
    _mainWindow->show();

    _ourTeam = ourTeam;
    _theirTeam = theirTeam;

    _timer.start();
    timeToUpdate = (1000.0/VSSConstants::threadFrequency()) / 4.0;
    _isEnabled = true;
}

void VSSGui::initialization(){

}

void VSSGui::loop(){
    _mainWindow->updateDetection(_ourTeam, _theirTeam);

    _timer.stop();
    if(_timer.timemsec() >= timeToUpdate){
        // Process players avaliability
        QHash<quint8, VSSPlayer*> ourPlayers = _ourTeam->avPlayers();
        for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
            bool status = PlayerBus::ourPlayerAvailable(x);
            _mainWindow->setPlayerStatus(x, status);
        }

        // start timer for the next it
        _timer.start();
    }
}

void VSSGui::updateTree(StrategyState *strat){
    // Process coach strategy, playbooks, roles and players
    QList<QString> playbookList;
    QMap<QString, QList<QString>> rolesList;
    QMap<std::pair<QString, QString>, QList<std::pair<QString, quint8>>> playersList;
    QMap<QString, QString> behavioursList;

    // Parsing strategy, playbook, roles, players and its actual behaviours
    if(strat != NULL){
        QString stratName = strat->name();
        QList<Playbook*> pbList = strat->getPlaybooks();

        QList<Playbook*>::iterator it;
        QList<Role*>::iterator it2;
        for(it = pbList.begin(); it != pbList.end(); it++){
            if((*it) == NULL) continue;
            if(!(*it)->isInitialized()) continue;
            QString playbookName = (*it)->name();
            playbookList.push_back(playbookName);
            QList<Role*> rList = (*it)->getRoles();
            for(it2 = rList.begin(); it2 != rList.end(); it2++){
                if((*it2) == NULL) continue;
                if(!(*it2)->isInitialized()) continue;
                QString roleName = (*it2)->name();
                rolesList[playbookName].push_back(roleName);
                if((*it2)->player() == NULL) continue;
                quint8 playerId = (*it2)->player()->playerId();
                _mainWindow->setPlayerRole(playerId, roleName);
                std::string playerName = "Robot " + std::to_string(playerId);
                behavioursList[playerName.c_str()] = ((*it2)->getBehaviours()[(*it2)->getActualBehaviour()])->name();
                playersList[std::make_pair(playbookName, roleName)].push_back(std::make_pair(playerName.c_str(), playerId));
            }
        }
        _mainWindow->resetTree(stratName, playbookList, rolesList, playersList, behavioursList);
    }
}

void VSSGui::finalization(){

}
