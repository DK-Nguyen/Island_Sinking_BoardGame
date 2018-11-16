#include <QtTest>

// add necessary includes here
#include <gamestate.hh>
#include <gameexception.hh>
#include <player.hh>
#include <memory>
#include "iplayer.hh"

class gamestatetest : public QObject
{
    Q_OBJECT

public:
    gamestatetest();
    ~gamestatetest();

private slots:
    void initGamestate();

};

gamestatetest::gamestatetest()
{
}

gamestatetest::~gamestatetest()
{   
}

void gamestatetest::initGamestate()
{
    std::shared_ptr<std::unordered_map<int, std::string>> player_names = std::make_shared<std::unordered_map<int, std::string>>();
    std::shared_ptr<std::unordered_map<std::string, int>> points = std::make_shared<std::unordered_map<std::string, int>>();
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10 =  std::make_shared<std::vector<std::pair<std::string, int>>>();
    std::vector<std::shared_ptr<Common::IPlayer>> players;
//    auto player_ptr = std::make_shared<Player>(0);
//    auto player_ptr2 = std::make_shared<Player>(1);
//    players.push_back(player_ptr);
//    players.push_back(player_ptr2);

    std::shared_ptr<GameState> game_state = std::make_shared<GameState>(Common::GamePhase(1), 1, players, player_names, points, top10);
    QCOMPARE(game_state->currentGamePhase(), Common::GamePhase(1));
    QCOMPARE(game_state->currentPlayer(), 1);


}


QTEST_APPLESS_MAIN(gamestatetest)

#include "tst_gamestatetest.moc"
