#include "mainwindow.h"
#include "build/tmp/moc/ui_mainwindow.h"
#include <src/const/constants.h>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Role Tree
    root = NULL;
    treeWidget = new QTreeWidget(ui->treeTab);
    treeWidget->setObjectName(QStringLiteral("treeWidget"));
    treeWidget->setGeometry(QRect(0, 0, 441, 179));
    treeWidget->setColumnCount(1);
    QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
    ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Strategy", Q_NULLPTR));
    qRegisterMetaType<QVector<int>>("QVector<int>");

    /// Parsing widgets
    // Batteries
    playerBatteries.push_back(ui->battery_0);
    playerBatteries.push_back(ui->battery_1);
    playerBatteries.push_back(ui->battery_2);
    playerBatteries.push_back(ui->battery_3);
    playerBatteries.push_back(ui->battery_4);

    // Roles
    playerRoles.push_back(std::make_pair(ui->roleIcon_0, ui->role_0));
    playerRoles.push_back(std::make_pair(ui->roleIcon_1, ui->role_1));
    playerRoles.push_back(std::make_pair(ui->roleIcon_2, ui->role_2));
    playerRoles.push_back(std::make_pair(ui->roleIcon_3, ui->role_3));
    playerRoles.push_back(std::make_pair(ui->roleIcon_4, ui->role_4));

    // Groupboxes
    playerBoxes.push_back(ui->robot_0);
    playerBoxes.push_back(ui->robot_1);
    playerBoxes.push_back(ui->robot_2);
    playerBoxes.push_back(ui->robot_3);
    playerBoxes.push_back(ui->robot_4);

    // Pixmaps (arrumar isso aqui dps com as fotos dos robos)
    playerPixmaps.push_back(QPixmap(":/gui/robot.png"));
    playerPixmaps.push_back(QPixmap(":/gui/robot.png"));
    playerPixmaps.push_back(QPixmap(":/gui/robot.png"));
    playerPixmaps.push_back(QPixmap(":/gui/robot.png"));
    playerPixmaps.push_back(QPixmap(":/gui/robot.png"));

    // Dark pallete
    this->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    this->setPalette(darkPalette);

    // Reset robots
    resetRobots();

    // Creating first tree node
    addRoot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetRobots(){
    for(quint8 x = 0; x < 5; x++){
        setPlayerBattery(x, 0);
        setPlayerRole(x, "none");
        setPlayerStatus(x, false);
    }
}

void MainWindow::setPlayerStatus(int id, bool status){
    playerBoxes.at(id)->setDisabled(!status);
}

void MainWindow::updateDetection(VSSTeam *ourTeam, VSSTeam *theirTeam){
    ui->openGLWidget->updateDetection(ourTeam, theirTeam);
}

void MainWindow::setPlayerRole(quint8 id, QString role){
    QPixmap pixmp = getRolePixmap(role);

    if(role.toLower() == playerRoles.at(id).second->text().toLower()) return;

    playerRoles.at(id).first->setPixmap(pixmp);
    playerRoles.at(id).second->setText(role);
}

QPixmap MainWindow::getRolePixmap(QString role){
    QPixmap pixmp;

    if(role.toLower() == "role_goalkeeper"){ // teste
        pixmp.load(":/gui/gk.png");
    }else if(role.toLower() == "role_barrier"){
        pixmp.load(":/gui/bar.png");
    }else if(role.toLower() == "role_attacker"){
        pixmp.load(":/gui/atk.png");
    }else if(role.toLower() == "role_support"){
        pixmp.load(":/gui/sup.png");
    }else{
        pixmp.load(":/gui/none.png");
    }

    return pixmp;
}

void MainWindow::addRoot(){
    QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);

    item->setText(0, "Strategy");

    root = item;
}

