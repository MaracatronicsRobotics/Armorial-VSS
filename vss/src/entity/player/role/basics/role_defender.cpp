#include "role_defender.h"

QString Role_Defender::name(){
    return "Role_Defender";
}

Role_Defender::Role_Defender() {
    _bh_sb = nullptr;
    _bh_as = nullptr;
    _bh_br = nullptr;
}

void Role_Defender::initializeBehaviours(){
    usesBehaviour(BHV_STAYBACK, _bh_sb = new Behaviour_StayBack());
    usesBehaviour(BHV_ASSISTANT, _bh_as = new Behaviour_Assistant());
    usesBehaviour(BHV_BARRIER, _bh_br = new Behaviour_Barrier());
}

void Role_Defender::configure(){
    setBehaviour(BHV_STAYBACK); //initial behaviour
}

void Role_Defender::run(){
    bool theyAreNearBall = false;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(PlayerBus::theirPlayer(x)->hasBallPossession() || PlayerBus::theirPlayer(x)->isNearbyPosition(loc()->ball(), player()->distBall())){
                theyAreNearBall = true;
                break;
            }
        }
    }
    //if ball is closer to their goal and it isn't with this player (nor very close to it): stay back
    if(loc()->isInsideTheirField(loc()->ball()) && fabs(loc()->ball().x()) >= 0.2f*loc()->fieldMaxX() && !player()->isNearbyPosition(loc()->ball(), 0.07f)){
        setBehaviour(BHV_STAYBACK);
    }else{
        //if ball is distant from their goal and there's an opponent closer to it: barrier
        if(theyAreNearBall){
            setBehaviour(BHV_BARRIER);
        }
        //if ball is distant from their goal and there isn't any opponent closer to it: assistant (go to ball)
        else{
            setBehaviour(BHV_ASSISTANT);
        }
    }
}
