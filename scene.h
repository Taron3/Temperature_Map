

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

#include <QDebug>

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {Default, Line, Rectangle, Polygon };

    explicit Scene(QObject *parent = nullptr);

    void setGridSize(int myGridSize);
    int getGridSize() const { return this->m_gridSize; }
    void setLayer(int layer);
    int getLayer() const { return this->m_layer; }
    void setDraw(bool isDraw);

//public slots:
    void setMode(Mode mode);
    void boundingBox(); // adding QGraphicsRectItem for bounding box
    void addRandomRect();  // randomly adding rects in scene
    void addRectFromFile(QString &fileName); // add rects in scene from file
    QString writeNetlist();
    // void drawGrid(const QRectF &rect);

protected:
    void drawBackground (QPainter* painter, const QRectF &rect);

    void mousePressEvent  (QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent   (QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);


private:
    int m_gridSize;
    int m_layer;
    bool m_isDraw;

    Mode myMode;
    void changeReceivedPoint(QPointF &point);
    QList<QRectF> getCells();
    qreal power(QRectF intersectedRect, QRectF rect, int i);
    QList<qreal> getPowers();
    // qreal round(qreal val, int step);

    QGraphicsLineItem* line;
    QGraphicsRectItem* rect;
    QGraphicsLineItem* lineForPolygon;

    bool isFirstPress;

    //QVector<QPointF> points;
};


#endif // SCENE_H
