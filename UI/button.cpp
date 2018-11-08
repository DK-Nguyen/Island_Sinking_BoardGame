#include "button.h"
#include <QGraphicsTextItem>
#include <QBrush>

Button::Button(QString name, std::vector<int> vertex, QGraphicsItem* parent)
{
    // draw the rect
    this->vertex = vertex;
    setRect(vertex[0], vertex[1], vertex[2]-vertex[0],vertex[3]-vertex[1]);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::darkCyan);
    setBrush(brush);

    // draw the text
    this->name = name;
    this->button_text = new QGraphicsTextItem(name,this);
    int xPos = rect().width()/2 - this->button_text->boundingRect().width()/2;
    int yPos = rect().height()/2 - this->button_text->boundingRect().height()/2;
    this->button_text->setPos(xPos,yPos);

}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event){
    emit clicked();
}
