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
    std::shared_ptr<std::unordered_map<int, std::string>> player_names = std::make_shared<std::unordered_map<int, std::string>>();
    std::shared_ptr<std::unordered_map<std::string, int>> points = std::make_shared<std::unordered_map<std::string, int>>();
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();

    // add the first player with id=0 and name "dat", 0 point
    std::shared_ptr<Player> player_ptr = std::make_shared<Player>(0);
    players.push_back(player_ptr);
    player_names->insert(std::make_pair(0, "dat"));
    points->insert(std::make_pair("dat", 0));
    // ad the second player with id=1 and name "khoa", 0 point
    std::shared_ptr<Player> player_ptr2 = std::make_shared<Player>(1);
    players.push_back(player_ptr2);
    player_names->insert(std::make_pair(1, "khoa"));
    points->insert(std::make_pair("khoa", 0));

    std::shared_ptr<GameState> game_state = std::make_shared<GameState>
            (Common::GamePhase(1), 1, players, player_names, points, top10);

    QCOMPARE(game_state->currentGamePhase(), Common::GamePhase(1));
    QCOMPARE(game_state->currentPlayer(), 1);
    QCOMPARE(game_state->currentPlayerName(), player_names->at(1));
    QCOMPARE(game_state->get_player_name(1), player_names->at(1));
    QCOMPARE(game_state->currentGamePhaseName(), QString("movement"));
}

void gamestatetest::testChangeGamePhase()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player_names = std::make_shared<std::unordered_map<int, std::string>>();
    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();
    auto game_state = std::make_shared<GameState>(Common::GamePhase(1), 1, players, player_names, points, top10);

    game_state->changeGamePhase(Common::GamePhase(2));
    QCOMPARE(game_state->currentGamePhaseName(), QString("sinking"));
}

void gamestatetest::testChangePlayerTurn()
{
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    auto player_names = std::make_shared<std::unordered_map<int, std::string>>();
    auto points = std::make_shared<std::unordered_map<std::string, int>>();
    auto top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();
    auto game_state = std::make_shared<GameState>(Common::GamePhase(1), 1, players, player_names, points, top10);

    game_state->changePlayerTurn(2);
    QCOMPARE(game_state->currentPlayer(), 2);
}




QTEST_APPLESS_MAIN(gamestatetest)

#include "tst_gamestatetest.moc"
