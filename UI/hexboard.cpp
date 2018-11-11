#include "hexboard.h"
#include <QGraphicsTextItem>
#include <hex.hh>
#include <graphichex.h>
#include <graphicpawn.h>
#include <graphicactor.h>
#include <gameengine.hh>
#include <QVector>
#include <QPointF>
#include <QScrollBar>
#include <QTimeLine>
#include <QWheelEvent>

HexBoard::HexBoard(std::shared_ptr<Common::IGameRunner> game_engine_ptr,
                       std::shared_ptr<GameBoard> gameboard_ptr,
                       std::shared_ptr<GameState> gamestate_ptr,
                       std::vector<std::shared_ptr<Common::IPlayer>> game_players,
                       int width,
                       int height,
                       int board_scale,
                       QWidget* parent)
    : QGraphicsView (parent)

{

    this->game_engine_ptr = game_engine_ptr;
    this->gameboard_ptr = gameboard_ptr;
    this->gamestate_ptr = gamestate_ptr;
    this->game_players = game_players;
    this->board_scale = board_scale;
    this->width = width;
    this->height = height;


    // set up the screen
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(width, height);

    h_scrollbar = new QScrollBar(this);
    h_scrollbar->setMinimumHeight(200);

    v_scrollbar = new QScrollBar(this);
    v_scrollbar->setMinimumWidth(200);

    setHorizontalScrollBar(h_scrollbar);
    setVerticalScrollBar(v_scrollbar);

    // set up the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,160*board_scale,180*board_scale);
    setScene(scene);

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

void HexBoard::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    wheel_schedule += numSteps;
    if (wheel_schedule * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
    wheel_schedule = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}

void HexBoard::scalingTime(qreal x)
{
    qreal factor = 1.0+ qreal(wheel_schedule) / 300.0;
    this->scale(factor, factor);
}

void HexBoard::animFinished()
{
    if (wheel_schedule > 0)
    wheel_schedule--;
    else
    wheel_schedule++;
    sender()->~QObject();
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
    vertex << board_scale*QPointF(center.first, center.second-3.5);
    vertex << board_scale*QPointF(center.first +3, center.second-1.5);
    vertex << board_scale*QPointF(center.first +3, center.second+1.5);
    vertex << board_scale*QPointF(center.first, center.second+3.5);
    vertex << board_scale*QPointF(center.first -3, center.second+1.5);
    vertex << board_scale*QPointF(center.first -3, center.second-1.5);

    return vertex;
}

// convert 2D coordinate to cube coordinate, TODO complete code
Common::CubeCoordinate HexBoard::plane_to_cube(int x_pos, int y_pos)
{
    Common::CubeCoordinate coord;
    return coord;
}

