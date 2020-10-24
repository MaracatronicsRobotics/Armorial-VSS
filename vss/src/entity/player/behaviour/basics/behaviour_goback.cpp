#include "behaviour_goback.h"
#include <iostream>

#define distToBack 0.10f

QString Behaviour_GoBack::name() {
    return "Behaviour_GoBack";
}

Behaviour_GoBack::Behaviour_GoBack(){

}

void Behaviour_GoBack::configure() {

    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());

    //initial state
    _state = STATE_GOBACK;
    _start = true;

    //initial skill
    setInitialSkill(_sk_goTo);

    //transitions
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    _retreated = true;

};

void Behaviour_GoBack::run() {
    _playerPos = PlayerBus::ourPlayer(player()->playerId())->position();
    //até aqui foi

    //Find New Position
    if(_start){
        _newPos = newPosBack(_playerPos);
        _start = false;
        _retreated = false;
        _state = STATE_GOBACK;
    }

    switch (_state) {
    case STATE_GOBACK:{
        enableTransition(STATE_GOTO);

        _sk_goTo->setGoToPos(_newPos);

        if(abs(_playerPos.x() - _newPos.x()) < 0.01f && abs(_playerPos.y() - _newPos.y()) < 0.01f){
            _state = STATE_LOOKAT;
            _angleBefore = player()->orientation().value();
        }
        break;
    }
    case STATE_LOOKAT:{

        //Rotate
        _angleTarget = _angleBefore + 15 * (GEARSystem::Angle::pi / 180); //15 graus
        _sk_rotateTo->setDesiredAngle(_angleTarget);

        //_sk_rotateTo->setDesiredPosition(loc()->ball());
        enableTransition(STATE_ROTATE);

        if(player()->orientation().value() > 0.9f *_angleTarget){
            _retreated = true;
            _wall = false;
        }
        break;
    }
    }

}

Position Behaviour_GoBack:: newPosBack(Position playerPos){

    Position obsPos;
    float dist;

    if(_wall){
        Position centre(true, 0.0, 0.0, 0.0);
        dist = WR::Utils::distance(playerPos, centre);

        Position direction(true, - playerPos.x()/dist, - playerPos.y()/dist, 0.0);

        //New Point
        Position newP(true, playerPos.x() + distToBack * direction.x(), playerPos.y() + distToBack * direction.y(), 0.0);

        return newP;
    } else {
        for(quint8 i = 0; i < 3; i++){
            obsPos = PlayerBus::theirPlayer(i)->position();
            dist = WR::Utils::distance(playerPos, obsPos);

            if(dist < 0.105f){
                break;
            }
        }

        Position direction(true, (obsPos.x() - playerPos.x())/dist, (obsPos.y() - playerPos.y())/dist, 0.0);

        //New Point
        Position newP(true, playerPos.x() - distToBack * direction.x(), playerPos.y() - distToBack * direction.y(), 0.0);

        return newP;
    }
}

