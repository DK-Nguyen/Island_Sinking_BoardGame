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
    // receive click signal from hex object and perform hex flipping if valid
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
    void scalingTime(qreal x);
    void animFinished();


private:
    QScrollBar *h_scrollbar, *v_scrollbar;

    void wheelEvent(QWheelEvent * event);
    int wheel_schedule=10;

    // key: graphic_hex_id, value: GraphicHex*
    QHash<int, GraphicHex*> graphic_hex_list;

    // key: pawn_id, value : GraphicPawn*
    QHash<int, GraphicPawn*> graphic_pawn_list;

    // key: transport_id, value : GraphicTransport*
    QHash<int, GraphicTransport*> graphic_transport_list;

    // key: actor_id, value : GraphicActor*
    QHash<int, GraphicActor*> graphic_actor_list;

    QHash<QString, int> pawn_vertices;

    QHash<QString, QColor> pawn_colors;
    std::list<int> current_player_pawn_list;

    std::unordered_map<std::string, HexData> data_map;

    int current_actor_id;
    int current_transport_id;
    std::pair<std::string, std::string> wheel_output_;


    // add pawns to scene
    void add_pawn(int pawn_id, std::shared_ptr<Common::Pawn> pawn_ptr);

    // add animal to scene
    void add_actor(int actor_id, std::shared_ptr<Common::Actor> actor_ptr);

    // add transport
    void add_transport(int transport_id, std::shared_ptr<Common::Transport> transport_ptr);

    // add hex to scene
    void add_hex(std::shared_ptr<Common::Hex> hex_ptr, int id);

    // populate main window based on data from game_board
    void populate();

    double euclidean_distance(std::pair<double, double> x, std::pair<double, double> y);

    QPointF cube_to_hex_pos(Common::CubeCoordinate coord);

    // convert cube coordinate to 2D coordinate
    std::pair<double,double> cube_to_hex_center(Common::CubeCoordinate coord);

    // convert 2D coordinate to cube coordinate
    Common::CubeCoordinate plane_to_cube(double x, double y);

    std::pair<double, double> generate_pawn_position(Common::CubeCoordinate coord);

    QColor get_pawn_color(std::string pawn_owner);

    bool is_pawn_on_transport(QPointF pawn_pos, GraphicTransport* transport);

    void change_player_turn(int player_id);
    void update_existing_player();

    void do_vortex_action(int id);
    void do_kraken_action(int id);
    void do_shark_action(int id);
    void do_seamunster_action(int id);


    std::string cube_to_string(Common::CubeCoordinate coord);

    void remove_data(HexData& data, bool pawn, bool transport, bool actor);

    void enable_pawn_movement();

    void disable_pawn_movement();

};



#endif // HEXBOARD_H
