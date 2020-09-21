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
    setBehaviour(BHV_BARRIER); //initial behaviour
}
void Role_Supporter::run(){
    //distancia da bola para o nosso gol, distancia do nosso jogador para o gol, se tem um inimigo mais proximo dela
    if(EnemyHasBallPossession()){
    //if(true){
        float distBallFactor = 1.0f - loc()->distBallOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
        float distPlayerFactor = 1.0f - player()->distOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
        float ballVelFactor = loc()->ballVelocity().abs()/1.2f;
        //std::cout << distBallFactor <<" "<< distPlayerFactor<<" "<< ballVelFactor << std::endl;
        float avgFactor = (distBallFactor + distPlayerFactor + ballVelFactor)/3.0f;
        if(avgFactor > 0.5f){
            setBehaviour(BHV_BARRIER);
        }else{
            setBehaviour(BHV_ASSISTANT);
        }
    }else{
        setBehaviour(BHV_ASSISTANT);
    }
}
bool Role_Supporter::EnemyHasBallPossession(){
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(PlayerBus::theirPlayer(x)->isNearbyPosition(loc()->ball(), 0.09f)){
                return true;
            }
        }
    }
    return false;
}
