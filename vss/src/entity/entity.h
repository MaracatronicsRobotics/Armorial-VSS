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

#ifndef ENTITY_HH
#define ENTITY_HH

#include <QThread>
#include <QMutex>

class Entity : public QThread {
public:
    // Enumerating all entities (for control in worldmap)
    enum EntityType {ENT_WORLD, ENT_PLAYER, ENT_CONTROLMODULE, ENT_SENSOR, ENT_GUI};

    // Constructor
    Entity(EntityType type);

    // Getters
    EntityType entityType() const { return _type; }
    int entityId()          const { return _id; }
    int entityPriority();
    float loopTime();
    bool isEnabled();

    // Utils functions
    void setEntityPriority(int p);
    void setLoopTime(float ms);
    void setLoopFrequency(int hz);
    void enable(bool value);
    void stopEntity();

    // Entity name (class name)
    virtual QString name() = 0;
private:
    // Util functions
    void run();
    bool isRunning();

    // Virtual methods to be implemented
    virtual void initialization() = 0;
    virtual void loop() = 0;
    virtual void finalization() = 0;

    // Private vars
    volatile int _id;
    volatile EntityType _type;
    static int _idcont;
    bool _running;
    bool _enabled;
    int _priority;
    float _loopTime;

    // Mutexes
    QMutex _mutexRunning;
    QMutex _mutexEnabled;
    QMutex _mutexPriority;
    QMutex _mutexLoopTime;
};
#endif // ENTITY_HH
