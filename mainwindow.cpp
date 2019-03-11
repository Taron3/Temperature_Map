#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QRadioButton>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createToolBar();
    createGroupBox();



    scene = new Scene(this);
    scene->setSceneRect(QRectF(10,10,260,200) );

    QHBoxLayout *layout = new QHBoxLayout;
    layout -> addWidget(groupBox );
    view = new QGraphicsView(scene);
    layout -> addWidget(view);

    ////////////////////////////////////////////////////////////////////////////////////////
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

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

void MainWindow::createToolBar()
{
    boundingBoxButton = new QToolButton;
    boundingBoxButton->setIcon(QIcon(":/images/bbox.png"));
    connect(boundingBoxButton, SIGNAL(clicked()), this, SLOT(boundingBoxButtonTriggered() ));
    addRandomRectButton = new QToolButton;
    addRandomRectButton->setIcon(QIcon(":/images/rrect.png"));
    connect(addRandomRectButton, SIGNAL(clicked()), this, SLOT(addRandomRectButtonTriggered() ));

    editToolBar = addToolBar("Edit Tool Buttons");
    editToolBar->addWidget(boundingBoxButton);
    editToolBar->addWidget(addRandomRectButton);
}

