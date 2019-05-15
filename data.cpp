
#include <data.h>

#include <QString>
#include <QPointF>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QMessageBox>

#include <QDebug>


QPointF Parser::getPoint(QStringList &strList)
{
    QString topStr = strList[0];
    QStringList stringList = topStr.split('=');

    QString pointX = stringList[1].section(',', 0, 0);
    QString pointY = stringList[1].section(',', 1, 1);
    return QPointF(pointX.toDouble(), pointY.toDouble());
}

double Parser::getWidth(QStringList &strList)
{
    QString widthStr = strList[1];
    QStringList stringList = widthStr.split('=');
    return stringList[1].toDouble();
}

double Parser::getHeight(QStringList &strList)
{
    QString heightStr = strList[2];
    QStringList stringList = heightStr.split('=');
    return stringList[1].toDouble();
}

int Parser::getLayer(QStringList &strList)
{
    QString layerStr = strList[3];
    QStringList stringList = layerStr.split('=');
    return stringList[1].toInt();
}

qreal Parser::getPower(QStringList &strList)
{
    QString powerStr = strList[4];
    QStringList stringList = powerStr.split('=');
    return stringList[1].toDouble();
}

QVector<qreal> Parser::getCellPowers(QString &fileName, int row, int column, int layer)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("The document cannot opened.");
        msgBox.exec();
    }

    QVector<qreal> cellPowers(row * column);
    QTextStream inStream(&file);

    while (!inStream.atEnd())
    {

        QString line = file.readLine();

        line = line.simplified();
                //  + _c_10_10_0_s = 87.8748m

        if(line.startsWith("+ _c_"))
        {

            QStringList sl = line.split('=');

            QString cellName = sl[0];
            if(cellName.section('_', 5, 5) == "s ")
            {
                continue;
            }
            int i = cellName.section('_', 2, 2).toInt(); // get i
            int j = cellName.section('_', 3, 3).toInt(); // get j
            int k = cellName.section('_', 4, 4).toInt(); // get layer
                // if(layer == k)

            QString cellValue = sl[1];
            if(cellValue.endsWith('m'))
            {
                cellValue.remove(' ');
                cellValue.remove('m');
                qreal value = cellValue.toDouble() / 1000;
                cellValue = QVariant(value).toString();
            }
            if(cellValue.endsWith('u'))
            {
                cellValue.remove(' ');
                cellValue.remove('u');
                qreal value = cellValue.toDouble() / 1000000;
                cellValue = QVariant(value).toString();
            }
            qreal value = cellValue.toDouble();
            cellPowers[i * column + j] = value;
        }
    }
    return cellPowers;
}
