
#include "scene.h"
#include <QDebug>
#include <QPoint>

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QtMath>

#include "random.h"
#include "data.h"


Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
    , m_gridSize(5)
    , m_layer(0)
{
    Q_ASSERT(m_gridSize > 0);

    line = 0;
    rect = 0;
    lineForPolygon = 0;

    isFirstPress   = true;
}

void Scene::setGridSize(int myGridSize)
{
    m_gridSize = myGridSize;
}

void Scene::setLayer(int layer)
{
    m_layer = layer;
}

void Scene::setMode(Scene::Mode mode)
{
    myMode = mode;
}

void Scene::boundingBox()
{

    QRectF boundingRect = itemsBoundingRect();
    //qDebug() << "boundingBox  "<< boundingRect.left() << "  " << boundingRect.top();
    //qDebug() << "boundingBoxBOOTOM  "<< boundingRect.bottomRight();

    QGraphicsRectItem *rect1 = new QGraphicsRectItem(boundingRect) ;
    //QGraphicsRectItem *rect1 = new QGraphicsRectItem(sceneRect()) ;
    //qDebug() << sceneRect() << "\n";
    rect1->setPen(QPen(Qt::green, 1));
    addItem(rect1);
}

void Scene::addRandomRect()
{
    for(int i = 0; i < 1000; i++)
    {
        //qDebug() << "POWER= " << Random::get(0.05, 0.7) << "\n";
        QList<QGraphicsItem*> itemList = items();

        QRectF randomRect( Random::get(0, 400), Random::get(0, 170), Random::get(10, 300), Random::get(10, 300) );
        rect = new QGraphicsRectItem(randomRect );
        rect->setPen(QPen(Qt::red, 2));
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
            //qDebug() << "==== " << rect->rect() << " ====\n";
        }
    }
    qDebug() << "******************************************************\n";
}

void Scene::addRectFromFile()
{
///////////////////////////////////////////////////////////////////
    QFile file(":/files/rects");                           ////////
      if (!file.open(QIODevice::ReadOnly ))                ////////
          return;                                          ////////
                                                           ////////
    QTextStream inStream(&file);                           ////////
    while (!inStream.atEnd())                              ////////
    {                                                      ////////
        QString line = file.readLine();                    ////////
                                                           ////////
        line = line.simplified();                          ////////
        QStringList parameterList = line.split(';');       ////////
///////////////////////////////////////////////////////////////////
        QList<QGraphicsItem*> itemList = items();  // delete
        // if(m_layer != parser::getLayer() )
            // continue;

        if(Parser::getLayer(parameterList) == m_layer) // check layer
        {
            QRectF dataRect( Parser::getPoint(parameterList),
                             QSize(Parser::getWidth(parameterList), Parser::getHeight(parameterList)) );
            rect = new QGraphicsRectItem(dataRect );
            rect->setPen(QPen(Qt::red, 2));

            bool isIntersected = false;                                                       // delete
            foreach(QGraphicsItem *item, itemList)                                            // delete
            {                                                                                 // delete
                QGraphicsRectItem *rectItem = qgraphicsitem_cast<QGraphicsRectItem*> (item);  // delete
                if(dataRect.intersects(rectItem->rect()) )                                    // delete
                {                                                                             // delete
                    // isIntersected = true;                                                  // delete
                }                                                                             // delete
            }                                                                                 // delete
            if(!isIntersected)                                                                // delete
            {                                                                                 // delete
              addItem(rect);
            }                                                                                 // delete
        }
    }
}


