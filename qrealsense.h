#ifndef QREALSENSE_H
#define QREALSENSE_H
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>
#include <librealsense2/rs_frame.hpp>

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>


using namespace rs2;

class QRealsense
{
public:
    QRealsense();
    void init();
    rs2::device dev;
    bool verifyRS();

public slots:
    void getSnapshot(QString path);

private:
     QMutex m_mutex;

};

#endif // QREALSENSE_H
