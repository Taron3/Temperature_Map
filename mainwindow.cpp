#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "surfacegraph.h"
#include "data.h"

#include <QMouseEvent>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QFileDialog>
#include <QVector>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();
    createToolBar();
    // createGroupBox();


    scene = new Scene(this);
    scene->setSceneRect(QRectF(180,90,260,200) );
    scene->setBackgroundBrush(Qt::black);  // Qt::cyan

    QHBoxLayout *layout = new QHBoxLayout;
    //layout -> addWidget(groupBox );
    view = new QGraphicsView(scene);
    layout -> addWidget(view);

    ////////////////////////////////////////////////////////////////////////////////////////
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    // sfg = nullptr;

    setCentralWidget(widget);

    resize(700,500);
}


MainWindow::~MainWindow()
{
    delete ui;

    delete view;
    delete scene;
}

void MainWindow::groupBoxClicked()
{
    if (Default->isChecked())
    {
        scene->setMode(Scene::Default);
        qDebug() << Q_FUNC_INFO << "inDefauult"  ;
    }
    else if (Line->isChecked())
    {
        scene->setMode(Scene::Line);
        qDebug() << Q_FUNC_INFO << "inLine"  ;
    }
    else if (Rectangle->isChecked())
    {
        scene->setMode(Scene::Rectangle);
        qDebug() << Q_FUNC_INFO << "inRectangle"  ;
    }
    else if (Polygon->isChecked())
    {
        scene->setMode(Scene::Polygon);
        qDebug() << Q_FUNC_INFO << "inPolyygon"  ;
    }

/*
                                    QRadioButton *btn = qobject_cast<QRadioButton*>(QObject::sender() );
                                    if(btn!= null && btn.isChecked() )
                                    {
                                       Switch(btn.Text())
                                       {
                                          case "&Default":
                                            scene->setMode(Scene::Default);
                                            break;
                                          ...
                                       }
                                    }

*/
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
    // read file and add rects in scene
    scene->addRectFromFile();
}

void MainWindow::gridActionTriggered()
{
    scene->drawGrid(scene->itemsBoundingRect() ); //  < -------------------------------------
}

void MainWindow::netlistActionTriggered()
{
    QString netlist = scene->writeNetlist();
    QFile file("/home/taron/Desktop/netlist");
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream stream(&file);
        stream << netlist;
    }

    file.close();
}

void MainWindow::thermalMapActionTriggered()
{
    QString ic = QFileDialog::getOpenFileName(this, "Open File", "/home", "*ic0");
    if (ic.isEmpty())
    {
        return;
    }
qDebug() << "FFFFFF " << ic << "\n";
    int row = (scene->itemsBoundingRect().height() / scene->getGridSize()) + 1;
    int column = (scene->itemsBoundingRect().width() / scene->getGridSize()) + 1;
    int layer = scene->getLayer();

    QVector<qreal> cellPower = Parser::getCellPowers(ic, row, column, layer);
qDebug() << "\nCELLS=  " << cellPower.size() << "\n" << cellPower << "\n";

    SurfaceGraph *surfg = new SurfaceGraph();
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
    groupBox = new QGroupBox("GroupBox");

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



/*
                                    groupBox = new QGroupBox("AAAAAAAAAAAAAAA");

                                    QRadioButton *Default   = new QRadioButton("&Default");
                                    QRadioButton *Line      = new QRadioButton("&Line");
                                    QRadioButton *Rectangle = new QRadioButton("&Rectangle");
                                    QRadioButton *Polygon   = new QRadioButton("&Polygon");
                                    Default->setChecked(true);

                                    buttonGroup = new QButtonGroup(groupBox);

                                    buttonGroup->addButton(Default);
                                    buttonGroup->addButton(Line);
                                    buttonGroup->addButton(Rectangle);
                                    buttonGroup->addButton(Polygon);

                                    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(groupBoxClicked()));

                                    QVBoxLayout *vbox = new QVBoxLayout;
                                    vbox->addWidget(Default);
                                    vbox->addWidget(Line);
                                    vbox->addWidget(Rectangle);
                                    vbox->addWidget(Polygon);

                                    vbox->addStretch(1);
                                    groupBox->setLayout(vbox);

*/

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
    openAction->setIcon(QIcon(":/images/open.png"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openActionTriggered()) );

    gridAction = new QAction("&Grid");
    gridAction->setIcon(QIcon(":/images/grid.png"));
    // gridAction->setCheckable(true);
    connect(gridAction, SIGNAL(triggered()), this, SLOT(gridActionTriggered()) );

    netlisAction = new QAction("&Netlist");
    netlisAction->setIcon(QIcon(":/images/netlist.png"));
    connect(netlisAction, SIGNAL(triggered()), this, SLOT(netlistActionTriggered()) );

    thermalMapAction = new QAction("&Thermal Map");
    thermalMapAction->setIcon(QIcon(":/images/surface"));
    connect(thermalMapAction, SIGNAL(triggered()), this, SLOT(thermalMapActionTriggered()) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAction);

    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(gridAction);

    generateMenu = menuBar()->addMenu("&Generate");
    generateMenu->addAction(netlisAction);
    generateMenu->addAction(thermalMapAction);

    aboutMenu = menuBar()->addMenu("&About");
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
    gridSpinBox->setValue(30);
    connect(gridSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setGridSizeTriggered(int)) );

    layerSpinBox = new QSpinBox();
    layerSpinBox->setPrefix("Layer: ");
    layerSpinBox->setRange(0, 30);
    connect(layerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLayerTriggered(int)) );

    editToolBar = addToolBar("Edit Tool Buttons");
///////////////////////////////////////////////////////////////////
    // editToolBar->addWidget(boundingBoxButton);              ///////
    // editToolBar->addWidget(addRandomRectButton);            ///////
///////////////////////////////////////////////////////////////////

    editToolBar->addWidget(layerSpinBox);
    editToolBar->addAction(openAction);
    editToolBar->addAction(netlisAction);

    editToolBar->addWidget(gridSpinBox);
    editToolBar->addAction(gridAction);

    editToolBar->addAction(thermalMapAction);

}

