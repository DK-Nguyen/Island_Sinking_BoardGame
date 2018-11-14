#include <mainwindow.h>
#include <hexboard.h>
#include <QHBoxLayout>
#include <QDockWidget>
#include <igamerunner.hh>

MainWindow::MainWindow(std::shared_ptr<std::vector<std::pair<std::string, int>>> top10,
                       QWidget *parent)
    : QWidget (parent)
{
    this->top10 = top10;
    mountain_tiles.push_back(Common::CubeCoordinate(0,1,-1));
    mountain_tiles.push_back(Common::CubeCoordinate(1,0,-1));
    mountain_tiles.push_back(Common::CubeCoordinate(1,-1,0));
    mountain_tiles.push_back(Common::CubeCoordinate(0,-1,1));
    mountain_tiles.push_back(Common::CubeCoordinate(-1,0,1));
    mountain_tiles.push_back(Common::CubeCoordinate(1,1,0));

}

void MainWindow::initialize_game(Configuration config)
{

    this->config = config;

    game_board = std::make_shared<GameBoard>();
    player_names = std::make_shared<std::unordered_map<int, std::string>>();
    points = std::make_shared<std::unordered_map<std::string, int>>();
    int player_id = 0;
    int pawn_id = config.players.size() + 10;

    for (auto name : config.players)
    {
        auto player_ptr = std::make_shared<Player>(player_id);

        player_ptr->setActionsLeft(3);
        players.push_back(player_ptr);
        player_names->insert(std::make_pair(player_id, name.toUtf8().constData()));
        points->insert(std::make_pair(name.toUtf8().constData(), 0));

        for (auto i=0; i< config.no_pawn; i++)
        {
            auto pawn_data = std::make_pair(player_id, pawn_id);
            pawn_list.push_back(pawn_data);
            pawn_id += 1;
        }
        player_id += 1;
    }


    game_state = std::make_shared<GameState>(Common::GamePhase(1), 1, players, player_names, points, top10);


    game_runner = Common::Initialization::getGameRunner(game_board, game_state, players);


    unsigned int pawn_count = 0;
    unsigned int index;
    for (auto pawn_data : pawn_list)
    {
        index = pawn_count / 4;
        game_board->addPawn(pawn_data.first, pawn_data.second, mountain_tiles[index]);
        pawn_count++;
    }
    construct_window();
}

void MainWindow::construct_window()
{

    setFixedSize(WINDOW_WIDTH*BOARD_SCALE, WINDOW_HEIGHT*BOARD_SCALE);

    hex_board = new HexBoard(game_runner,
                             game_board,
                             game_state,
                             players,
                             HEXBOARD_WIDTH,
                             HEXBOARD_HEIGHT,
                             HEXBOARD_SCALE,
                             this);

    hex_board->scale(BOARD_SCALE, BOARD_SCALE);
    hex_board->setFixedSize(HEXBOARD_WIDTH*BOARD_SCALE, HEXBOARD_HEIGHT*BOARD_SCALE);

//    QSizePolicy hex_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    hex_board_pol.setHorizontalStretch(1);
//    hex_board->setSizePolicy(hex_board_pol);

    control_board = new ControlBoard(game_runner, game_state, 1, this);
    control_board->scale(BOARD_SCALE, BOARD_SCALE);
    control_board->setFixedSize(CONTROLBOARD_WIDTH*BOARD_SCALE,CONTROLBOARD_HEIGHT*BOARD_SCALE);

//    QSizePolicy control_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    control_board_pol.setHorizontalStretch(1);
//    control_board->setSizePolicy(control_board_pol);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(hex_board);
    layout->addWidget(control_board);
    setLayout(layout);

    connect(control_board, SIGNAL(control_board_close()), this, SLOT(quit_game()));

    this->show();
}


void MainWindow::quit_game()
// TODO: handle cleanup before quiting
{
    close();
}

