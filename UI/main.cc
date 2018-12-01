#include<iostream>
#include <memory>
#include <QApplication>
#include "configurationwindow.h"
#include <QObject>
#include <mainwindow.h>



int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    a.setOrganizationName("DK Group");
    a.setOrganizationDomain("SWD");
    a.setApplicationName("Island Sinking Game");


    UI::ConfigurationWindow *configWindow = new UI::ConfigurationWindow(nullptr);
    UI::MainWindow *mainWindow = new UI::MainWindow(nullptr);

    QObject::connect(configWindow, SIGNAL(gameStart(UI::Configuration)), mainWindow, SLOT(initializeGame(UI::Configuration)));

    configWindow->show();

    return a.exec();
}
