
#include "scene.h"
#include "data.h"

#include <QDebug>
#include <QPoint>

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QtMath>
#include <QMessageBox>
#include "QRandomGenerator"



Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
    , m_gridSize(35)
    , m_layer(0)
    , m_isDraw(false)
{
    line = nullptr;
    rect = nullptr;
    lineForPolygon = nullptr;

    isFirstPress   = true;
}

void Scene::setDataFileName(QString fileName)
{
    m_dataFileName = fileName;
}

void Scene::setGridSize(int myGridSize)
{
    m_gridSize = myGridSize;
}

void Scene::setLayer(int layer)
{
    m_layer = layer;
}

void Scene::setDraw(bool isDraw)
{
    m_isDraw = isDraw;
    this->update();
}

void Scene::setMode(Scene::Mode mode)
{
    myMode = mode;
}

void Scene::boundingBox()
{

    QRectF boundingRect = itemsBoundingRect();

    QGraphicsRectItem *rect1 = new QGraphicsRectItem(boundingRect) ;
    //QGraphicsRectItem *rect1 = new QGraphicsRectItem(sceneRect()) ;

    rect1->setPen(QPen(Qt::green, 1));
    addItem(rect1);
}

void Scene::addRandomRect()
{
    for(int i = 0; i < 1000; i++)
    {
        QList<QGraphicsItem*> itemList = items();
        QRectF randomRect( QRandomGenerator::global()->bounded(0, 400),
                           QRandomGenerator::global()->bounded(0, 170),
                           QRandomGenerator::global()->bounded(10, 300),
                           QRandomGenerator::global()->bounded(10, 300) );

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
        }
    }
}

void Scene::addElementsFromFile()
{
    QFile dataFile(m_dataFileName);
    if (!dataFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("The file cannot opened.");
        msgBox.exec();
        return;
    }

    QTextStream inStream(&dataFile);
    while (!inStream.atEnd())
    {
        QString line = dataFile.readLine();
        line = line.simplified();

        QStringList parameterList = line.split(';');
        // if(m_layer != parser::getLayer() )
            // continue;

        if(Parser::getLayer(parameterList) == m_layer) // check layer
        {
            QRectF dataRect( Parser::getPoint(parameterList),
                             QSize(Parser::getWidth(parameterList), Parser::getHeight(parameterList)) );
            rect = new QGraphicsRectItem(dataRect );
            rect->setPen(QPen(Qt::cyan, 1.5));
            addItem(rect);
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

    int row = (itemsBoundingRect().height() / m_gridSize) + 1;
    int column = (itemsBoundingRect().width() / m_gridSize) + 1;

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

    return netlist;
}
/*
void Scene::drawGrid(const QRectF &rect)
{
    qreal left = rect.left();
    qreal top  = rect.top();

    for (qreal x = left; x <= rect.width() ; x += m_gridSize)  // | | |
    {
        addLine(x, top, x, top + rect.height(), QPen(Qt::black));
    }
    for (qreal y = top; y <= rect.height(); y += m_gridSize)  // -- -- --
    {
        addLine(left, y , left + rect.width() , y, QPen(Qt::black)) ;
    }
}
*/
void Scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // setBackgroundBrush(Qt::black);  // don't work with QFileDialog::getOpenFileName( ... ) in Ubuntu
        painter->fillRect(rect, QColor(63, 60, 57 )); // more darkGray

/*
    // draw grid points
    painter->fillRect(rect, Qt::black);

    QPen pen(Qt::white, 1);
        painter->setPen(pen);

        qreal left = int(rect.left()) - (int(rect.left()) % m_gridSize);
        qreal top = int(rect.top()) - (int(rect.top()) % m_gridSize);
        QVector<QPointF> points;
        for (qreal x = left; x < rect.right(); x += m_gridSize)
        {
            for (qreal y = top; y < rect.bottom(); y += m_gridSize)
            {
                points.append(QPointF(x,y));
            }
        }
        painter->drawPoints(points.data(), points.size());
*/
    // draw grid
    if(m_isDraw)
    {
        int step = m_gridSize;
        painter->setPen(QPen(QColor(Qt::white)));

        // horizontal
        qreal start = int(rect.top()) - (int(rect.top()) % step);
        if (start > rect.top())
        {
            start -= step;
        }
        for (qreal y = start - step; y < rect.bottom(); )
        {
          y += step;
          painter->drawLine(rect.left(), y, rect.right(), y);
        }

        // vertical
        start = int(rect.left()) - (int(rect.left()) % step);
        if (start > rect.left())
        {
            start -= step;
        }
        for (qreal x = start - step; x < rect.right(); )
        {
          x += step;
          painter->drawLine(x, rect.top(), x, rect.bottom());
        }
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    switch (myMode)
    {
        case Default:
        {
            double  rad = 4;

            QPointF point = mouseEvent->scenePos();
            changeReceivedPoint(point);
            addEllipse( point.x()  - rad , point.y()  - rad ,  rad  *  5.0,  rad * 5.0,
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
            QPointF startPoint1;

            if( mouseEvent->button() == Qt::LeftButton)
            {
                if(isFirstPress)
                {
                    startPoint1 = mouseEvent->scenePos();
                    changeReceivedPoint(startPoint1);

                    lineForPolygon = new QGraphicsLineItem(QLineF(startPoint1, startPoint1)) ;
                    lineForPolygon->setPen(QPen(Qt::white, 1));
                    addItem(lineForPolygon);
                    isFirstPress = false;
                }
                else
                {
                    QPointF startPoint = lineForPolygon->line().p2();

                    lineForPolygon = new QGraphicsLineItem(QLineF(startPoint, startPoint)) ;
                    lineForPolygon->setPen(QPen(Qt::white, 1));
                    addItem(lineForPolygon);
                }
            }
            else if( mouseEvent->button() == Qt::RightButton)
            {
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
    if (myMode == Line && line != nullptr)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QLineF newLine(line->line().p1(), endPoint);
        line->setLine(newLine);
    }
    else if(myMode == Rectangle && rect != nullptr)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QRectF newRect(rect->rect().topLeft(), endPoint);
        rect->setRect(newRect);
    }
    else if(myMode == Polygon && lineForPolygon != nullptr)
    {
         QPointF endPoint = mouseEvent->scenePos();
         changeReceivedPoint(endPoint);

         QLineF newlineForPolygon(lineForPolygon->line().p1(), endPoint);
         lineForPolygon->setLine(newlineForPolygon);
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == Line && line != nullptr)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QLineF newLine(line->line().p1(), endPoint);
        line->setLine(newLine);
    }
    else if(myMode == Rectangle && rect != nullptr)
    {
        QPointF endPoint = mouseEvent->scenePos();
        changeReceivedPoint(endPoint);

        QRectF newRect(rect->rect().topLeft(), endPoint);
        rect->setRect(newRect);
    }
    else if(myMode == Polygon && lineForPolygon != nullptr)
    {
        if( mouseEvent->button() == Qt::LeftButton)
        {
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
    return cellList;
}

qreal Scene::power(QRectF intersectedRect, QRectF rect, int i)
{
    QFile dataFile(m_dataFileName);
    if (!dataFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("The file cannot opened \n(for powers).");
        msgBox.exec();
    }

    QTextStream inStream(&dataFile);

    qreal rectPower = 0;
    int rectNumber = 0;
    while (!inStream.atEnd())
    {
        QString line = dataFile.readLine();

        line = line.simplified();
        QStringList parameterList = line.split(';');

        if(rectNumber == i)
        {
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

