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

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(int id);

private:
    int id;
    std::shared_ptr<Common::Hex> hex_ptr;
    QVector<QPointF> vertex;

};


#endif // GRAPHICHEX_H
