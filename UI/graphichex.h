#ifndef GRAPHICHEX_H
#define GRAPHICHEX_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>


namespace UI{
class GraphicHex: public QObject, public QGraphicsPolygonItem{

    Q_OBJECT

public:

    // initialize with Common::Hex
    GraphicHex(std::shared_ptr<Common::Hex> hexPtr, QVector<QPointF> vertex, int id_, QGraphicsItem* parent=nullptr);
    ~GraphicHex();

    // perform flipping: change color
    void flip();

    // get Common::Hex
    std::shared_ptr<Common::Hex> getHex();

    // get Polygon vertices
    QVector<QPointF> getVertex();



protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

signals:

    // emit signal to hexBoard when clicked
    void clicked(int id_);

private:
    int id_;
    std::shared_ptr<Common::Hex> hexPtr_;
    QVector<QPointF> vertex;

    // set color
    void setColor(std::string type);

};
}

#endif // GRAPHICHEX_H