void MainWindow::resetTree(QString strat, QList<QString> playbookList, QMap<QString, QList<QString>> rolesList,
                           QMap<std::pair<QString, QString>, QList<std::pair<QString, quint8>>> playersList,
                           QMap<QString, QString> behavioursList){
    // Changing strategy name
    root->setText(0, strat);

    // Parsing playbooks
    for(int x = 0; x < playbookList.size(); x++){
        if(!isContained(root, playbookList.at(x))){
            addChild(root, playbookList.at(x))->setIcon(0, QIcon(getPlaybookPixmap(playbookList.at(x))));
        }
    }
    // Removing old playbooks
    removeOld(root, playbookList);

    // Parsing roles
    for(int x = 0; x < root->childCount(); x++){
        QList<QString> list = rolesList[root->child(x)->text(0)];
        for(int y = 0; y < list.size(); y++){
            if(!isContained(root->child(x), list[y])){
                addChild(root->child(x), list[y])->setIcon(0, QIcon(getRolePixmap(list[y])));
            }
        }
        // Removing old roles
        removeOld(root->child(x), list);
    }

    // Parsing robots
    for(int x = 0; x < root->childCount(); x++){
        for(int y = 0; y < root->child(x)->childCount(); y++){
            QList<std::pair<QString, quint8>> list = playersList[std::make_pair(root->child(x)->text(0), root->child(x)->child(y)->text(0))];
            QList<QString> listParse;
            for(int z = 0; z < list.size(); z++){
                if(!isContained(root->child(x)->child(y), list.at(z).first)){
                    QTreeWidgetItem *item = addChild(root->child(x)->child(y), list[z].first);
                    item->setIcon(0, QIcon(playerPixmaps[list.at(z).second]));
                }
                // Saving strings for remove later
                listParse.push_back(list[z].first);
            }
            // Removing old robots
            removeOld(root->child(x)->child(y), listParse);
        }
    }

    // Parsing behaviours
    for(int x = 0; x < root->childCount(); x++){ // pb
        for(int y = 0; y < root->child(x)->childCount(); y++){ // role
            for(int w = 0; w < root->child(x)->child(y)->childCount(); w++){ // robot
                QTreeWidgetItem *player = root->child(x)->child(y)->child(w);
                QString playerName = player->text(0);
                if(player->childCount() == 0){ // never had a behaviour before
                    addChild(player, behavioursList[playerName]);
                }else{                         // update existing behaviour
                    player->child(0)->setText(0, behavioursList[playerName]);
                }
            }
        }
    }
}

QTreeWidgetItem* MainWindow::addChild(QTreeWidgetItem* parent, QString text){
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, text);

    parent->addChild(item);

    return item;
}

bool MainWindow::isContained(QTreeWidgetItem *parent, QString text){
    for(int x = 0; x < parent->childCount(); x++){
        if(parent->child(x)->text(0) == text)
            return true;
    }
    return false;
}

void MainWindow::removeOld(QTreeWidgetItem *parent, QList<QString> stringList){
    for(int x = 0; x < parent->childCount(); x++){
        bool found = false;
        for(int y = 0; y < stringList.size(); y++){
            if(parent->child(x)->text(0) == stringList.at(y)){
                found = true;
                break;
            }
        }
        if(!found) parent->takeChild(x);
    }
}

QPixmap MainWindow::getPlaybookPixmap(QString playbook){
    QPixmap pixmp;

    if(playbook.toLower() == "playbook_offensive"){ // teste
        pixmp.load(":/gui/atk.png");
    }
    else if(playbook.toLower() == "playbook_defensive"){
        pixmp.load(":/gui/gk.png");
    }
    else{
        pixmp.load(":/gui/none.png");
    }

    return pixmp;
}

void MainWindow::setGameStage(QString stage){
    ui->gameStage->setText(stage);
}

void MainWindow::setRefereeCommand(QString command){
    ui->refereeCommand->setText(command);
}

void MainWindow::setTimeLeft(QString timeLeft){
    ui->timeLeft->setText(timeLeft);
}

void MainWindow::setPlayerBattery(quint8 id, int qt){
    this->playerBatteries[id]->setValue(qt);
}
