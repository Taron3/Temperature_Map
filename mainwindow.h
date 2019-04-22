#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>
#include <QSpinBox>

#include "scene.h"
#include "surfacegraph.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void groupBoxClicked();
    void boundingBoxButtonTriggered();
    void addRandomRectButtonTriggered();

    void openActionTriggered();
    void gridActionTriggered(bool isCecked);
    void netlistActionTriggered();
    void thermalMapActionTriggered();
    void setGridSizeTriggered(int gridSpinBoxValue);
    void setLayerTriggered(int layerSpinBoxValue);

private:    
    Ui::MainWindow *ui;

    void createGroupBox();
    QRadioButton *createRadioButton(const QString &text);
    void createActions();
    void createMenus();
    void createToolBar();

    QGraphicsView *view;
    Scene *scene;

    QRadioButton *Default;
    QRadioButton *Line;
    QRadioButton *Rectangle;
    QRadioButton *Polygon;

    QGroupBox *groupBox;
    QButtonGroup *buttonGroup;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *generateMenu;
    QMenu *aboutMenu;

    QToolBar *editToolBar;
    QToolButton *boundingBoxButton;
    QToolButton *addRandomRectButton;

    QAction *openAction;
    QAction *gridAction;
    QAction *netlisAction;
    QAction *thermalMapAction;

    QSpinBox *gridSpinBox;
    QSpinBox *layerSpinBox;
};

#endif // MAINWINDOW_H





