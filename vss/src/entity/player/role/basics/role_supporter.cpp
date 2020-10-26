#include "role_supporter.h"

QString Role_Supporter::name(){
    return "Role_Supporter";
}
Role_Supporter::Role_Supporter()
{
    _bh_br = nullptr;
    _bh_as = nullptr;

}
void Role_Supporter::initializeBehaviours(){
    usesBehaviour(BHV_ASSISTANT, _bh_as = new Behaviour_Assistant());
    usesBehaviour(BHV_BARRIER , _bh_br = new Behaviour_Barrier());
}
void Role_Supporter::configure(){
    if(_positioning == BARRIER_PREDOMINANT){
        setBehaviour(BHV_BARRIER); //initial behaviour
        _bhv = BHV_BARRIER;
    }else if(_positioning == ASSIST_PREDOMINANT){
        setBehaviour(BHV_ASSISTANT);
        _bhv = BHV_ASSISTANT;
    }else{
        setBehaviour(BHV_BARRIER);
        _bhv = BHV_BARRIER;
    }
}
void Role_Supporter::run(){
    switch(_positioning){
    case(FREE):{
        float enemyInOurFieldFactor = 0;
        if(EnemyInOurField()) enemyInOurFieldFactor = 0.75f;
        float distBallFactor = 1.0f - loc()->distBallOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
        float distPlayerFactor = 1.0f - player()->distOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
        float ballVelFactor = loc()->ballVelocity().abs()/1.2f;

        float avgFactor = (distBallFactor + distPlayerFactor + ballVelFactor + enemyInOurFieldFactor)/4.0f;
        if(avgFactor > 0.5f && _bhv == BHV_ASSISTANT){
            setBehaviour(BHV_BARRIER);
            _bhv = BHV_BARRIER;
        }else if(avgFactor <= 0.5f && _bhv == BHV_BARRIER){
            setBehaviour(BHV_ASSISTANT);
            _bhv = BHV_ASSISTANT;
        }
        break;
    }
    case(BARRIER_PREDOMINANT):{
        if(_bhv == BHV_BARRIER){
            if(!EnemyInOurField() && player()->isNearbyPosition(loc()->ball(), 0.1f) && player()->isNearbyPosition(loc()->ourGoal(), 0.4f) && !BySideOfGoal() && (player()->distOurGoal() < WR::Utils::distance(loc()->ball(), loc()->ourGoal()))){
                setBehaviour(BHV_ASSISTANT);
                _bhv = BHV_ASSISTANT;
            }
        }else if(_bhv == BHV_ASSISTANT){
            if(EnemyInOurField() || !player()->isNearbyPosition(loc()->ball(), 0.25f)){
                setBehaviour(BHV_BARRIER);
                _bhv = BHV_BARRIER;
            }
        }
        break;
    }
    case(ASSIST_PREDOMINANT):{
        setBehaviour(BHV_ASSISTANT);
        break;
    }
    }
}
bool Role_Supporter::EnemyInOurField(){
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(loc()->isInsideOurField(PlayerBus::theirPlayer(x)->position()) && PlayerBus::theirPlayer(x)->isNearbyPosition(loc()->ball(), 0.2f)){
                return true;
            }
        }
    }
    return false;
}

bool Role_Supporter::BySideOfGoal(){
    if(fabs(loc()->ball().y()) > (fabs(loc()->ourGoal().y()) + loc()->fieldDefenseLength()/2.0f)){
        return true;
    }else{
        return false;
    }
}
