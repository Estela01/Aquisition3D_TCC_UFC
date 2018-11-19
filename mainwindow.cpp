#include "mainwindow.h"


#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    initKinect();
    m_realSense->init();
//    auto start = std::chrono::system_clock::now();

    //Toolbar e Elementos
    QWidget *widget = new QWidget();
    setCentralWidget(widget);

    boxDevice = new QComboBox();
    boxDevice->addItem("Nenhum", -1);

    boxDeviceDepth = new QComboBox();
    boxDeviceDepth->addItem("Nenhum", -1);


   fToolbar = new QToolBar("Dispositivos detectados", this);
   QLabel *qlabel = new QLabel(fToolbar);
   qlabel -> setText("Dados do Dipositivo" );
   qlabel-> setAlignment(Qt::AlignLeft);
   qlabel->setStyleSheet("font-weight: bold; color: black");
   fToolbar->addWidget(qlabel);
   fToolbar->addSeparator();

   int n_devices = m_kinect->getNumberDevices();
   // int n_devices = 3;
    for (int i = 0; i < n_devices ; i++){
        fToolbar->setFixedWidth(160);

        fToolbar->setObjectName("Teste");
        QLabel *label = new QLabel(fToolbar);

        label -> setText("Kinect " + QString::number(i+1));
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);

        label = new QLabel(fToolbar);
        const char *serial = m_kinect -> getSerialNumber(i);
        label -> setText("Serial: " + QString::fromStdString(serial));
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);

        label = new QLabel(fToolbar);
        label -> setText("Angulo: ");
        label-> setAlignment(Qt::AlignLeft);
        fToolbar->addWidget(label);

        QDoubleSpinBox *angle = new QDoubleSpinBox(fToolbar);
        angle->setMaximum(30.0);
        angle->setMinimum(-30.0);
        angle->setSingleStep(1.0);
        QObject::connect(angle,SIGNAL(valueChanged(double)),m_kinect,SLOT(setAngle(double)));

        fToolbar->addWidget(angle);

        fToolbar->addSeparator();

        boxDevice->addItem("Kinect " + QString::number(i+1), i);
        boxDeviceDepth->addItem("Kinect " + QString::number(i+1), i);

    }

    this -> addToolBar(Qt::LeftToolBarArea,fToolbar);

    cToolbar = new QToolBar("Device Control", this);
    cToolbar->setOrientation(Qt::Horizontal);
    QLabel *capLabel = new QLabel(fToolbar);
    capLabel -> setText("Painel de Captura" );
    capLabel->setAlignment(Qt::AlignAbsolute);
    capLabel->setStyleSheet("font-weight: bold; color: black");
    cToolbar->addWidget(capLabel);
    cToolbar->addSeparator();

    QLabel *indLabel = new QLabel(fToolbar);
    indLabel -> setText("Individuo:" );
    indLabel->setAlignment(Qt::AlignLeft);
    cToolbar->addWidget(indLabel);

    indTextInput = new QLineEdit(fToolbar);
    indTextInput->setAlignment(Qt::AlignRight);
    indTextInput->setFocusPolicy(Qt::ClickFocus);
    cToolbar->addWidget(indTextInput);

    QLabel *expLabel = new QLabel(fToolbar);
    expLabel -> setText("Expressao:" );
    expLabel->setAlignment(Qt::AlignLeft);
    cToolbar->addWidget(expLabel);

    boxExpression= new QComboBox();
    boxExpression->addItem("Nenhum", -1);
    boxExpression->addItem("Neutro", 0);
    boxExpression->addItem("Alegria", 1);
    boxExpression->addItem("Tristeza", 2);
    boxExpression->addItem("Surpresa", 3);
    boxExpression->addItem("Nojo", 4);
    boxExpression->addItem("Desprezo", 5);

    connect(boxExpression, SIGNAL(currentTextChanged(QString)),
            this, SLOT(teste2(QString)));

    cToolbar->addWidget(boxExpression);

    QLabel *etaLabel = new QLabel(fToolbar);
    etaLabel -> setText("Etapa:" );
    etaLabel->setAlignment(Qt::AlignLeft);
    cToolbar->addWidget(etaLabel);

    boxEtapa= new QComboBox();
    boxEtapa->addItem("Nenhum", -1);
    boxEtapa->addItem("Etapa1", 0);
    boxEtapa->addItem("Etapa2", 1);
    boxEtapa->addItem("Etapa3", 2);

    cToolbar->addWidget(boxEtapa);

    cToolbar->addSeparator();


    QPushButton* captureRealSense = new QPushButton("Capture Realsense");
    captureRealSense->setEnabled(0);
    connect(captureRealSense, SIGNAL(clicked()),this,SLOT(saveXYZRealsense()));
    cToolbar->addWidget(captureRealSense);

    if (m_realSense->verifyRS()){
        captureRealSense->setEnabled(1);
    }


    QLabel *visLabel = new QLabel(cToolbar);
    visLabel -> setText("Selecione Kinect:");
    visLabel-> setAlignment(Qt::AlignLeft);
    cToolbar->addWidget(visLabel);
    cToolbar->addWidget(boxDevice);
    connect(boxDevice, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeStatus(int)));


    capture = new QPushButton("Capture Kinect");
    capture->setEnabled(0);
    connect(capture, SIGNAL(clicked()),this,SLOT(saveXYZKinect()));

    cToolbar->addWidget(capture);

    cToolbar->addSeparator();
    this -> addToolBar(Qt::RightToolBarArea,cToolbar);

    QString message = tr("Pronto para iniciar");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Aquisiction3D"));
    setMinimumSize(160, 160);
    resize(720, 512);

    //Menu
    createActions();
    createMenus();

    //RGB Window/*

    m_mdiArea = new QMdiArea;
    setCentralWidget(m_mdiArea);


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


        /// create a window for our depth draw (1 = depth)
        w_depth= new RGBWindow(this);
        w_depth->setMode(1);
        QMdiSubWindow *subWindow2 = new QMdiSubWindow;
        subWindow2->setWidget(w_depth);
        subWindow2->setAttribute(Qt::WA_DeleteOnClose);
        subWindow2->setWindowTitle("Depth Output");
        subWindow2->resize(640,480);
        m_mdiArea->addSubWindow(subWindow2);
        subWindow2->show();
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

        w_depth= new RGBWindow(this);
        w_depth->setMode(1);
        w_depth->setIndexDevice(indexDevice - 1);
        QMdiSubWindow *subWindow2 = new QMdiSubWindow;
        subWindow2->setWidget(w_depth);
        subWindow2->setAttribute(Qt::WA_DeleteOnClose);
        subWindow2->setWindowTitle("Depth Output");
        subWindow2->resize(640,480);
        m_mdiArea->addSubWindow(subWindow2);
        subWindow2->show();

        m_mdiArea->tileSubWindows();


    }




}


