#include "vssgui.h"

QString VSSGui::name(){
    return "VSSGui";
}

VSSGui::VSSGui(VSSTeam *ourTeam, VSSTeam *theirTeam) : Entity(ENT_GUI)
{
    _mainWindow = new MainWindow();
    _mainWindow->show();

    _ourTeam = ourTeam;
    _theirTeam = theirTeam;
}

void VSSGui::initialization(){

}

void VSSGui::loop(){
    _mainWindow->updateDetection(_ourTeam, _theirTeam);
}

void VSSGui::finalization(){

}
