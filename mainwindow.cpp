#include "mainwindow.h"


#include <QAction>
#include <QKeySequence>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_kinect = QKinectSensor::instace();

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

    //Toolbar e Elementos

    boxDevice = new QComboBox();
    boxDevice->addItem("Nenhum", -1);

    boxDeviceDepth = new QComboBox();
    boxDeviceDepth->addItem("Nenhum", -1);



    int n_devices = m_kinect->getNumberDevices();
    //int n_devices = 3;
    for (int i = 0; i < n_devices ; i++){
        fToolbar = new QToolBar("Device Control", this);
        fToolbar->setObjectName("Teste");
        QLabel *label = new QLabel(fToolbar);
        label -> setText("Kinect " + QString::number(i+1));
        label-> setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-weight: bold; color: black");
        fToolbar->addWidget(label);
        fToolbar->addSeparator();
        label = new QLabel(fToolbar);
        label -> setText("Dados");
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);

        label = new QLabel(fToolbar);
        label -> setText("Serial: ");
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);

        label = new QLabel(fToolbar);
        label -> setText("Modelo: ");
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);


        label = new QLabel(fToolbar);
        label -> setText("Angle: ");
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);


        fToolbar->addSeparator();


//        QPushButton *rgbMode = new QPushButton("Visualizar RGB");
//        rgbMode-> setFlat(true);

        //connect(rgbMode, SIGNAL(triggered()), this, [this]{ });

//        fToolbar->addWidget(rgbMode);
        //fToolbar->addSeparator();




        boxDevice->addItem("Kinect " + QString::number(i+1), i);
        boxDeviceDepth->addItem("Kinect " + QString::number(i+1), i);

        this -> addToolBar(Qt::LeftToolBarArea,fToolbar);


    }

   fToolbar = new QToolBar("Device Control", this);

    QLabel *label = new QLabel(fToolbar);
    label -> setText("Visualizar RGB");
    label-> setAlignment(Qt::AlignLeft);
    fToolbar->addWidget(label);
    fToolbar->addWidget(boxDevice);
    connect(boxDevice, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeStatus(int)));

    fToolbar->addSeparator();

    this -> addToolBar(Qt::LeftToolBarArea,fToolbar);




    fToolbar = new QToolBar("Device Control", this);
     label = new QLabel(fToolbar);
     label -> setText("Visualizar Depth");
     label-> setAlignment(Qt::AlignLeft);
     fToolbar->addWidget(label);
     fToolbar->addWidget(boxDeviceDepth);
     connect(boxDeviceDepth, SIGNAL(currentIndexChanged(int)),
             this, SLOT(changeStatus(int)));
     fToolbar->addSeparator();





    QPushButton *capture = new QPushButton("Capture");

    fToolbar->addWidget(capture);
    this -> addToolBar(Qt::LeftToolBarArea,fToolbar);






    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);



    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);



    setWindowTitle(tr("Aquisiction3D"));
    setMinimumSize(160, 160);
    resize(480, 320);

    //Menu
    createActions();
    createMenus();






    //RGB Window/*

    m_mdiArea = new QMdiArea;
    setCentralWidget(m_mdiArea);

//    m_rgb= new RGBWindow(this);
//    m_rgb->setMode(0);
//    QMdiSubWindow *subWindow1 = new QMdiSubWindow;
//    subWindow1->setWidget(m_rgb);
//    subWindow1->setAttribute(Qt::WA_DeleteOnClose);
//    subWindow1->setWindowTitle("RGB Output");
//    subWindow1->resize(640,480);
//    m_mdiArea->addSubWindow(subWindow1);
//    subWindow1->show();
////    /// create a window for our depth draw (1 = depth)
////    m_depth= new RGBWindow(this);
////    m_depth->setMode(1);
////    QMdiSubWindow *subWindow2 = new QMdiSubWindow;
////    subWindow2->setWidget(m_depth);
////    subWindow2->setAttribute(Qt::WA_DeleteOnClose);
////    subWindow2->setWindowTitle("Depth Output");
////    subWindow2->resize(640,480);
////    m_mdiArea->addSubWindow(subWindow2);
////    subWindow2->show();
    m_mdiArea->tileSubWindows();



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
    connect(newAct,  SIGNAL(triggered()), this, SLOT(initKinect()));
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Devices"));
    fileMenu->addAction(newAct);
}

void MainWindow::createRGBWindow(){
    //loop
    int n_devides_kinect = m_kinect->getNumberDevices();


    for(int i = 0; i < n_devides_kinect;i++){
        m_rgb= new RGBWindow(this);
        m_rgb->setMode(0);
        QMdiSubWindow *subWindow1 = new QMdiSubWindow;
        subWindow1->setWidget(m_rgb);
        subWindow1->setAttribute(Qt::WA_DeleteOnClose);
        subWindow1->setWindowTitle("Kinect: "+  QString::number(i)+" RGB Output");
        subWindow1->resize(640,480);
        m_mdiArea->addSubWindow(subWindow1);
        subWindow1->show();
        m_mdiArea->tileSubWindows();

    }

}

void MainWindow::createRGBWindowForDevice(int indexDevice){

    if (indexDevice - 1 < m_kinect->getNumberDevices()){

        m_kinect->setDeviceToShowRGB(indexDevice);

        m_rgb= new RGBWindow(this);
        m_rgb->setMode(0);
        m_rgb->setIndexDevice(indexDevice);
        QMdiSubWindow *subWindow1 = new QMdiSubWindow;
        subWindow1->setWidget(m_rgb);
        subWindow1->setAttribute(Qt::WA_DeleteOnClose);
        subWindow1->setWindowTitle("Kinect: "+  QString::number(indexDevice)+" RGB Output");
        subWindow1->resize(640,480);
        m_mdiArea->addSubWindow(subWindow1);
        subWindow1->show();
        m_mdiArea->tileSubWindows();


    }




}


void MainWindow::initKinect(){

//    m_kinect = QKinectSensor::instace();


     int n_devides_kinect = m_kinect->getNumberDevices();
//     infoLabel -> setText("Dispositivos Kinects encontrados" + QString::number(n_devides_kinect));
     statusBar() -> showMessage("Dispositivos Kinects encontrados" + QString::number(n_devides_kinect));
    // createRGBWindow();
     //showSerialNumber();

}



void MainWindow::showSerialNumber(){
    const char* serialNumber = m_kinect->getSerialNumber();
    statusBar()->showMessage("Numero de serie: " + QString::fromUtf8(serialNumber));

}

void MainWindow::changeStatus(int index){
    qDebug()<<"index: "<<index;
    qDebug()<<"index Anterior: "<<indexDeviceAnterior;

    if (indexDeviceAnterior == -1){
        indexDeviceAnterior = index;
    } else {
        if (indexDeviceAnterior != 0){
            m_kinect->stopVideo(indexDeviceAnterior - 1);
        }
    }

    if(index > 0){
        m_mdiArea->closeAllSubWindows();
        statusBar()->showMessage("Visualizando RGB do kinect" + QString::number(index));
        createRGBWindowForDevice(index);
    }if(index == 0){

        m_mdiArea->closeAllSubWindows();
    }

    indexDeviceAnterior = index;
}
