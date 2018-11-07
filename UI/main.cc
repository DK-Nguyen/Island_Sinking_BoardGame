#include "gameboard.hh"
#include "gamestate.hh"
#include "player.hh"
#include "initialize.hh"
#include<iostream>
#include <mainwindow.h>
#include <memory>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player1_ptr = std::make_shared<Player>(1);
    auto player2_ptr = std::make_shared<Player>(2);

    players.push_back(player1_ptr);
    players.push_back(player2_ptr);
    std::cerr << "finish creating player vector \n";

    auto game_board_ptr = std::make_shared<GameBoard>();
    std::cerr << "finish creating game board \n";

    auto game_state_ptr = std::make_shared<GameState>(Common::GamePhase(), 1);
    std::cerr << "finish creating game state \n";

    std::cerr << "creating game runner \n";
    auto game_runner_ptr = Common::Initialization::getGameRunner(game_board_ptr, game_state_ptr, players);
    std::cerr << "finish creating game runner\n";

    game_board_ptr->print_hex_list();
    game_board_ptr->print_hex_stat();

    MainWindow* main_window = new MainWindow(game_runner_ptr, game_board_ptr, game_state_ptr, players, 10, nullptr);
    main_window->show();
    main_window->start();
    return a.exec();
}