QString Scene::writeNetlist()
{
    QString netlist;
    netlist += "\n";
    netlist += ".prot\n";
    netlist += ".lib '/remote/exchange/synopsys/SAED32_PDK/hspice/saed32nm.lib' TT\n";
    netlist += ".unprot\n\n";
    netlist += ".temp 25\n";
    netlist += "vvdd vdd gnd dc = 1.2\n";
    netlist += "vvss vss gnd dc = 0.0\n";
    netlist += "***********************************************************\n";
    netlist += ".param Rij = 180.33  \n";
    netlist += ".param rij = 1490.22 \n";
    netlist += ".param Ri = 28.3739  \n";
    netlist += ".param Rsub = 3.43348\n\n";

    netlist += "****************  LAYER " + QVariant(m_layer).toString() + " ***************\n\n";

    QList<qreal> powerList = getPowers();
    //QList<QRectF> cellList = getCells();

    int row = (itemsBoundingRect().height() / m_gridSize) + 1;
    int column = (itemsBoundingRect().width() / m_gridSize) + 1;
    //qDebug() << "ROW = "  << row << " COLUMN= " << column << "\n";
    for(int i = 0; i < row ; ++i)
    {
        for(int j = 0; j < column; ++j)
        {
            QString istr = QVariant(i).toString();
            QString jstr = QVariant(j).toString();
            QString kstr = QVariant(m_layer).toString();

            netlist += "\n**** CELL " + istr + "_" + jstr + "_" + kstr + " ****\n";

            // .param i_i_j_layer =
            netlist += ".param i_" + istr + "_" + jstr + "_" + kstr
                    +  " = " + QVariant(powerList[column * i + j]).toString() + "\n";

            // ii_j_layer vdd _c_i_j_layer dc = i_0_1_0 ac = 0
            netlist += "i"         + istr + "_" + jstr + "_" + kstr
                    +  " vdd _c_"  + istr + "_" + jstr + "_" + kstr
                    +  " dc = i_"  + istr + "_" + jstr + "_" + kstr
                    + " ac = 0\n\n";

            // rR_i_j_layer  _c_i_j_layer     _c_i_j_layer_s   R=Ri
            netlist += "rR_" + istr + "_" + jstr + "_" + kstr
                    + " _c_" + istr + "_" + jstr + "_" + kstr
                    + " _c_" + istr + "_" + jstr + "_" + kstr + "_s "
                    + "R=Ri\n";

            // rRs_i_j_layer  _c_i_j_layer_s  vss  R=Rsub
            netlist += "rRs_" + istr + "_" + jstr + "_" + kstr
                    +  " _c_" + istr + "_" + jstr + "_" + kstr + "_s "
                    +  "vss R=Rsub\n";
            if(i + 1 < row)
            {
                // rR_i_j_layer_i+1_j_layer     _c_i_j_layer     _c_i+1_j_layer     R=Rij
                netlist += "rR_" + istr + "_" + jstr + "_" + kstr
                        +  "_" + QVariant(i + 1).toString() + "_" + jstr + "_" + kstr
                        +  " _c_" + istr + "_" + jstr + "_" + kstr
                        +  " _c_" + QVariant(i + 1).toString() + "_" + jstr + "_" + kstr
                        +  " R=Rij\n";

                // rr_s_i_j_layer_i+1_j_layer   _c_i_j_layer_s   _c_i+1_j_layer_s   R=rij
                netlist += "rr_s_" + istr + "_" + jstr + "_" + kstr
                        + "_" + QVariant(i + 1).toString() + "_" + jstr + "_" + kstr
                        + " _c_" + istr + "_" + jstr + "_" + kstr + "_s"
                        + " _c_" + QVariant(i + 1).toString() + "_" + jstr + "_" + kstr + "_s"
                        + " R=rij\n";
            }
            if(j + 1 < column)
            {
                // rR_i_j_layer_i_j+1_layer     _c_i_j_layer     _c_i_j+1_layer     R=Rij
                netlist += "rR_" + istr + "_" + jstr + "_" + kstr
                        + "_" + istr + "_" + QVariant(j + 1).toString() + "_" + kstr
                        + " _c_" + istr + "_" + jstr + "_" + kstr
                        + " _c_" + istr + "_" + QVariant(j + 1).toString() + "_" + kstr
                        + " R=Rij\n";

                // rr_s_i_j_layer_i_j+1_layer   _c_i_j_layer_s   _c_i_j+1_layer_s   R=rij
                netlist += "rr_s_" + istr + "_" + jstr + "_" + kstr
                        + "_" + istr + "_" + QVariant(j + 1).toString() + "_" + kstr
                        + " _c_" + istr + "_" + jstr + "_" + kstr + "_s"
                        + " _c_" + istr + "_" + QVariant(j + 1).toString() + "_" + kstr + "_s"
                        + " R=rij\n";
            }
        }
    }

    netlist += "\n\n.option post probe\n\n";
    netlist += ".global gnd\n";
    netlist += ".probe v(*) i(*)\n";
    netlist += ".tran 10p 5n\n\n";
    netlist += ".end";

     // qDebug() << "DEBUG_START CellSIZE= " << powerList;

//*******************************************************************************************************************
//*******************************************************************************************************************
   /* QList<QGraphicsItem*> itemList = items(Qt::AscendingOrder);
    foreach(QGraphicsItem *item, itemList)
    {
        QGraphicsRectItem *rectItem = qgraphicsitem_cast<QGraphicsRectItem*> (item);
        qDebug() << "RECT = " << rectItem->rect() << "\n";
    }
    */
    return netlist;
}

