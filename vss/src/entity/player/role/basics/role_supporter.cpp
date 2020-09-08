#include "role_supporter.h"

QString Role_Supporter::name(){
    return "Role_Supporter";
}
Role_Supporter::Role_Supporter()
{
    _bh_br = nullptr;
    _bh_as = nullptr;
    _bh_sb = nullptr;

}
void Role_Supporter::initializeBehaviours(){
    usesBehaviour(BHV_ASSISTANT, _bh_as = new Behaviour_Assistant());
    usesBehaviour(BHV_BARRIER , _bh_br = new Behaviour_Barrier());
    usesBehaviour(BHV_STAYBACK , _bh_sb = new Behaviour_StayBack());
}
void Role_Supporter::configure(){
    setBehaviour(BHV_ASSISTANT); //initial behaviour

    //Coordenadas da quina de Cima
    TopCorner_X = loc()->ourFieldTopCorner().x();
    TopCorner_Y = loc()->ourFieldTopCorner().y();
    //Coordenadas da quina de Baixo
     BottomCorner_X = loc()->ourFieldBottomCorner().x();
    BottomCorner_Y = loc()->ourFieldBottomCorner().y();
    //Coordenadas auxiliares da Trave esquerda
     Post_X = loc()->ourGoalLeftPost().x();
     Post_Y = loc()->ourGoalLeftPost().y();
    //distancia constante da quina pra trave

     dist_Corner_LeftPost = sqrt(pow((TopCorner_X - Post_X), 2) + pow((TopCorner_Y - Post_Y) , 2 )) ;

}
void Role_Supporter::run(){

    //Areas das quinas
    float distBallfromTopCorner = sqrt(pow((TopCorner_X - loc()->ball().x()), 2) + pow((TopCorner_Y - loc()->ball().y()) , 2 ));
    float distBallfromBottomCorner = sqrt(pow((BottomCorner_X - loc()->ball().x()), 2) + pow((BottomCorner_Y - loc()->ball().y()) , 2 ));

    if(distBallfromTopCorner <= dist_Corner_LeftPost || distBallfromBottomCorner <= dist_Corner_LeftPost){
        if(EnemyHasBallPossession()){
            setBehaviour(BHV_BARRIER);
            //std::cout << "BHV_BARRIER" << std::endl;
        }
        else{
            setBehaviour(BHV_ASSISTANT);
            //std::cout << "BHV_ASSISTANT" << std::endl;
        }

    }

    else{
        setBehaviour(BHV_ASSISTANT);
        //std::cout << "BHV_ASSISTANT" << std::endl;
    }
}
bool Role_Supporter::EnemyHasBallPossession(){
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(PlayerBus::theirPlayer(x)->hasBallPossession()){
                return true;
            }
        }
    }
    return false;
}
