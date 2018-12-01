#include <QtTest>

// add necessary includes here
#include <gamestate.hh>
#include <gameexception.hh>
#include "iostream"
#include <memory>
#include "player.hh"
#include "string"

class gamestatetest : public QObject
{
    Q_OBJECT

public:
    gamestatetest();
    ~gamestatetest();

private slots:
    void testInit();
    void testChangeGamePhase();
    void testChangePlayerTurn();
    void testgetActionsLeft();
};

gamestatetest::gamestatetest()
{
}

gamestatetest::~gamestatetest()
{   
}

void gamestatetest::testInit()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    std::shared_ptr<std::unordered_map<int, std::string>> playerNames = std::make_shared<std::unordered_map<int, std::string>>();
    std::shared_ptr<std::unordered_map<std::string, int>> points = std::make_shared<std::unordered_map<std::string, int>>();
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();

    // add the first player with id=0 and name "dat", 0 point
    auto player_ptr = std::make_shared<Student::Player>(0);
    players.push_back(player_ptr);
    playerNames->insert(std::make_pair(0, "dat"));
    points->insert(std::make_pair("dat", 0));
    // add the second player with id=1 and name "khoa", 0 point
    auto player_ptr2 = std::make_shared<Student::Player>(1);
    players.push_back(player_ptr2);
    playerNames->insert(std::make_pair(1, "khoa"));
    points->insert(std::make_pair("khoa", 0));

    auto gameState = std::make_shared<Student::GameState>
            (Common::GamePhase(1), 1, players, playerNames, points, top10);

    QCOMPARE(gameState->currentGamePhase(), Common::GamePhase(1));
    QCOMPARE(gameState->currentPlayer(), 1);
    QCOMPARE(gameState->currentPlayerName(), playerNames->at(1));
    QCOMPARE(gameState->getPlayerName(1), playerNames->at(1));
    QCOMPARE(gameState->currentGamePhaseName(), QString("movement"));
}

void gamestatetest::testChangeGamePhase()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player_names = std::make_shared<std::unordered_map<int, std::string>>();
    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();
    auto gameState = std::make_shared<Student::GameState>(Common::GamePhase(1), 1, players, player_names, points, top10);

    gameState->changeGamePhase(Common::GamePhase(2));
    QCOMPARE(gameState->currentGamePhaseName(), QString("sinking"));
}

void gamestatetest::testChangePlayerTurn()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto playerNames = std::make_shared<std::unordered_map<int, std::string>>();
    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();
    auto gameState = std::make_shared<Student::GameState>(Common::GamePhase(1), 1, players, playerNames, points, top10);

    gameState->changePlayerTurn(2);
    QCOMPARE(gameState->currentPlayer(), 2);
}

void gamestatetest::testgetActionsLeft()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player_names = std::make_shared<std::unordered_map<int, std::string>>();
    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();

    // add the first player with id=0 and name "dat", 0 point
    std::shared_ptr<Student::Player> playerPtr = std::make_shared<Student::Player>(0);
    players.push_back(playerPtr);
    player_names->insert(std::make_pair(0, "dat"));
    points->insert(std::make_pair("dat", 0));
    playerPtr->setActionsLeft(3);
    // add the second player with id=1 and name "khoa", 0 point
    std::shared_ptr<Student::Player> playerPtr2 = std::make_shared<Student::Player>(1);
    players.push_back(playerPtr2);
    player_names->insert(std::make_pair(1, "khoa"));
    points->insert(std::make_pair("khoa", 0));
    playerPtr2->setActionsLeft(2);

    auto gameState = std::make_shared<Student::GameState>
            (Common::GamePhase(1), 0, players, player_names, points, top10);

    QCOMPARE(gameState->getActionsLeft(), 3u);

    gameState->changePlayerTurn(1);
    QCOMPARE(gameState->getActionsLeft(), 2u);
}




QTEST_APPLESS_MAIN(gamestatetest)

#include "tst_gamestatetest.moc"
