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


    ConfigurationWindow *config_window = new ConfigurationWindow(nullptr);
    MainWindow *main_window = new MainWindow(nullptr);

    QObject::connect(config_window, SIGNAL(game_start(Configuration)), main_window, SLOT(initialize_game(Configuration)));

    config_window->show();

    return a.exec();
}
