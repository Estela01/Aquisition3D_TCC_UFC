#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QToolBar>
#include <QLabel>
#include <QMenu>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include "qkinectsensor.h"
#include "qrealsense.h"
#include <QMdiArea>
#include "rgbwindow.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:


    void createActions();
    void createMenus();
    void createRGBWindow();
    void createRGBWindowForDevice(int indexDevice);




private:

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QActionGroup *alignmentGroup;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *boldAct;
    QAction *italicAct;
    QAction *leftAlignAct;
    QAction *rightAlignAct;
    QAction *justifyAct;
    QAction *centerAct;
    QAction *setLineSpacingAct;
    QAction *setParagraphSpacingAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLabel *infoLabel;
    QKinectSensor *m_kinect;
    QRealsense *m_realSense;
    QMdiArea *m_mdiArea;
    RGBWindow *m_rgb;
    RGBWindow *m_depth;
    QToolBar *fToolbar;
    QToolBar *bToolbar;
    QComboBox *boxDevice;
    QComboBox *boxDeviceDepth;
    int indexDeviceAnterior = -1;
     QPushButton *capture;

public slots:
    void newFile();
    void initKinect();
    void showSerialNumber();
    void changeStatus(int index);
    void saveXYZKinect();


signals:



};

#endif // MAINWINDOW_H
