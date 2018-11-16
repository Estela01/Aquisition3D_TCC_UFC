#include "qrealsense.h"
#include <iostream>
#include <QTextStream>
#include <QString>
#include <fstream>
#include <sstream>



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
      // dev = list.front();


    }
}



void QRealsense::getSnapshot(){

   // QMutexLocker locker( &m_mutex );
    std::string return_current_time_and_date();

    std::string stream_desc{};

    rs2::pointcloud pc;
    rs2::points points;
    rs2::pipeline pipe;

    pipe.start();


    auto frames = pipe.wait_for_frames();
    auto color = frames.get_color_frame();
    auto depth = frames.get_depth_frame();




    pc.map_to(color);
    points = pc.calculate(depth);

    pipe.stop();
    std::string end = "/home/latin/Desktop/UFC_ESTELA/TCC/build-Aquisition3D_Project-Desktop-Debug/capturas/";
    std::string endPly;
    endPly.append(end);
    endPly.append( "pointsRealsense_" + return_current_time_and_date() + ".ply");
    points.export_to_ply(endPly,color);


//    std::stringstream ss;
//    stream_desc = rs2_stream_to_string(color.get_profile().stream_type());
//    auto filename_png =  end + "_" + stream_desc + ".png";
//    save_to_png(filename_png.data(), color.get_width(), color.get_height(), color.get_bytes_per_pixel(),
//    color.get_data(), color.get_width() * color.get_bytes_per_pixel());

//    ss << "PNG snapshot was saved to " << filename_png << std::endl;
}
