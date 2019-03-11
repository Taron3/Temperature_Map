#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>
#include "scene.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void groupBoxClicked();
    void boundingBoxButtonTriggered();
    void randomRectButtonTriggered();

private:    
    Ui::MainWindow *ui;

    void createGroupBox();
    QRadioButton *createRadioButton(const QString &text);
    void createToolBar();

    QGraphicsView *view;
    Scene *scene;

    QRadioButton *Default;
    QRadioButton *Line;
    QRadioButton *Rectangle;
    QRadioButton *Polygon;

    QGroupBox *groupBox;
    QButtonGroup *buttonGroup;

    QToolBar *editToolBar;
    QToolButton *boundingBoxButton;
    QToolButton *randomRectButton;

};

#endif // MAINWINDOW_H





