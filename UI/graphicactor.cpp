#include <graphicactor.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


UI::GraphicActor::GraphicActor(std::shared_ptr<Common::Actor> actorPtr, QGraphicsItem* parent)
    : QGraphicsPixmapItem (parent)
{

    this->actorPtr_ = actorPtr;
    this->parent = parent;
    actorType_ = actorPtr->getActorType();
    movementAllowed_ = false;

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);


    if (actorType_.compare("kraken")==0)
    {
        image = QPixmap(":/Images/kraken.png");
    }
    else if (actorType_.compare("seamunster")==0)
    {
        image = QPixmap(":/Images/seamunster.png");
    }
    else if (actorType_.compare("shark")==0)
    {
        image = QPixmap(":/Images/shark.png");
    }
    else
    {
        image = QPixmap(":/Images/vortex.png");
    }

    setPixmap(image);

    setToolTip(QString::fromUtf8(actorType_.c_str()));
}

std::shared_ptr<Common::Actor> UI::GraphicActor::getActor()
{
    return actorPtr_;
}

void UI::GraphicActor::allowMovement(bool allowed, std::list<int> actorId)
{
    for (auto id : actorId)
    {
        if (id == actorPtr_->getId())
        {
            movementAllowed_ = allowed;

            if (allowed)
            {
                setFlags(QGraphicsItem::ItemIsSelectable|
                         QGraphicsItem::ItemIsMovable);
            }
            else
            {
                setFlag(QGraphicsItem::ItemIsSelectable, false);
                setFlag(QGraphicsItem::ItemIsMovable, false);
            }

            break;
        }
    }
}

void UI::GraphicActor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void UI::GraphicActor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void UI::GraphicActor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    QPointF new_pos = this->scenePos();
    emit actorIsMoved(actorPtr_->getId(), old_pos, new_pos);
}
