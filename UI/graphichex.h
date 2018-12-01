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

    GraphicHex(std::shared_ptr<Common::Hex> hexPtr, QVector<QPointF> vertex, int id_, QGraphicsItem* parent=nullptr);
    ~GraphicHex();

    // flip
    void flip();

    // set color
    void setColor(std::string type);

    std::shared_ptr<Common::Hex> getHex();

    QVector<QPointF> getVertex();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

signals:
    void clicked(int id_);

private:
    int id_;
    std::shared_ptr<Common::Hex> hexPtr_;
    QVector<QPointF> vertex;

};
}

#endif // GRAPHICHEX_H
