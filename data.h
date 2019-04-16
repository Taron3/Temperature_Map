#ifndef DATA_H
#define DATA_H

#include <QString>

namespace Parser
{
    QPointF getPoint(QStringList &strList)
    {
        QString topStr = strList[0];
        QStringList stringList = topStr.split('=');

        QString pointX = stringList[1].section(',', 0, 0);
        QString pointY = stringList[1].section(',', 1, 1);
        return QPointF(pointX.toDouble(), pointY.toDouble());
    }

    double getWidth(QStringList &strList)
    {
        QString widthStr = strList[1];
        QStringList stringList = widthStr.split('=');
        return stringList[1].toDouble();
    }

    double getHeight(QStringList &strList)
    {
        QString heightStr = strList[2];
        QStringList stringList = heightStr.split('=');
        return stringList[1].toDouble();
    }

    int getLayer(QStringList &strList)
    {
        QString layerStr = strList[3];
        QStringList stringList = layerStr.split('=');
        return stringList[1].toInt();
    }

    qreal getPower(QStringList &strList)
    {
        QString powerStr = strList[4];
        QStringList stringList = powerStr.split('=');
        return stringList[1].toDouble();
    }
}

#endif // DATA_H
