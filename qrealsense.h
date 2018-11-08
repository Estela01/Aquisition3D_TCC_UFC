#ifndef QREALSENSE_H
#define QREALSENSE_H
#include <librealsense2/rs.hpp>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>


using namespace rs2;

class QRealsense
{
public:
    QRealsense();
    void init();
    rs2::device dev;

public slots:
    void getSnapshot();

private:
     QMutex m_mutex;

};

#endif // QREALSENSE_H
