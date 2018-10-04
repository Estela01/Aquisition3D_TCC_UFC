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


    //Toolbar e Elementos

    //RGB Window/*

    m_mdiArea = new QMdiArea;
    setCentralWidget(m_mdiArea);

    m_rgb= new RGBWindow(this);
    m_rgb->setMode(0);
    QMdiSubWindow *subWindow1 = new QMdiSubWindow;
    subWindow1->setWidget(m_rgb);
    subWindow1->setAttribute(Qt::WA_DeleteOnClose);
    subWindow1->setWindowTitle("RGB Output");
    subWindow1->resize(640,480);
    m_mdiArea->addSubWindow(subWindow1);
    subWindow1->show();
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
        subWindow1->setWindowTitle("Kinect: "+  QString::number(i+1)+" RGB Output");
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

//Start: Change to Kinecct Class
//Iniciar Kinect

//void MainWindow::initKinect(){
//    if (freenect_init(&m_ctx, NULL) < 0){
//        qDebug()<<"freenect_init() failed\n";
//        exit(EXIT_FAILURE);
//    }
//    freenect_set_log_level(m_ctx,FREENECT_LOG_DEBUG);
//    int nr_devices = freenect_num_devices(m_ctx);
//    qDebug()<<"numero de devices kinect found"<<nr_devices<<"\n";
//    infoLabel->setText("Dispositivos Kinect encontrados: " +QString::number( nr_devices ));

//    //Alocate the buffers
//    m_userDeviceNumber = 0;


////    if(freenect_open_device(m_ctx, &m_dev, m_userDeviceNumber) < 0){
////        qDebug()<<"Não foi possivel abrir o dispositivo";
////        exit(EXIT_FAILURE);

////    }

////    freenect_camera_to_world();


//   // freenect_start_video(m_dev);

////    //Descobrindo serial das cameras
////    for(int i=0; i < nr_devices; i++){
////        freenect_open_device(m_ctx, &m_dev, i);

////    }



//   int count  = freenect_list_device_attributes(m_ctx, &attribute_list);
//   attribute_list->camera_serial;
//   statusBar()->showMessage(attribute_list->camera_serial);

//   qDebug()<<freenect_enabled_subdevices(m_ctx)<<" Dispositivos abertos.";
//   freenect_open_device_by_camera_serial(m_ctx,&m_dev,"A00364A10850110A");
//   qDebug()<<freenect_enabled_subdevices(m_ctx)<<" Dispositivos abertos apos open_device.";






//}


////End
