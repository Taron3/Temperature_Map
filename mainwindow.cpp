#include "mainwindow.h"
#include "surfacegraph.h"
#include "data.h"

#include <QtWidgets>
#include <QMouseEvent>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QFileDialog>
#include <QDir>
#include <QVector>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();
    createToolBar();
            // createGroupBox();


    scene = new Scene(this);
    scene->setSceneRect(QRectF(180,90,260,200) );
    // scene->setBackgroundBrush(Qt::black);
    QHBoxLayout *layout = new QHBoxLayout;
            //layout -> addWidget(groupBox );
    view = new QGraphicsView(scene);
    layout -> addWidget(view);

    /////////////////////////////
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);

        // scene->setBackgroundBrush(Qt::black);
    setWindowTitle("Temperature Map");
    resize(700,500);
}

void MainWindow::groupBoxClicked()
{
    if (Default->isChecked())
    {
        scene->setMode(Scene::Default);
    }
    else if (Line->isChecked())
    {
        scene->setMode(Scene::Line);
    }
    else if (Rectangle->isChecked())
    {
        scene->setMode(Scene::Rectangle);
    }
    else if (Polygon->isChecked())
    {
        scene->setMode(Scene::Polygon);
    }
}

void MainWindow::boundingBoxButtonTriggered()
{
    scene->boundingBox();
}

void MainWindow::addRandomRectButtonTriggered()
{
    scene->addRandomRect();
}

void MainWindow::openActionTriggered()
{
    // read file and add elements in scene
    QString layout = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), "*el");
    if(layout.isEmpty())
    {
        return;
    }

    scene->setDataFileName(layout);
    scene->addElementsFromFile();
}

void MainWindow::gridActionTriggered(bool isCecked)
{
    scene->setDraw(isCecked);
}

void MainWindow::netlistActionTriggered()
{
    QString netlist = scene->writeNetlist();
    QString fileNetlist = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::homePath());

    QFile file(fileNetlist + "/netlist.sp");
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream outStream(&file);
        outStream << netlist;
    }
}

void MainWindow::thermalMapActionTriggered()
{
    QString ic = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), "*ic0");

    if (ic.isEmpty())
    {
        return;
    }

    int row = (scene->itemsBoundingRect().height() / scene->getGridSize()) + 1;
    int column = (scene->itemsBoundingRect().width() / scene->getGridSize()) + 1;
    int layer = scene->getLayer();

    QVector<qreal> cellPowers = Parser::getCellPowers(ic, row, column, layer);
    SurfaceGraph *surfg = new SurfaceGraph(cellPowers, row, column, layer);
    surfg->show();
}

void MainWindow::setGridSizeTriggered(int gridSpinBoxValue)
{
    scene->setGridSize(gridSpinBoxValue);
}

void MainWindow::setLayerTriggered(int layerSpinBoxValue)
{
    scene->setLayer(layerSpinBoxValue);
}

void MainWindow::createGroupBox()
{
    groupBox = new QGroupBox("Draw");

    Default   = createRadioButton("&Default");
    Line      = createRadioButton("&Line");
    Rectangle = createRadioButton("&Rectangle");
    Polygon   = createRadioButton("&Polygon");
    Default->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(Default);
    vbox->addWidget(Line);
    vbox->addWidget(Rectangle);
    vbox->addWidget(Polygon);

    vbox->addStretch(1);
    groupBox->setLayout(vbox);
}

QRadioButton *MainWindow::createRadioButton(const QString &text)
{
    QRadioButton *button = new QRadioButton(text);
    connect(button, SIGNAL(clicked()), this, SLOT(groupBoxClicked() ));
    return button;
}
/////////////////////////////////////////////////


void MainWindow::createActions()
{
    openAction = new QAction("&Open");
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open the file with the preliminary distribution of the "
                                "elements of integrated circuits"));
    openAction->setIcon(QIcon(":/images/open.png"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openActionTriggered()) );

    exitAction = new QAction("E&xit");
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the Temperature Map"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    gridAction = new QAction("&Grid");
    gridAction->setShortcut(tr("Ctrl+G"));
    gridAction->setStatusTip("Enable / Disable grid display");
    gridAction->setIcon(QIcon(":/images/grid.png"));
    gridAction->setCheckable(true);
    connect(gridAction, SIGNAL(toggled(bool) ), this, SLOT(gridActionTriggered(bool)) );

    netlistAction = new QAction("&Netlist");
    netlistAction->setShortcut(tr("Ctrl+Alt+N"));
    netlistAction->setStatusTip("Create netlist.sp file");
    netlistAction->setIcon(QIcon(":/images/netlist.png"));
    connect(netlistAction, SIGNAL(triggered()), this, SLOT(netlistActionTriggered()) );

    thermalMapAction = new QAction("&Temperature Map");
    thermalMapAction->setShortcut(tr("Ctrl+T"));
    thermalMapAction->setStatusTip("Show Temperature Map");
    thermalMapAction->setIcon(QIcon(":/images/surface"));
    connect(thermalMapAction, SIGNAL(triggered()), this, SLOT(thermalMapActionTriggered()) );

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show info about Temperature Map"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutActoinTriggered()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(gridAction);

    generateMenu = menuBar()->addMenu("&Generate");
    generateMenu->addAction(netlistAction);
    generateMenu->addAction(thermalMapAction);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAction);
}

void MainWindow::createToolBar()
{
    boundingBoxButton = new QToolButton;
    boundingBoxButton->setIcon(QIcon(":/images/bbox.png"));
    connect(boundingBoxButton, SIGNAL(clicked()), this, SLOT(boundingBoxButtonTriggered() ));

    addRandomRectButton = new QToolButton;
    addRandomRectButton->setIcon(QIcon(":/images/rrect.png"));
    connect(addRandomRectButton, SIGNAL(clicked()), this, SLOT(addRandomRectButtonTriggered() ));

    gridSpinBox = new QSpinBox();
    gridSpinBox->setPrefix("Grid size: ");
    gridSpinBox->setRange(5, 100);
    gridSpinBox->setSingleStep(5);
    gridSpinBox->setValue(35);
    connect(gridSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setGridSizeTriggered(int)) );

    layerSpinBox = new QSpinBox();
    layerSpinBox->setPrefix("Layer: ");
    layerSpinBox->setRange(0, 10);
    connect(layerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLayerTriggered(int)) );

    QToolBar *editToolBar = addToolBar(tr("Edit Tool Buttons"));

    // editToolBar->addWidget(boundingBoxButton);
    // editToolBar->addWidget(addRandomRectButton);


    editToolBar->addWidget(layerSpinBox);
    editToolBar->addAction(openAction);
    editToolBar->addAction(netlistAction);

    editToolBar->addWidget(gridSpinBox);
    editToolBar->addAction(gridAction);

    editToolBar->addAction(thermalMapAction);
}

void MainWindow::aboutActoinTriggered()
{
    QMessageBox::about(this, tr("About Temperature Map"),
             tr("The application demonstrates "
                "<b>Temperature Map</b> of three-dimensional integrated circuits."));

}

