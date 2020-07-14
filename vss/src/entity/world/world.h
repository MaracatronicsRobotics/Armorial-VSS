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

#ifndef WORLD_HH
#define WORLD_HH

#include <QReadWriteLock>
#include <iostream>
#include <src/entity/entity.h>
#include <src/entity/world/worldmapupdater.h>
#include <src/const/constants.h>
#include <src/entity/controlmodule/controlmodule.h>

class World : public Entity {
public:
    World(Controller *ctr, Fields::Field *defaultField);
    ~World();
    QString name();

    // Management
    void setTeams(VSSTeam *ourTeam, VSSTeam *theirTeam);
    void setControlModule(ControlModule *ctrModule);

    // Entities management
    void addEntity(Entity *e, int priority);
    void removeEntity(int id);

    // WorldMap read-write synchronization
    void wmLockRead();
    void wmUnlock();

    // Team pointer returns
    VSSTeam* ourTeam() { return _ourTeam; }
    VSSTeam* theirTeam() { return _theirTeam; }

    // Controller/WorldMap access
    Controller* getController() const { return _ctr; }
    WorldMap* getWorldMap() const { return _wm; }
private:
    // Entity implementation
    void initialization();
    void loop();
    void finalization();

    // Controller access
    Controller *_ctr;

    // Teams access
    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;

    // WorldMap
    WorldMap *_wm;
    WorldMapUpdater *_wmUpdater;

    // Control module
    ControlModule *_ctrModule;
    QMutex _ctrModuleMutex;

    // WorldMap read-write synchronization
    QReadWriteLock _wmLock;
    void wmLockWrite();

    // Entities by priority
    QMap<int,QHash<int,Entity*>*> _priorityLevels; // Map<priority, Hash<id,Entity> >

    // Internal
    void setupWorldMap();
    void stopAndDeleteEntities();
};

#endif // WORLD_HH
