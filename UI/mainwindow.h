﻿#ifndef MAINWINDOW_H
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

const double BOARD_SCALE = 1.0;
const int HEXBOARD_WIDTH = 720;
const int HEXBOARD_HEIGHT = 720;
const int CONTROLBOARD_WIDTH = 350;
const int CONTROLBOARD_HEIGHT = 720;
const int HEXBOARD_SCALE = 5;
const int WINDOW_WIDTH = HEXBOARD_WIDTH + 350 + 30;
const int WINDOW_HEIGHT = HEXBOARD_HEIGHT + 30;

struct Compare{
  bool operator() (std::pair<std::string, int> x, std::pair<std::string, int> y) { return (x.second > y.second);}
};

namespace UI{

class MainWindow: public QWidget
{
    Q_OBJECT
public:

    MainWindow(QWidget* parent);


signals:
    // signal sent to controlBoard to change points
    void pointChange(std::vector<int>);

public slots:
    // handle game quitting
    void quitGame();

    // initialize the game
    void initializeGame(UI::Configuration cconfig_);

    // update the point and send signal to controlBoard
    void updatePoint(std::vector<int> IDs, std::vector<int> increment);

    // handle game over case
    void gameOver(std::string winner);

    // handle play again functionality
    void playAgain();

private:
    std::shared_ptr<Common::IGameRunner> gameRunner_;
    std::shared_ptr<Student::GameBoard> gameBoard_;
    std::shared_ptr<Student::GameState> gameState_;
    std::vector<std::shared_ptr<Common::IPlayer>> players_;
    std::vector<std::pair<int, int>> pawnList_;
    std::shared_ptr<std::unordered_map<int, std::string>> playerNames_;
    std::shared_ptr<std::unordered_map<std::string, int>> points_;
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10_;
    UI::HexBoard* hexBoard_;
    UI::ControlBoard* controlBoard_;
    UI::Configuration config_;
    std::vector<Common::CubeCoordinate> mountainTiles_;

    // create hexBoard and controlBoard and display game elements
    void constructWindow();

    // read top10 players from setting file
    void loadTop10();

    // save top10 players to setting file
    void saveTop10();
    void clear();

};

}
#endif // MAINWINDOW_H
