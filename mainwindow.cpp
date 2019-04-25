#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "surfacegraph.h"
#include "data.h"

#include <QMouseEvent>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QFileDialog>
#include <QDir>
#include <QVector>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();
    createToolBar();
            //createGroupBox();


    scene = new Scene(this);
    scene->setSceneRect(QRectF(180,90,260,200) );
    // scene->setBackgroundBrush(Qt::black);  // Qt::cyan

    QHBoxLayout *layout = new QHBoxLayout;
            //layout -> addWidget(groupBox );
    view = new QGraphicsView(scene);
    layout -> addWidget(view);

    ////////////////////////////////////////////////////////////////////////////////////////
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);

        // scene->setBackgroundBrush(Qt::black);
    setWindowTitle("Thermal Modeling");
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
//qDebug() << fileNetlist << "\n";

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
     //QString ic = "/home/taron/Desktop/netlist(1).ic0";

    if (ic.isEmpty())
    {
        return;
    }
//qDebug() << "FFFFFF " << ic << "\n";
    int row = (scene->itemsBoundingRect().height() / scene->getGridSize()) + 1;
    int column = (scene->itemsBoundingRect().width() / scene->getGridSize()) + 1;
    int layer = scene->getLayer();

    QVector<qreal> cellPowers = Parser::getCellPowers(ic, row, column, layer);
//qDebug() << "\nCELLS=  " << cellPowers.size() << "\n" << cellPowers << "\n";
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
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open the file with the preliminary distribution of the "
                                "elements of integrated circuits"));
    openAction->setIcon(QIcon(":/images/open.png"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openActionTriggered()) );

    exitAction = new QAction("E&xit");
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the Thermal Modeling"));
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

    thermalMapAction = new QAction("&Thermal Map");
    thermalMapAction->setShortcut(tr("Ctrl+T"));
    thermalMapAction->setStatusTip("Show Thermal Model");
    thermalMapAction->setIcon(QIcon(":/images/surface"));
    connect(thermalMapAction, SIGNAL(triggered()), this, SLOT(thermalMapActionTriggered()) );

    aboutAction = new QAction(tr("&About Thermal Modeling"), this);
    aboutAction->setStatusTip(tr("Show info about Thermal Map"));
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

    //QToolBar *fileToolBar = addToolBar(tr("File"));
    QToolBar *editToolBar = addToolBar(tr("Edit Tool Buttons"));
///////////////////////////////////////////////////////////////////
    // editToolBar->addWidget(boundingBoxButton);           ///////
    // editToolBar->addWidget(addRandomRectButton);         ///////
///////////////////////////////////////////////////////////////////

    editToolBar->addWidget(layerSpinBox);
    editToolBar->addAction(openAction);
    editToolBar->addAction(netlistAction);

    editToolBar->addWidget(gridSpinBox);
    editToolBar->addAction(gridAction);

    editToolBar->addAction(thermalMapAction);
}

void MainWindow::aboutActoinTriggered()
{
    QMessageBox::about(this, tr("About Thermal Modeling"),
             tr("The application demonstrates "
                "<b>Thermal Modeling</b> of three-dimensional integrated circuits."));

}

