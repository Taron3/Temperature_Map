#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>
#include <QSpinBox>
#include <QShortcut>

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

    void aboutActoinTriggered();

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
    QMenu *helpMenu;

    //QToolBar *editToolBar;
    QToolButton *boundingBoxButton;
    QToolButton *addRandomRectButton;

    QAction *openAction;
    QAction *exitAction;
    QAction *gridAction;
    QAction *netlistAction;
    QAction *thermalMapAction;
    QAction *aboutAction;

    QSpinBox *gridSpinBox;
    QSpinBox *layerSpinBox;

    QShortcut *shortcut;
};

#endif // MAINWINDOW_H





