#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H


#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>

#include <QWidget>

using namespace QtDataVisualization;

class SurfaceGraph : public QWidget
{
    Q_OBJECT
public:
    explicit SurfaceGraph(QVector<qreal> &cellPowers, int row, int column, int layer, QWidget *parent = nullptr);
    ~SurfaceGraph();

    void sqrtSinModel(double max);

    //! [0]
    void toggleModeNone() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionNone); }
    void toggleModeItem() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem); }
    void toggleModeSliceRow() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow
                                                          | QAbstract3DGraph::SelectionSlice); }
    void toggleModeSliceColumn() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn
                                                             | QAbstract3DGraph::SelectionSlice); }
    //! [0]

    void setBlackToYellowGradient();
    void setGreenToRedGradient();

public Q_SLOTS:

private:
    Q3DSurface *m_graph;
    QSurfaceDataProxy *m_sqrtSinProxy;
    QSurface3DSeries *m_sqrtSinSeries;
        // QSurface3DSeries::DrawFlag m_draw_mode = QSurface3DSeries::DrawSurfaceAndWireframe;
    QVector<qreal> m_cellPowers;
    int m_row;
    int m_column;
    int m_layer;
    void fillSqrtSinProxy();
};

#endif // SURFACEGRAPH_H
