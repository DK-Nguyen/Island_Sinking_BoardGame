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
#include <illegalmoveexception.hh>
#include <iostream>

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
    h_scrollbar->setMinimumHeight(100);

    v_scrollbar = new QScrollBar(this);
    v_scrollbar->setMinimumWidth(100);

    setHorizontalScrollBar(h_scrollbar);
    setVerticalScrollBar(v_scrollbar);

    // set up the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,160*board_scale,180*board_scale);
    setScene(scene);

    scene->clear();
    populate();
}


void HexBoard::hex_clicked(int id)
// TODO: check when flip result return actor's name -> what's that for?
{
    if (gamestate_ptr->currentGamePhase() != 2)
    {
        return;
    }

    auto graphic_hex = graphic_hex_list[id];
    auto coord = graphic_hex->get_hex()->getCoordinates();
    try {
        auto flip_result = game_engine_ptr->flipTile(coord);

        std::cerr << "finish trying to flip tile \n";
        if (flip_result.compare("") == 0)
        {
            graphic_hex->flip();
        }
    } catch (Common::IllegalMoveException exception) {
        std::cerr << "******EXCEPTION: " << exception.msg() << "\n";
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
void HexBoard::add_hex(std::shared_ptr<Common::Hex> hex_ptr, int id)
{
    // calculate vertex coordinates
    QVector<QPointF> vertex = cube_to_vertex(hex_ptr->getCoordinates());

    // create graphic_hex object
    GraphicHex* graphic_hex = new GraphicHex(hex_ptr, vertex, id);

    // add graphic_hex to scene
    scene->addItem(graphic_hex);

    // add to list of graphic hex
    graphic_hex_list.insert(id, graphic_hex);

    connect(graphic_hex, SIGNAL(clicked(int)), this, SLOT(hex_clicked(int)));
}

// populate main window based on data from game_board, TODO add pawn and actors part
void HexBoard::populate()
{
    int id = 0;
    for (auto& it: gameboard_ptr->hex_list)
    {
        // populate hex
        add_hex(it.second, id);
        id += 1;

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

