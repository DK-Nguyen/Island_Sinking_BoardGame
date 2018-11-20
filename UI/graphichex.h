#ifndef GRAPHICHEX_H
#define GRAPHICHEX_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>

class GraphicHex: public QObject, public QGraphicsPolygonItem{

    Q_OBJECT

public:

    GraphicHex(std::shared_ptr<Common::Hex> hex_ptr, QVector<QPointF> vertex, int id, QGraphicsItem* parent=nullptr);
    ~GraphicHex();

    // flip
    void flip();

    // set color
    void set_color(std::string type);

    std::shared_ptr<Common::Hex> get_hex();

    QVector<QPointF> get_vertex();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    std::string getName();

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

signals:
    void clicked(int id);

private:
    int id;
    std::shared_ptr<Common::Hex> hex_ptr;
    QVector<QPointF> vertex;

};


#endif // GRAPHICHEX_H
