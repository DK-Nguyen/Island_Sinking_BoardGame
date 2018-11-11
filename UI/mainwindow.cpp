#include <mainwindow.h>
#include <hexboard.h>
#include <QHBoxLayout>
#include <QDockWidget>

MainWindow::MainWindow(std::shared_ptr<Common::IGameRunner> game_runner,
                       std::shared_ptr<GameBoard> game_board,
                       std::shared_ptr<GameState> game_state,
                       std::vector<std::shared_ptr<Common::IPlayer>> players,
                       std::shared_ptr<std::unordered_map<int, std::string>> player_names,
                       std::shared_ptr<std::unordered_map<std::string, int>> points,
                       std::shared_ptr<std::vector<std::pair<std::string, int>>> top10,
                       QWidget *parent)
    : QWidget (parent)
{

    this->game_runner = game_runner;
    this->game_board = game_board;
    this->game_state = game_state;
    this->players = players;
    this->player_names = player_names;
    this->points = points;
    this->top10 = top10;

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    hex_board = new HexBoard(game_runner,
                             game_board,
                             game_state,
                             players,
                             HEXBOARD_WIDTH,
                             HEXBOARD_HEIGHT,
                             HEXBOARD_SCALE,
                             this);

//    QSizePolicy hex_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    hex_board_pol.setHorizontalStretch(1);
//    hex_board->setSizePolicy(hex_board_pol);

    control_board = new ControlBoard(game_runner, game_state, CONTROLBOARD_SCALE, this);

//    QSizePolicy control_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    control_board_pol.setHorizontalStretch(1);
//    control_board->setSizePolicy(control_board_pol);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(hex_board);
    layout->addWidget(control_board);
    setLayout(layout);

    connect(control_board, SIGNAL(control_board_close()), this, SLOT(quit_game()));
}


void MainWindow::quit_game()
// TODO: handle cleanup before quiting
{
    close();
}

