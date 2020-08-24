#ifndef VSSGUI_H
#define VSSGUI_H

#include <src/entity/entity.h>
#include <src/gui/mainwindow.h>

class VSSGui : public Entity
{
public:
    QString name();
    VSSGui(VSSTeam *ourTeam, VSSTeam *theirTeam);

private:
    void initialization();
    void loop();
    void finalization();

    MainWindow *_mainWindow;
    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;
};

#endif // VSSGUI_H
