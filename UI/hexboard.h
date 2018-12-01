#ifndef HEXBOARD_H
#define HEXBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <hex.hh>
#include <graphichex.h>
#include <graphicpawn.h>
#include <graphicactor.h>
#include <graphictransport.h>
#include <gameengine.hh>
#include <QVector>
#include <QPointF>
#include <gameboard.hh>
#include <gamestate.hh>
#include <player.hh>
#include <QColor>

const double ACTOR_SCALE = 0.02;



struct HexData{
    Common::CubeCoordinate coord;
    std::list<UI::GraphicActor*> actors;
    std::list<UI::GraphicTransport*> transports;
    std::list<UI::GraphicPawn*> pawns;
};

namespace UI{

class HexBoard: public QGraphicsView{
    Q_OBJECT
public:
    // constructors
    HexBoard(std::shared_ptr<Common::IGameRunner> gameEnginePtr,
             std::shared_ptr<Student::GameBoard> gameboardPtr,
             std::shared_ptr<Student::GameState> gamestatePtr,
             std::vector<std::shared_ptr<Common::IPlayer>> gamePlayers,
             int width,
             int height,
             int boardScale,
             QWidget* parent=nullptr);

    // container to hold items
    QGraphicsScene* scene_;

    std::shared_ptr<Common::IGameRunner> gameEngine_;
    std::shared_ptr<Student::GameBoard> gameBoard_;
    std::shared_ptr<Student::GameState> gameState_;
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    int boardScale_, width, height;

    void clear();

signals:
    void setPawnMovement(bool allowed, std::list<int> pawn_id);
    void setActorMovement(bool allowed, std::list<int> actor_id);
    void setTransportMovement(bool allowed);
    void setWheelClick(bool flag);
    void updateStage();
    void updateMovementLeft();
    void updatePlayerTurn();
    void updatePoint(std::vector<int> IDs, std::vector<int> increment);
    void gameOver(std::string);


public slots:
    // receive click signal from GraphicHex object and perform hex flipping if valid
    void hexClicked(int id);

    // receive click signal from wheel
    void wheelClicked(std::pair<std::string, std::string> wheelOutput);

    // receive movement signal from GraphicPawn object and check if move is valid and act accordingly
    void pawnIsMoved(int pawnId, QPointF oldPos, QPointF newPos);

    // receive movement signal from GraphicActor object and check if move is valid and act accordingly
    void actorIsMoved(int actorId, QPointF oldPos, QPointF newPos);

    // receive movement signal from GraphicTransport object and check if move is valid and act accordingly
    void transportIsMoved(int transportId, QPointF oldPos, QPointF newPos);


private slots:
    // functionality to perform zooming
    void scalingTime(qreal x);
    void animFinished();


private:
    QScrollBar *hScrollbar_, *vScrollbar_;

    void wheelEvent(QWheelEvent * event);
    int wheel_schedule=10;

    // hash table to query GraphicHex based on ID -> key: graphic_hex_id, value: GraphicHex*
    QHash<int, UI::GraphicHex*> graphicHexList;

    // hash table to query GraphicPawn based on ID -> key: pawn_id, value : GraphicPawn*
    QHash<int, UI::GraphicPawn*> graphicPawnList_;

    // hash table to query GraphicTransport based on ID -> key: transport_id, value : GraphicTransport*
    QHash<int, UI::GraphicTransport*> graphicTransportList_;

    // hash table to query GraphicActor based on ID -> key: actor_id, value : GraphicActor*
    QHash<int, UI::GraphicActor*> graphicActorList_;

    // hash table to keep track of initial pawn vertices
    QHash<QString, int> pawnVertices_;

    // hash table to query pawn color based on player's name
    QHash<QString, QColor> pawnColors_;

    // list of pawn ID of current player, used to emit signal that enable pawn movement
    std::list<int> currentPlayerPawnList_;

    // list of currently movable actors
    std::list<int> movableActors_;

    // list of currently movable transport
    std::list<int> movableTransports_;

    // hash table to query graphical data on each hex -> key: CubeCoorindate in string, value HexData struct
    std::unordered_map<std::string, HexData> dataMap_;

    // variable to keep track of actor ID, used to create new actor when tile is flipped
    int currentActorId_;

    // variable to keep track of transport ID, used to create new transport when tile is flipped
    int currentTransportId_;

    // keep current wheel output
    std::pair<std::string, std::string> wheelOutput_;

    // keep track of hex centers on the board
    std::list<std::pair<std::pair<double, double>, Common::CubeCoordinate>> hexCenters;


    // add pawns to scene
    void addPawn(int pawnId, std::shared_ptr<Common::Pawn> pawnPtr);

    // add animal to scene
    void addActor(int actor_id, std::shared_ptr<Common::Actor> actorPtr);

    // add transport
    void addTransport(int transportId, std::shared_ptr<Common::Transport> transportPtr);

    // add hex to scene
    void addHex(std::shared_ptr<Common::Hex> hexPtr, int id);

    // populate hex board based on game board
    void populate();

    // calculate euclidean distance between two points on 2D plane
    double euclideanDistance(std::pair<double, double> x, std::pair<double, double> y);

    // calculate 2D coorindate of hex object: coordinate indicating top left corner of hex bounding box
    QPointF cubeToHexPos(Common::CubeCoordinate coord);

    // convert cube coordinate to 2D coordinate of hex center
    std::pair<double,double> cubeToHexCenter(Common::CubeCoordinate coord);

    // convert 2D coordinate to cube coordinate
    Common::CubeCoordinate planeToCube(double x, double y);

    // calculate initial pawn position on the scene
    std::pair<double, double> generatePawnPosition(Common::CubeCoordinate coord);

    // get pawn color according to player's name
    QColor getPawnColor(std::string pawnOwner);

    // check if pawn is graphically on transport
    bool isPawnOnTransport(QPointF pawnPos, UI::GraphicTransport* transport);

    // change to next player given current player id
    void changePlayerTurn(int playerId);

    // update if there is eliminated player
    void updateExistingPlayer();

    // perform action of vortex (grahic and backend)
    void doVortexAction(int id);

    // perform action of kraken (graphic and backend)
    void doKrakenAction(int id);

    // perform action of shark (graphic and backend)
    void doSharkAction(int id);

    // perform action of seamunster (graphic and backend)
    void doSeamunsterAction(int id);

    // convert cube coorindate to string as key to hash table
    std::string cubeToString(Common::CubeCoordinate coord);

    // remove graphic data, given the flag
    HexData removeData(std::string key, bool pawn, std::string transportType, std::string actorType);

    // enable current player's pawn movement
    void enablePawnMovement();

    // enable transport movement that has current player's pawns
    void enableTransportMovement();

    // disbale current player's pawn movement
    void disablePawnMovement();

    // disable transport movement
    void disableTransportMovement();

    void changeStage(int stage);

    bool checkActorMovement(Common::CubeCoordinate newPos, int id);

    bool checkTransportMovement(Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos, int id);

    // move graphic pawns on data_map
    void moveGraphicPawn(int pawnId, Common::CubeCoordinate oldPos, Common::CubeCoordinate newPos);

    // move graphic transports on data_map
    void moveGraphicTransport(int transport_id, Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos);

    // move graphic actor on data_map
    void moveGraphicActor(int actor_id, Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos);

};
}


#endif // HEXBOARD_H
