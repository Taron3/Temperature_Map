

#include "scene.h"
#include <QDebug>
#include <QPoint>

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QtMath>

#include "random.h"

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
    , gridSize(25)
{
    Q_ASSERT(gridSize > 0);

    line = 0;
    rect = 0;
    lineForPolygon = 0;

    isFirstPress   = true;
}

void Scene::setMode(Scene::Mode mode)
{
    myMode = mode;
}

void Scene::boundingBox()
{
    QGraphicsRectItem *rect1 = new QGraphicsRectItem(itemsBoundingRect()) ;
    rect1->setPen(QPen(Qt::green, 1));
    addItem(rect1);
}

void Scene::addRandomRect()
{
    for(int i = 0; i < 1000; i++)
    {
        QList<QGraphicsItem*> itemList = items();

        QRectF randomRect( Random::get(-50, 200), Random::get(-50, 200) ,Random::get(5, 300), Random::get(5, 300) );
        rect = new QGraphicsRectItem(randomRect );
        rect->setPen(QPen(Qt::red, 1));

        bool isIntersected = false;
        foreach(QGraphicsItem *item, itemList)
        {
            QGraphicsRectItem *rectItem = qgraphicsitem_cast<QGraphicsRectItem*> (item);
            if(randomRect.intersects(rectItem->rect()) )
            {
                isIntersected = true;
            }
        }
        if(!isIntersected)
        {
            addItem(rect);
        }
    }
////
    QList<QGraphicsItem*> itemList = items();
    qDebug() <<  itemList.size();
////
}

void Scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top  = int(rect.top())  - (int(rect.top())  % gridSize);

    QVector<QPointF> points;
    for (qreal x = left; x < rect.right(); x += gridSize)
    {
        for (qreal y = top; y < rect.bottom(); y += gridSize)
        {
            points.append(QPointF(x,y) );
        }
    }
    painter->setPen(Qt::white);
    painter->fillRect(rect, Qt::black);
    painter->drawPoints(points.data(), points.size() );
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //if (mouseEvent->button() != Qt::LeftButton)
      //  return;

    switch (myMode)
    {
        case Default:
        {
            double  rad = 4;
            QPointF point = mouseEvent->scenePos();

            changeReceivedPoint(point);
            addEllipse( point.x() - rad, point.y() - rad, rad * 2.0, rad * 2.0,
            QPen(Qt::white), QBrush(Qt::NoBrush) );
            break;
        }
        case Line:
        {
            QPointF startPoint = mouseEvent->scenePos();
            changeReceivedPoint(startPoint);

            line = new QGraphicsLineItem(QLineF(startPoint, startPoint)) ;
            line->setPen(QPen(Qt::white, 1));
            addItem(line);
            break;
        }
        case Rectangle:
        {
            QPointF startPoint = mouseEvent->scenePos();
            changeReceivedPoint(startPoint);

            rect = new QGraphicsRectItem(QRectF(startPoint, startPoint)) ;
            rect->setPen(QPen(Qt::white, 1));
            addItem(rect);
            break;
        }
        case Polygon:
        {
            static QPointF startPoint1;

            if( mouseEvent->button() == Qt::LeftButton)
            {
                if(isFirstPress)
                {
                    qDebug() << Q_FUNC_INFO << "Press111" ;
                    startPoint1 = mouseEvent->scenePos();
                    changeReceivedPoint(startPoint1);

                    lineForPolygon = new QGraphicsLineItem(QLineF(startPoint1, startPoint1)) ;
                    lineForPolygon->setPen(QPen(Qt::white, 1));
                    addItem(lineForPolygon);
                    isFirstPress = false;
                }
                else
                {
                    qDebug() << Q_FUNC_INFO << "Press222"  ;
                    QPointF startPoint = lineForPolygon->line().p2();

                    lineForPolygon = new QGraphicsLineItem(QLineF(startPoint, startPoint)) ;
                    lineForPolygon->setPen(QPen(Qt::white, 1));
                    addItem(lineForPolygon);
                }
            }
            else if( mouseEvent->button() == Qt::RightButton)
            {
                qDebug() << Q_FUNC_INFO << "PressRIGHT"  ;
                QPointF startPoint = lineForPolygon->line().p2();

                lineForPolygon = new QGraphicsLineItem(QLineF(startPoint, startPoint1)) ;
                lineForPolygon->setPen(QPen(Qt::white, 1));
                addItem(lineForPolygon);
            }

            break;
        }
        default:
        ;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == Line && line != 0)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QLineF newLine(line->line().p1(), endPoint);
        line->setLine(newLine);
    }
    else if(myMode == Rectangle && rect != 0)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        /*                       if(rect->rect().topLeft().x() > endPoint.x()
                                        && rect->rect().topLeft().y() > endPoint.y())

                                    std::swap(static_cast<int> (rect->rect().topLeft().x() ), static_cast<int> (endPoint.x() ));
                                    std::swap(static_cast<int> (rect->rect().topLeft().y() ), static_cast<int> (endPoint.y() ));
        */
        QRectF newRect(rect->rect().topLeft(), endPoint);
        rect->setRect(newRect);
    }
    else if(myMode == Polygon && lineForPolygon != 0)
    {
         qDebug() << Q_FUNC_INFO << "Move" ;
         QPointF endPoint = mouseEvent->scenePos();
         changeReceivedPoint(endPoint);

         QLineF newlineForPolygon(lineForPolygon->line().p1(), endPoint);
         lineForPolygon->setLine(newlineForPolygon);
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == Line && line != 0)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QLineF newLine(line->line().p1(), endPoint);
        line->setLine(newLine);
    }
    else if(myMode == Rectangle && rect != 0)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QRectF newRect(rect->rect().topLeft(), endPoint);
        rect->setRect(newRect);
    }
    else if(myMode == Polygon && lineForPolygon != 0)
    {
        if( mouseEvent->button() == Qt::LeftButton)
        {
            qDebug() << Q_FUNC_INFO << "Release";
            QPointF endPoint = mouseEvent->scenePos();
            changeReceivedPoint(endPoint);

            QLineF newlineForPolygon(lineForPolygon->line().p1(), endPoint);
            lineForPolygon->setLine(newlineForPolygon);
        }
    }

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void Scene::changeReceivedPoint(QPointF &point)
{
    int xSign = 1;
    int ySign = 1;
    if(point.x() < 0)
    {
        xSign = -1;
    }
    if(point.y() < 0)
    {
        ySign = -1;
    }

    if( static_cast<int>(qFabs(point.x()) ) % gridSize < gridSize / 2)
    {
        point.setX( (static_cast<int>(point.x()) / gridSize) * gridSize ) ;
    }
    else
    {
        point.setX( (static_cast<int>(point.x()) / gridSize + xSign) * gridSize ) ;
    }

    if( static_cast<int>(qFabs(point.y()) )  % gridSize < gridSize / 2)
    {
        point.setY( static_cast<int>(point.y()) / gridSize * gridSize) ;
    }
    else
    {
        point.setY( (static_cast<int>(point.y()) / gridSize + ySign) * gridSize ) ;
    }
}
