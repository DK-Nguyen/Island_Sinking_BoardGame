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

    auto top10 = std::make_shared<std::vector<std::pair<std::string, int>>>();
    top10->push_back(std::make_pair("Ping", 10));
    top10->push_back(std::make_pair("Pong", 12));

    ConfigurationWindow *config_window = new ConfigurationWindow(nullptr);
    MainWindow *main_window = new MainWindow(top10, nullptr);

    QObject::connect(config_window, SIGNAL(game_start(Configuration)), main_window, SLOT(initialize_game(Configuration)));

    config_window->show();

    return a.exec();
}
