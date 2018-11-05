#ifndef QREALSENSE_H
#define QREALSENSE_H
#include <librealsense2/rs.hpp>
#include <QDebug>

using namespace rs2;

class QRealsense
{
public:
    QRealsense();
    void init();
    rs2::device dev;

private:

};

#endif // QREALSENSE_H
