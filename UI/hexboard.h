#ifndef HEXBOARD_H
#define HEXBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <hex.hh>
#include <graphichex.h>
#include <graphicpawn.h>
#include <graphicactor.h>
#include <gameengine.hh>
#include <QVector>
#include <QPointF>
#include <gameboard.hh>
#include <gamestate.hh>
#include <player.hh>
#include <QColor>



class HexBoard: public QGraphicsView{
    Q_OBJECT
public:
    // constructors
    HexBoard(std::shared_ptr<Common::IGameRunner> game_engine,
             std::shared_ptr<GameBoard> game_board,
             std::shared_ptr<GameState> game_state,
             std::vector<std::shared_ptr<Common::IPlayer>> players,
             int width,
             int height,
             int board_scale,
             QWidget* parent=nullptr);

    // container to hold items
    QGraphicsScene* scene;

    std::shared_ptr<Common::IGameRunner> game_engine;
    std::shared_ptr<GameBoard> game_board;
    std::shared_ptr<GameState> game_state;
    std::vector<std::shared_ptr<Common::IPlayer>> players;
    int board_scale, width, height;


public slots:
    // receive click signal from hex object and perform hex flipping if valid
    void hex_clicked(int id);

    // receive movement signal from GraphicPawn object and check if move is valid and act accordingly
    void pawn_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos);

    // receive movement signal from GraphicActor object and check if move is valid and act accordingly
    void actor_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos);

    // receive movement signal from GraphicTransport object and check if move is valid and act accordingly
    void transport_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos);


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

    QHash<QString, int> pawn_vertices;

    QHash<QString, QColor> pawn_colors;


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

    // convert cube coordinate to hex vertices coordinate
    QVector<QPointF> cube_to_hex_vertex(Common::CubeCoordinate coord);

    // convert cube coordinate to 2D coordinate
    std::pair<double,double> cube_to_hex_center(Common::CubeCoordinate coord);

    // convert 2D coordinate to cube coordinate
    Common::CubeCoordinate plane_to_cube(int x_pos, int y_pos);

    std::pair<double, double> generate_pawn_position(Common::CubeCoordinate coord);

    QColor get_pawn_color(std::string pawn_owner);

};



#endif // HEXBOARD_H