void MainWindow::initKinect(){

    m_kinect = QKinectSensor::instace();
     int n_devides_kinect = m_kinect->getNumberDevices();
     statusBar() -> showMessage("Dispositivos Kinects encontrados" + QString::number(n_devides_kinect));
    // createRGBWindow();
    // showSerialNumber();

}



void MainWindow::showSerialNumber(){

    //m_kinect->getSerialNumber();
   // statusBar()->showMessage("Numero de serie: " + QString::fromUtf8(serialNumber));

}

void MainWindow::changeStatus(int index){
    qDebug()<<"index: "<<index;
    qDebug()<<"index Anterior: "<<indexDeviceAnterior;

    if (indexDeviceAnterior == -1){
        indexDeviceAnterior = index;
    } else {
        if (indexDeviceAnterior != 0){
            m_kinect->stopVideo(indexDeviceAnterior - 1);
            m_kinect->stopDepth(indexDeviceAnterior - 1);
        }
    }

    if(index > 0){

        m_mdiArea->closeAllSubWindows();
        capture->setEnabled(1);
        statusBar()->showMessage("Visualizando RGB do kinect" + QString::number(index));
        createRGBWindowForDevice(index);


    }if(index == 0){

        m_mdiArea->closeAllSubWindows();
    }

    indexDeviceAnterior = index;
}

void MainWindow::saveXYZKinect(){

    QString textEditText =  indTextInput->text();
    QString boxText =  boxExpression->currentText();
    QString etapa = boxEtapa->currentText();

    qDebug()<<textEditText << boxText << etapa;

    QString path = "./capturas/" + textEditText + "/" + boxText  + "/" + etapa + "/Kinect";

    QDir dir(path);
    if (!dir.exists()){
         dir.mkpath(path);
    }
    w_depth->saveXYZ(path);
}

void MainWindow::saveXYZRealsense(){

    QString textEditText =  indTextInput->text();
    QString boxText =  boxExpression->currentText();
    QString etapa = boxEtapa->currentText();

    qDebug()<<textEditText << boxText << etapa;

    QString path = "./capturas/" + textEditText + "/" + boxText  + "/" + etapa + "/Realsense/";

    QDir dir(path);
    if (!dir.exists()){
         dir.mkpath(path);
    }

    m_realSense -> getSnapshot(path);
}

void MainWindow::teste(){
 QString textEditText =  indTextInput->text();
 QString boxText =  boxExpression->currentText();
 QString etapa = boxEtapa->currentText();

 qDebug()<<textEditText << boxText << etapa;

 QDir dir("./capturas/" + textEditText + "/" + boxText);
 if (!dir.exists())
     dir.mkpath(".");

}

void MainWindow::teste2(QString str){
 qDebug()<<str;
}
