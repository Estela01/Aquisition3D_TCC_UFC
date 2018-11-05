#include "qrealsense.h"


QRealsense::QRealsense()
{

}


void QRealsense::init(){

    rs2::context ctx;
    auto list = ctx.query_devices(); // Get a snapshot of currently connected devices
    if (list.size() == 0){
        qDebug() << "No realsense device found. Is it plugged in?";
    } else {
        qDebug() << "Realsense encontrada";
       dev = list.front();
    }
}
