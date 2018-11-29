#include <mainwindow.h>
#include <hexboard.h>
#include <QHBoxLayout>
#include <QDockWidget>
#include <igamerunner.hh>
#include <gameover.h>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QWidget (parent)
{
    top10 = std::make_shared<std::vector<std::pair<std::string, int>>>();
    loadTop10();

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

    game_board = std::make_shared<Student::GameBoard>();
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
        points->insert(std::make_pair(name.toUtf8().constData(), config.no_pawn));

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
    constructWindow();
}

void MainWindow::update_point(std::vector<int> IDs, std::vector<int> increment)
{
    for (unsigned int i=0; i< IDs.size(); i++)
    {
        (*points)[player_names->at(IDs[i])] += increment[i];
    }

    emit(point_change(IDs));

    return;
}

void MainWindow::game_over()
{
    saveTop10();
    auto game_over_window = new GameOverWindow();
    connect(game_over_window, SIGNAL(quit()), this, SLOT(quit_game()));
    connect(game_over_window, SIGNAL(play_again()), this, SLOT(play_again()));
    hex_board->clear();
    hex_board->close();
    control_board->clear();
    control_board->close();
    game_over_window->show();
}

void MainWindow::play_again()
// clear old data and create new data again
{
    clear();
    top10->clear();
    loadTop10();
    initialize_game(config);
}

void MainWindow::constructWindow()
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

    if (layout()!=0)
    {
        delete layout();
    }

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(hex_board);
    layout->addWidget(control_board);
    setLayout(layout);

    connect(control_board, SIGNAL(control_board_close()), this, SLOT(quit_game()));
    connect(hex_board, SIGNAL(update_player_turn()), control_board, SLOT(update_current_turn()));
    connect(hex_board, SIGNAL(update_stage()), control_board, SLOT(update_stage()));
    connect(hex_board, SIGNAL(update_movement_left()), control_board, SLOT(update_movement_left()));
    connect(hex_board, SIGNAL(set_wheel_click(bool)), control_board, SLOT(set_wheel_click(bool)));
    connect(this, SIGNAL(point_change(std::vector<int>)), control_board, SLOT(update_point(std::vector<int>)));
    connect(hex_board, SIGNAL(update_point(std::vector<int>, std::vector<int>)), this, SLOT(update_point(std::vector<int>, std::vector<int>)));
    connect(control_board, SIGNAL(wheel_spin(std::pair<std::string, std::string>)), hex_board, SLOT(wheel_clicked(std::pair<std::string, std::string>)));
    connect(hex_board, SIGNAL(game_over()), this, SLOT(game_over()));
    this->show();
}

void MainWindow::loadTop10()
{
    QSettings settings;
    QHash<QString, QVariant> top10_tmp;
    top10_tmp = settings.value("top10", QVariant::fromValue(top10_tmp)).toHash();

    for (auto it=top10_tmp.begin(); it!=top10_tmp.end(); it++)
    {
        top10->push_back(std::make_pair(it.key().toStdString(),it.value().toInt()));
    }
}

void MainWindow::saveTop10()
{
    for (auto it: *points)
    {
        top10->push_back(it);
    }

    auto compare_operator = Compare();

    std::sort(top10->begin(), top10->end(), compare_operator);

    std::vector<std::pair<std::string, int>> new_top10;
    for (auto i=0; i< top10->size() && i < 10; i++)
    {
        new_top10.push_back((*top10)[i]);
    }

    QSettings settings;
    QHash<QString, QVariant> top10_tmp;
    for (auto it : new_top10)
    {
        top10_tmp.insert(QString::fromUtf8(it.first.c_str()), QVariant::fromValue(it.second));
    }

    settings.setValue("top10", top10_tmp);
}

void MainWindow::clear()
{
    players.clear();
    pawn_list.clear();
}


void MainWindow::quit_game()
// TODO: clear data from both controlboard and hexboard and backend data
{
    saveTop10();
    hex_board->clear();
    hex_board->close();
    control_board->clear();
    control_board->close();
    clear();
    close();
}

