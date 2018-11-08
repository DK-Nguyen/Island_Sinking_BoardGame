#include "hexboard.h"
#include <QGraphicsTextItem>
#include <hex.hh>
#include <graphichex.h>
#include <graphicpawn.h>
#include <graphicactor.h>
#include <gameengine.hh>
#include <QVector>
#include <QPointF>

HexBoard::HexBoard(std::shared_ptr<Common::IGameRunner> game_engine_ptr,
                       std::shared_ptr<GameBoard> gameboard_ptr,
                       std::shared_ptr<GameState> gamestate_ptr,
                       std::vector<std::shared_ptr<Common::IPlayer>> game_players,
                       int width,
                       int height,
                       int scale,
                       QWidget* parent)
    : QGraphicsView (parent)

{

    this->game_engine_ptr = game_engine_ptr;
    this->gameboard_ptr = gameboard_ptr;
    this->gamestate_ptr = gamestate_ptr;
    this->game_players = game_players;
    this->scale = scale;
    this->width = width;
    this->height = height;


    // set up the screen
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(width, height);

    // set up the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,160*scale,180*scale);
    setScene(scene);
}

void HexBoard::start()
{
    scene->clear();
    populate();
}

void HexBoard::plot_center_hex()
{
    scene->clear();
    for (auto& it: gameboard_ptr->hex_list)
    {
        // populate hex
        auto coord = it.second->getCoordinates();
        if (coord.x == 0 && coord.y == 0 && coord.z == 0)
        {
            add_hex(it.second);
            break;
        }
    }
}

// add hex to scene
void HexBoard::add_hex(std::shared_ptr<Common::Hex> hex_ptr)
{
    // calculate vertex coordinates
    QVector<QPointF> vertex = cube_to_vertex(hex_ptr->getCoordinates());

    // create graphic_hex object
    GraphicHex* graphic_hex = new GraphicHex(hex_ptr, vertex);

    // add graphic_hex to scene
    scene->addItem(graphic_hex);
}

// populate main window based on data from game_board, TODO add pawn and actors part
void HexBoard::populate()
{

    for (auto& it: gameboard_ptr->hex_list)
    {
        // populate hex
        add_hex(it.second);

        // populate pawn

        // populate actors
    }
}

// convert cube coordinate to 2D coordinate
std::pair<long,long> HexBoard::cube_to_plane(Common::CubeCoordinate coord)
{
    long x, y;
    x = 3*coord.x - 3*coord.y + 67;
    y = 5*coord.x + 5*coord.y + 76.5;
    return std::make_pair(x,y);
}

// convert cube coordinate to hex vertices coordinate
QVector<QPointF> HexBoard::cube_to_vertex(Common::CubeCoordinate coord)
{
    auto center = cube_to_plane(coord);
    QVector<QPointF> vertex;
    vertex << scale*QPointF(center.first, center.second-3.5);
    vertex << scale*QPointF(center.first +3, center.second-1.5);
    vertex << scale*QPointF(center.first +3, center.second+1.5);
    vertex << scale*QPointF(center.first, center.second+3.5);
    vertex << scale*QPointF(center.first -3, center.second+1.5);
    vertex << scale*QPointF(center.first -3, center.second-1.5);

    return vertex;
}

// convert 2D coordinate to cube coordinate, TODO complete code
Common::CubeCoordinate HexBoard::plane_to_cube(int x_pos, int y_pos)
{
    Common::CubeCoordinate coord;
    return coord;
}

