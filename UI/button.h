#ifndef BUTTON_H
#define BUTTON_H
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QPointF>

class Button: public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    // constructor
    Button(QString name, std::vector<int> vertex, QGraphicsItem* parent=nullptr);

    // event handler
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    QString name;
    std::vector<int> vertex;

signals:
    void clicked();
private:
    QGraphicsTextItem* button_text;
};

#endif // BUTTON_H
