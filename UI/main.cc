#include "gameboard.hh"
#include "gamestate.hh"
#include "player.hh"
#include "initialize.hh"
#include<iostream>
#include "hexboard.h"
#include <memory>
#include <QApplication>
#include "controlboard.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player1_ptr = std::make_shared<Player>(1);
    auto player2_ptr = std::make_shared<Player>(2);

    player1_ptr->setActionsLeft(3);
    player2_ptr->setActionsLeft(3);

    auto player_names = std::make_shared<std::unordered_map<int, std::string>>();
    player_names->insert(std::make_pair(1, "Khoa"));
    player_names->insert(std::make_pair(2, "Dat"));

    players.push_back(player1_ptr);
    players.push_back(player2_ptr);
    std::cerr << "finish creating player vector \n";

    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 = std::make_shared<std::vector<std::pair<std::string, int>>>();

    points->insert(std::make_pair("Khoa", 10));
    points->insert(std::make_pair("Dat", 9));

    top10->push_back(std::make_pair("Ping", 20));
    top10->push_back(std::make_pair("Pong", 30));

    auto game_board_ptr = std::make_shared<GameBoard>();
    std::cerr << "finish creating game board \n";

    auto game_state_ptr = std::make_shared<GameState>(Common::GamePhase(), 1, players, player_names, points, top10);
    std::cerr << "finish creating game state \n";

    std::cerr << "creating game runner \n";
    auto game_runner_ptr = Common::Initialization::getGameRunner(game_board_ptr, game_state_ptr, players);
    std::cerr << "finish creating game runner\n";

    game_board_ptr->print_hex_list();
    game_board_ptr->print_hex_stat();

    MainWindow * main_window = new MainWindow(game_runner_ptr,
                                              game_board_ptr,
                                              game_state_ptr,
                                              players,
                                              player_names,
                                              points,
                                              top10,
                                              nullptr);

//    HexBoard* main_window = new HexBoard(game_runner_ptr,
//                                         game_board_ptr,
//                                         game_state_ptr,
//                                         players,
//                                         650,
//                                         720,
//                                         10,
//                                         nullptr);


//    ControlBoard* main_window = new ControlBoard(game_runner_ptr, game_state_ptr, 1.0, nullptr);
    main_window->show();

    return a.exec();
}
