

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include <QApplication>

#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QPolygonF>

#include <QVector>

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {Default, Line, Rectangle, Polygon };

    explicit Scene(QObject *parent = 0);

    int getGridSize() const { return this->gridSize; }

//public slots:
    void setMode(Mode mode);
    void boundingBox(); // adding QGraphicsRectItem for bounding box
    void addRandomRect();  // randomly adding rects in scene

protected:
    void drawBackground (QPainter* painter, const QRectF &rect);

    void mousePressEvent  (QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent   (QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);


private:
    int gridSize;

    Mode myMode;
    void changeReceivedPoint(QPointF &point);

    QGraphicsLineItem* line;
    QGraphicsRectItem* rect;
    QGraphicsLineItem* lineForPolygon;

    bool isFirstPress;

    //QVector<QPointF> points;
};


#endif // SCENE_H