void Scene::drawGrid(const QRectF &rect)
{
    //qreal left = int(rect.left()) - (int(rect.left()) % m_gridSize);
    //qreal top  = int(rect.top())  - (int(rect.top())  % m_gridSize);

    qreal left = rect.left();
    qreal top  = rect.top();
    qDebug() << "drawGrid RECT  " << rect << "\n";
    qDebug() << "\nLEFT " << left << "top " << top << "\n" ;
    qDebug() << "\nwww " << rect.width() << "hhh " << rect.height() << "\n" ;
    for (qreal x = left; x <= rect.width() ; x += m_gridSize)  // | | |
    {
        addLine(x, top, x, top + rect.height(), QPen(Qt::white));
    }
    for (qreal y = top; y <= rect.height(); y += m_gridSize)  // -- -- --
    {
        addLine(left, y , left + rect.width() , y, QPen(Qt::white)) ;
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //if (mouseEvent->button() != Qt::LeftButton)
      //  return;

    switch (myMode)
    {
        case Default:
        {
        qDebug() << "MOUSE  " << mouseEvent->scenePos() << "  MOUSE\n";
            // double  rad = 4;
        //
        //    QPointF point = mouseEvent->scenePos();
        //    changeReceivedPoint(point);
        //    addEllipse( point.x() /* - rad */, point.y() /* - rad */, /* rad  * */ 5.0, /* rad * */ 5.0,
        //    QPen(Qt::white), QBrush(Qt::NoBrush) );
        //    break;

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

    if( static_cast<int>(qFabs(point.x()) ) % m_gridSize < m_gridSize / 2)
    {
        point.setX( (static_cast<int>(point.x()) / m_gridSize) * m_gridSize ) ;
    }
    else
    {
        point.setX( (static_cast<int>(point.x()) / m_gridSize + xSign) * m_gridSize ) ;
    }

    if( static_cast<int>(qFabs(point.y()) )  % m_gridSize < m_gridSize / 2)
    {
        point.setY( static_cast<int>(point.y()) / m_gridSize * m_gridSize) ;
    }
    else
    {
        point.setY( (static_cast<int>(point.y()) / m_gridSize + ySign) * m_gridSize ) ;
    }
}

QList<QRectF> Scene::getCells()
{
    // get cells
    QRectF br = itemsBoundingRect();
    QList<QRectF> cellList;
    for(qreal y = br.y(); y <= br.height(); y += m_gridSize )
    {
        for(qreal x = br.x(); x <= br.width(); x += m_gridSize)
        {
            cellList.append(QRectF(x, y, m_gridSize, m_gridSize) );
        }
    }
    qDebug() << "GRIDSIZE " << m_gridSize << "\n";
    return cellList;
}

qreal Scene::power(QRectF intersectedRect, QRectF rect, int i)
{
    QFile file(":/files/rects");
      if (!file.open(QIODevice::ReadOnly ))
          return - 1;


    qreal rectPower = 0;
    int rectNumber = 0;
    QTextStream inStream(&file);

    while (!inStream.atEnd())
    {
        //qDebug() << "WHILE_IN_power()" << "\n";
        QString line = file.readLine();

        line = line.simplified();
        QStringList parameterList = line.split(';');

        if(rectNumber == i)
        {
            //qDebug() << "IF_IN_power()" << "\n";
            rectPower = Parser::getPower(parameterList);
            // break;
        }
        ++rectNumber;
    }
    qreal power = ( intersectedRect.width() * intersectedRect.height() )
               /  (rect.width() * rect.height() )
               * rectPower;

    return power;
}

QList<qreal> Scene::getPowers()
{
    // get Cells
    QList<QRectF> cellList = getCells();

    // powers for each cell
    QList<qreal> cellPowerList;
    // get rects
    QList<QGraphicsItem*> sceneItemList = items(Qt::AscendingOrder);

    foreach(QRectF cell, cellList)
    {
        qreal cellPower = 0; // if don't intersected, it's 0
        for(int i = 0; i < sceneItemList.size(); ++i)
        {
            QGraphicsRectItem *rectItem = qgraphicsitem_cast<QGraphicsRectItem*> (sceneItemList[i]);
            if(cell.intersects(rectItem->rect()))
            {
                cellPower += power(cell.intersected(rectItem->rect()), rectItem->rect(), i);
            }
        }
        cellPowerList.append(cellPower);
    }
    return cellPowerList;
}
