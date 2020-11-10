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

#include "behaviour_goalkeeper.h"

#define GOAL_OFFSET 0.01f

#define BALLPREVISION_MINVELOCITY 0.01f
#define BALLPREVISION_VELOCITY_FACTOR 3.0f
#define BALLPREVISION_FACTOR_LIMIT 0.15f

QString Behaviour_Goalkeeper::name() {
    return "Behaviour_Goalkeeper";
}

Behaviour_Goalkeeper::Behaviour_Goalkeeper() {
    _sk_goto = nullptr;
    _sk_intercept = nullptr;
    _sk_spin = nullptr;
}

void Behaviour_Goalkeeper::configure() {
    usesSkill(_sk_goto = new Skill_GoTo());
    usesSkill(_sk_intercept = new Skill_InterceptBall());
    usesSkill(_sk_spin = new Skill_Spin());

    // Setting initial skill
    setInitialSkill(_sk_goto);

    // Transitions:
    // Skill GoTo active
    addTransition(STATE_GOTO, _sk_intercept, _sk_goto);
    addTransition(STATE_GOTO, _sk_spin, _sk_goto);

    // Skill InterceptBall active
    addTransition(STATE_INTERCEPT, _sk_goto, _sk_intercept);
    addTransition(STATE_INTERCEPT, _sk_spin, _sk_intercept);

    // Skill Spin active
    addTransition(STATE_SPIN, _sk_goto, _sk_spin);
    addTransition(STATE_SPIN, _sk_intercept, _sk_spin);

    _state = STATE_GOTO;
}

