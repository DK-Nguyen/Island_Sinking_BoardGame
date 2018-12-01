#include <mainwindow.h>
#include <hexboard.h>
#include <QHBoxLayout>
#include <QDockWidget>
#include <igamerunner.hh>
#include <gameover.h>
#include <QSettings>


namespace UI{

MainWindow::MainWindow(QWidget *parent)
    : QWidget (parent)
{
    top10_ = std::make_shared<std::vector<std::pair<std::string, int>>>();
    loadTop10();

    mountainTiles_.push_back(Common::CubeCoordinate(0,1,-1));
    mountainTiles_.push_back(Common::CubeCoordinate(1,0,-1));
    mountainTiles_.push_back(Common::CubeCoordinate(1,-1,0));
    mountainTiles_.push_back(Common::CubeCoordinate(0,-1,1));
    mountainTiles_.push_back(Common::CubeCoordinate(-1,0,1));
    mountainTiles_.push_back(Common::CubeCoordinate(1,1,0));

}

void MainWindow::initializeGame(UI::Configuration config)
{

    this->config_ = config;

    gameBoard_ = std::make_shared<Student::GameBoard>();
    playerNames_ = std::make_shared<std::unordered_map<int, std::string>>();
    points_ = std::make_shared<std::unordered_map<std::string, int>>();
    int playerId = 0;
    int pawnId = config.players.size() + 10;

    for (auto name : config.players)
    {
        auto playerPtr = std::make_shared<Student::Player>(playerId);

        playerPtr->setActionsLeft(3);
        players_.push_back(playerPtr);
        playerNames_->insert(std::make_pair(playerId, name.toUtf8().constData()));
        points_->insert(std::make_pair(name.toUtf8().constData(), config.noPawn));

        for (auto i=0; i< config.noPawn; i++)
        {
            auto pawnData = std::make_pair(playerId, pawnId);
            pawnList_.push_back(pawnData);
            pawnId += 1;
        }
        playerId += 1;
    }


    gameState_ = std::make_shared<Student::GameState>(Common::GamePhase(1), 1, players_, playerNames_, points_, top10_);


    gameRunner_ = Common::Initialization::getGameRunner(gameBoard_, gameState_, players_);


    unsigned int pawnCount = 0;
    unsigned int index;
    for (auto pawnData : pawnList_)
    {
        index = pawnCount / 4;
        gameBoard_->addPawn(pawnData.first, pawnData.second, mountainTiles_[index]);
        pawnCount++;
    }
    constructWindow();
}

void MainWindow::updatePoint(std::vector<int> IDs, std::vector<int> increment)
{
    for (unsigned int i=0; i< IDs.size(); i++)
    {
        (*points_)[playerNames_->at(IDs[i])] += increment[i];
    }

    emit(pointChange(IDs));

    return;
}

void MainWindow::gameOver(std::string winner)
{
    saveTop10();
    auto gameOverWindow = new UI::GameOverWindow(winner);
    connect(gameOverWindow, SIGNAL(quit()), this, SLOT(quitGame()));
    connect(gameOverWindow, SIGNAL(playAgain()), this, SLOT(playAgain()));
    hexBoard_->clear();
    hexBoard_->close();
    controlBoard_->clear();
    controlBoard_->close();
    gameOverWindow->show();
}

void MainWindow::playAgain()
// clear old data and create new data again
{
    clear();
    top10_->clear();
    loadTop10();
    initializeGame(config_);
}

void MainWindow::constructWindow()
{

    setFixedSize(WINDOW_WIDTH*BOARD_SCALE, WINDOW_HEIGHT*BOARD_SCALE);

    hexBoard_ = new UI::HexBoard(gameRunner_,
                             gameBoard_,
                             gameState_,
                             players_,
                             HEXBOARD_WIDTH,
                             HEXBOARD_HEIGHT,
                             HEXBOARD_SCALE,
                             this);

    hexBoard_->scale(BOARD_SCALE, BOARD_SCALE);
    hexBoard_->setFixedSize(HEXBOARD_WIDTH*BOARD_SCALE, HEXBOARD_HEIGHT*BOARD_SCALE);

//    QSizePolicy hex_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    hex_board_pol.setHorizontalStretch(1);
//    hex_board->setSizePolicy(hex_board_pol);

    controlBoard_ = new UI::ControlBoard(gameRunner_, gameState_, 1, this);
    controlBoard_->scale(BOARD_SCALE, BOARD_SCALE);
    controlBoard_->setFixedSize(CONTROLBOARD_WIDTH*BOARD_SCALE,CONTROLBOARD_HEIGHT*BOARD_SCALE);

//    QSizePolicy control_board_pol(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    control_board_pol.setHorizontalStretch(1);
//    control_board->setSizePolicy(control_board_pol);

    if (layout()!=nullptr)
    {
        delete layout();
    }

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(hexBoard_);
    layout->addWidget(controlBoard_);
    setLayout(layout);

    connect(controlBoard_, SIGNAL(controlBoardClose()), this, SLOT(quitGame()));
    connect(hexBoard_, SIGNAL(updatePlayerTurn()), controlBoard_, SLOT(updateCurrentTurn()));
    connect(hexBoard_, SIGNAL(updateStage()), controlBoard_, SLOT(updateStage()));
    connect(hexBoard_, SIGNAL(updateMovementLeft()), controlBoard_, SLOT(updateMovementLeft()));
    connect(hexBoard_, SIGNAL(setWheelClick(bool)), controlBoard_, SLOT(setWheelClick(bool)));
    connect(this, SIGNAL(pointChange(std::vector<int>)), controlBoard_, SLOT(updatePoint(std::vector<int>)));
    connect(hexBoard_, SIGNAL(updatePoint(std::vector<int>, std::vector<int>)), this, SLOT(updatePoint(std::vector<int>, std::vector<int>)));
    connect(controlBoard_, SIGNAL(wheelSpin(std::pair<std::string, std::string>)), hexBoard_, SLOT(wheelClicked(std::pair<std::string, std::string>)));
    connect(hexBoard_, SIGNAL(gameOver(std::string)), this, SLOT(gameOver(std::string)));
    this->show();
}

void MainWindow::loadTop10()
{
    QSettings settings;
    QHash<QString, QVariant> top10Tmp;
    top10Tmp = settings.value("top10", QVariant::fromValue(top10Tmp)).toHash();

    for (auto it=top10Tmp.begin(); it!=top10Tmp.end(); it++)
    {
        top10_->push_back(std::make_pair(it.key().toStdString(),it.value().toInt()));
    }

    auto compareOperator = Compare();

    std::sort(top10_->begin(), top10_->end(), compareOperator);

}

void MainWindow::saveTop10()
{
    for (auto it: *points_)
    {
        top10_->push_back(it);
    }

    auto compareOperator = Compare();

    std::sort(top10_->begin(), top10_->end(), compareOperator);

    std::vector<std::pair<std::string, int>> newTop10;
    for (auto i=0; i< top10_->size() && i < 10; i++)
    {
        newTop10.push_back((*top10_)[i]);
    }

    QSettings settings;
    QHash<QString, QVariant> top10Tmp;
    for (auto it : newTop10)
    {
        top10Tmp.insert(QString::fromUtf8(it.first.c_str()), QVariant::fromValue(it.second));
    }

    settings.setValue("top10", top10Tmp);
}

void MainWindow::clear()
{
    players_.clear();
    pawnList_.clear();
}


void MainWindow::quitGame()
// TODO: clear data from both controlboard and hexboard and backend data
{
    saveTop10();
    hexBoard_->clear();
    hexBoard_->close();
    controlBoard_->clear();
    controlBoard_->close();
    clear();
    close();
}

}
