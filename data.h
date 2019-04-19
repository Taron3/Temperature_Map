#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QPointF>
#include <QStringList>
#include <QVector>

namespace Parser
{
    QPointF getPoint(QStringList &strList);
    double getWidth(QStringList &strList);
    double getHeight(QStringList &strList);
    int getLayer(QStringList &strList);
    qreal getPower(QStringList &strList);
    QVector<qreal> getCellPowers(QString &fileName, int row, int column, int layer);
}

#endif // DATA_H