void Behaviour_Goalkeeper::run() {
    float xPos, distFromBack = 0.07f;
    Position intercept1, intercept2;
    bool isComing = false;

    //defining interception points (send to skill intercept)
    if(loc()->ourSide().isLeft()){
        if(loc()->ballVelocity().y() <= 0) isComing = true;
        else isComing = false;

        Position r1(true, loc()->fieldMinX() + distFromBack, loc()->fieldMinY(), 0);
        Position r2(true, loc()->fieldMinX() + distFromBack, loc()->fieldMaxY(), 0);
        if(loc()->ball().y() < 0){
            intercept1 = WR::Utils::hasInterceptionSegments(r1, r2, loc()->ourGoalLeftPost(), loc()->ball());
            if(intercept1.isValid()){
                //intercept1 = WR::Utils::threePoints(loc()->ourGoalLeftPost(), loc()->ball(), 0, 0);
                //intercept1.setPosition(intercept1.x() + distFromBack, intercept1.y() - marginFromPost, 0);
                intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);
            }else{
                intercept1 = loc()->ourGoalLeftPost();
                intercept2 = loc()->ourGoalRightPost();
            }
        }else{
            intercept1 = WR::Utils::hasInterceptionSegments(r1, r2, loc()->ourGoalRightPost(), loc()->ball());
            if(intercept1.isValid()){
                //intercept1 = WR::Utils::threePoints(loc()->ourGoalLeftPost(), loc()->ball(), 0, 0);
                //intercept1.setPosition(intercept1.x() + distFromBack, intercept1.y() - marginFromPost, 0);
                intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);
            }else{
                intercept1 = loc()->ourGoalLeftPost();
                intercept2 = loc()->ourGoalRightPost();
            }
            /*intercept1 = WR::Utils::threePoints(loc()->ourGoalRightPost(), loc()->ball(), 0, 0);
            intercept1.setPosition(intercept1.x() + distFromBack, intercept1.y() + marginFromPost, 0);
            intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);*/
        }
        //xPos = loc()->fieldMinX() + 0.05f;
    }else{
        if(loc()->ballVelocity().y() >= 0) isComing = true;
        else isComing = false;

        Position r1(true, loc()->fieldMaxX() - distFromBack, loc()->fieldMinY(), 0);
        Position r2(true, loc()->fieldMaxX() - distFromBack, loc()->fieldMaxY(), 0);
        if(loc()->ball().y() < 0){
            intercept1 = WR::Utils::hasInterceptionSegments(r1, r2, loc()->ourGoalRightPost(), loc()->ball());
            if(intercept1.isValid()){
                //intercept1 = WR::Utils::threePoints(loc()->ourGoalLeftPost(), loc()->ball(), 0, 0);
                //intercept1.setPosition(intercept1.x() + distFromBack, intercept1.y() - marginFromPost, 0);
                intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);
            }else{
                intercept1 = loc()->ourGoalLeftPost();
                intercept2 = loc()->ourGoalRightPost();
            }
            /*intercept1 = WR::Utils::threePoints(loc()->ourGoalRightPost(), loc()->ball(), 0, 0);
            intercept1.setPosition(intercept1.x() - distFromBack, intercept1.y() - marginFromPost, 0);
            intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);*/
        }else{
            intercept1 = WR::Utils::hasInterceptionSegments(r1, r2, loc()->ourGoalLeftPost(), loc()->ball());
            if(intercept1.isValid()){
                //intercept1 = WR::Utils::threePoints(loc()->ourGoalLeftPost(), loc()->ball(), 0, 0);
                //intercept1.setPosition(intercept1.x() + distFromBack, intercept1.y() - marginFromPost, 0);
                intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);
            }else{
                intercept1 = loc()->ourGoalLeftPost();
                intercept2 = loc()->ourGoalRightPost();
            }
            /*intercept1 = WR::Utils::threePoints(loc()->ourGoalLeftPost(), loc()->ball(), 0, 0);
            intercept1.setPosition(intercept1.x() - distFromBack, intercept1.y() + marginFromPost, 0);
            intercept2.setPosition(intercept1.x(), -1*intercept1.y(), 0);*/
        }
    }
    //limit Y value
    float yLim = 0.31f;
    if(intercept1.y() < -yLim){
        intercept1.setPosition(intercept1.x(), -yLim, intercept1.z());
        intercept2.setPosition(intercept2.x(), yLim, intercept2.z());
    }else if(intercept1.y() > yLim){
        intercept1.setPosition(intercept1.x(), yLim, intercept1.z());
        intercept2.setPosition(intercept2.x(), -yLim, intercept2.z());
    }

    if(!isComing && loc()->isInsideTheirField(loc()->ball()) ){
        intercept1.setPosition(intercept1.x(), 0.04f, intercept1.z());
        intercept2.setPosition(intercept2.x(), -0.04f, intercept2.z());
    }

    xPos = intercept1.x();

    //ball projection
    Position goalProjection;
    Position projectedBall = loc()->ball();
    goalProjection.setUnknown();
    //considering ball velocity to define player position
    if(loc()->ballVelocity().abs() > BALLPREVISION_MINVELOCITY){
        //calc unitary vector of velocity
        const Position velUni(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);

        //calc velocity factor
        float factor = BALLPREVISION_VELOCITY_FACTOR*loc()->ballVelocity().abs();
        WR::Utils::limitValue(&factor, 0.0f, BALLPREVISION_FACTOR_LIMIT);

        //calc projected position
        const Position delta(true, factor*velUni.x(), factor*velUni.y(), 0.0);
        Position projectedPos(true, loc()->ball().x()+delta.x(), loc()->ball().y()+delta.y(), 0.0);
        projectedBall = projectedPos;
    }else{
        projectedBall = loc()->ball();
    }
    float ballY = projectedBall.y();
    if(loc()->ourSide().isLeft()){
        WR::Utils::limitValue(&ballY, loc()->ourGoalLeftPost().y(), loc()->ourGoalRightPost().y());
    }else{
        WR::Utils::limitValue(&ballY, loc()->ourGoalRightPost().y(), loc()->ourGoalLeftPost().y());
    }
    Position followBallY(true, intercept1.x(), ballY, 0);


    //intercept velocity factor
    float maxDist = WR::Utils::distance(Position(true, loc()->fieldMaxX(), loc()->fieldMaxY(), 0), Position(true, loc()->fieldMinX(), 0, 0));
    float fac;
    if(maxDist < 0.2f) fac = 1.0f;
    else fac = 1 - (WR::Utils::distance(loc()->ball(), loc()->ourGoal())/maxDist);
    float velFacIntercept = 2.0f + 2.0f*fac;

    //transitions
    if (player()->distBall() <= 0.1f) {
        bool spinDirection = setSpinDirection();
        _sk_spin->setClockWise(spinDirection);
        enableTransition(STATE_SPIN);
    }else if(isBallComingToGoal()){
        _sk_intercept->selectVelocityNeeded(false);
        _sk_intercept->setInterceptSegment(intercept1, intercept2);
        _sk_intercept->setVelocityFactor(velFacIntercept);
        enableTransition(STATE_INTERCEPT);
    }else{
        _sk_goto->setGoToPos(followBallY);
        _sk_goto->setGoToVelocityFactor(2.0f);
        _sk_goto->setMinVelocity(0.7f);
        enableTransition(STATE_GOTO);
    }
}

bool Behaviour_Goalkeeper::isBallComingToGoal(){
    if(loc()->ourSide().isLeft()){
        if(loc()->ballVelocity().x() < 0){
            return true;
        }else{
            return false;
        }
    }else{
        if(loc()->ballVelocity().x() > 0){
            return true;
        }else{
            return false;
        }
    }
}

bool Behaviour_Goalkeeper::setSpinDirection() {
    // true if clockwise, false otherwise
    //caso original
    /*if ((loc()->distBallOurRightPost() < loc()->distBallOurLeftPost()))
        return true;
    else
        return false;*/

    //problema: se a bola tiver em baixo do jogador mas perto do poste esquerdo,ele gira antihorario,q faz gol contra  , e o caso inverso tb
    //nessa implementaçao ele entra quase sempre no caso normal mas em casos de bola cruzada , inverte a rotaçao para devolver pro campo
    if(loc()->ourSide().isRight()){
        if(loc()->ball().y() >= player()->position().y()){
            return false;
        }
        else{
            return true;
        }
    } else {
        if(loc()->ball().y() >= player()->position().y()){
            return true;
        }
        else{
            return false;
        }
    }
}
