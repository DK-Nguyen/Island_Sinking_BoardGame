#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gameboard.hh"
#include "gamestate.hh"
#include "player.hh"
#include "initialize.hh"
#include <iostream>
#include "hexboard.h"
#include <memory>
#include "controlboard.h"
#include <QWidget>
#include <QDockWidget>
#include <configurationwindow.h>
#include <igamerunner.hh>

const double BOARD_SCALE = 1.2;
const int HEXBOARD_WIDTH = 720;
const int HEXBOARD_HEIGHT = 720;
const int CONTROLBOARD_WIDTH = 350;
const int CONTROLBOARD_HEIGHT = 720;
const int HEXBOARD_SCALE = 5;
const int WINDOW_WIDTH = HEXBOARD_WIDTH + 350 + 30;
const int WINDOW_HEIGHT = HEXBOARD_HEIGHT + 30;



class MainWindow: public QWidget
{
    Q_OBJECT
public:

    MainWindow(std::shared_ptr<std::vector<std::pair<std::string, int>>> top10,
               QWidget* parent);


public slots:
    void quit_game();
    void initialize_game(Configuration config);

private:
    std::shared_ptr<Common::IGameRunner> game_runner;
    std::shared_ptr<GameBoard> game_board;
    std::shared_ptr<GameState> game_state;
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    std::shared_ptr<std::unordered_map<int, std::string>> player_names;
    std::shared_ptr<std::unordered_map<std::string, int>> points;
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10;
    HexBoard* hex_board;
    ControlBoard* control_board;
    Configuration config;

    void construct_window();

};

#endif // MAINWINDOW_H
