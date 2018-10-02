#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //Layout
    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);



    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    createActions();
    createMenus();


    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);


    setWindowTitle(tr("Aquisiction3D"));
    setMinimumSize(160, 160);
    resize(480, 320);

    //Menu



    //Toolbar e Elementos

}

MainWindow::~MainWindow()
{

}


void MainWindow::newFile()
{
    infoLabel->setText(tr("Detectar dispostivos conectados"));
}



void MainWindow::createActions()
{
    newAct = new QAction(tr("&Detectar"), this);
    //newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Detectar um dispositivo"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Devices"));
       fileMenu->addAction(newAct);
}
