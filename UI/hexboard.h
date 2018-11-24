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
    std::list<GraphicActor*> actors;
    std::list<GraphicTransport*> transports;
    std::list<GraphicPawn*> pawns;
};


class HexBoard: public QGraphicsView{
    Q_OBJECT
public:
    // constructors
    HexBoard(std::shared_ptr<Common::IGameRunner> game_engine,
             std::shared_ptr<Student::GameBoard> game_board,
             std::shared_ptr<GameState> game_state,
             std::vector<std::shared_ptr<Common::IPlayer>> players,
             int width,
             int height,
             int board_scale,
             QWidget* parent=nullptr);

    // container to hold items
    QGraphicsScene* scene;

    std::shared_ptr<Common::IGameRunner> game_engine;
    std::shared_ptr<Student::GameBoard> game_board;
    std::shared_ptr<GameState> game_state;
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    int board_scale, width, height;

    void clear();

signals:
    void set_pawn_movement(bool allowed, std::list<int> pawn_id);
    void set_actor_movement(bool allowed, std::list<int> actor_id);
    void set_transport_movement(bool allowed);
    void set_wheel_click(bool flag);
    void update_stage();
    void update_movement_left();
    void update_player_turn();
    void update_point(std::vector<int> IDs, std::vector<int> increment);
    void game_over();


public slots:
    // receive click signal from GraphicHex object and perform hex flipping if valid
    void hex_clicked(int id);

    // receive click signal from wheel
    void wheel_clicked(std::pair<std::string, std::string> wheel_output);

    // receive movement signal from GraphicPawn object and check if move is valid and act accordingly
    void pawn_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos);

    // receive movement signal from GraphicActor object and check if move is valid and act accordingly
    void actor_is_moved(int actor_id, QPointF old_pos, QPointF new_pos);

    // receive movement signal from GraphicTransport object and check if move is valid and act accordingly
    void transport_is_moved(int transport_id, QPointF old_pos, QPointF new_pos);


private slots:
    // functionality to perform zooming
    void scalingTime(qreal x);
    void animFinished();


private:
    QScrollBar *h_scrollbar, *v_scrollbar;

    void wheelEvent(QWheelEvent * event);
    int wheel_schedule=10;

    // hash table to query GraphicHex based on ID -> key: graphic_hex_id, value: GraphicHex*
    QHash<int, GraphicHex*> graphic_hex_list;

    // hash table to query GraphicPawn based on ID -> key: pawn_id, value : GraphicPawn*
    QHash<int, GraphicPawn*> graphic_pawn_list;

    // hash table to query GraphicTransport based on ID -> key: transport_id, value : GraphicTransport*
    QHash<int, GraphicTransport*> graphic_transport_list;

    // hash table to query GraphicActor based on ID -> key: actor_id, value : GraphicActor*
    QHash<int, GraphicActor*> graphic_actor_list;

    // hash table to keep track of initial pawn vertices
    QHash<QString, int> pawn_vertices;

    // hash table to query pawn color based on player's name
    QHash<QString, QColor> pawn_colors;

    // list of pawn ID of current player, used to emit signal that enable pawn movement
    std::list<int> current_player_pawn_list;

    // list of currently movable actors
    std::list<int> movable_actors;

    // hash table to query graphical data on each hex -> key: CubeCoorindate in string, value HexData struct
    std::unordered_map<std::string, HexData> data_map;

    // variable to keep track of actor ID, used to create new actor when tile is flipped
    int current_actor_id;

    // variable to keep track of transport ID, used to create new transport when tile is flipped
    int current_transport_id;

    // keep current wheel output
    std::pair<std::string, std::string> wheel_output_;


    // add pawns to scene
    void add_pawn(int pawn_id, std::shared_ptr<Common::Pawn> pawn_ptr);

    // add animal to scene
    void add_actor(int actor_id, std::shared_ptr<Common::Actor> actor_ptr);

    // add transport
    void add_transport(int transport_id, std::shared_ptr<Common::Transport> transport_ptr);

    // add hex to scene
    void add_hex(std::shared_ptr<Common::Hex> hex_ptr, int id);

    // populate hex board based on game board
    void populate();

    // calculate euclidean distance between two points on 2D plane
    double euclidean_distance(std::pair<double, double> x, std::pair<double, double> y);

    // calculate 2D coorindate of hex object: coordinate indicating top left corner of hex bounding box
    QPointF cube_to_hex_pos(Common::CubeCoordinate coord);

    // convert cube coordinate to 2D coordinate of hex center
    std::pair<double,double> cube_to_hex_center(Common::CubeCoordinate coord);

    // convert 2D coordinate to cube coordinate
    Common::CubeCoordinate plane_to_cube(double x, double y);

    // calculate initial pawn position on the scene
    std::pair<double, double> generate_pawn_position(Common::CubeCoordinate coord);

    // get pawn color according to player's name
    QColor get_pawn_color(std::string pawn_owner);

    // check if pawn is graphically on transport
    bool is_pawn_on_transport(QPointF pawn_pos, GraphicTransport* transport);

    // change to next player given current player id
    void change_player_turn(int player_id);

    // update if there is eliminated player
    void update_existing_player();

    // perform action of vortex (grahic and backend)
    void do_vortex_action(int id);

    // perform action of kraken (graphic and backend)
    void do_kraken_action(int id);

    // perform action of shark (graphic and backend)
    void do_shark_action(int id);

    // perform action of seamunster (graphic and backend)
    void do_seamunster_action(int id);

    // convert cube coorindate to string as key to hash table
    std::string cube_to_string(Common::CubeCoordinate coord);

    // remove graphic data, given the flag
    void remove_data(HexData& data, bool pawn, std::string transport_type, std::string actor_type);

    // enable current player's pawn movement
    void enable_pawn_movement();

    // disbale current player's pawn movement
    void disable_pawn_movement();

    void change_stage(int stage);

    void print_cube(Common::CubeCoordinate coord);

};



#endif // HEXBOARD_H
