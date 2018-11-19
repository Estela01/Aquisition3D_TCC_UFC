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

bool QRealsense::verifyRS(){

    rs2::context ctx;
    auto list = ctx.query_devices(); // Get a snapshot of currently connected devices
    if (list.size() == 0){
        return 0;
    } else {
        return 1;
    }
}



void QRealsense::getSnapshot(QString path){

    std::string return_current_time_and_date();

    rs2::config cfg;
    rs2::pipeline pipe;

    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 60);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 60);

    rs2::pipeline_profile prf = pipe.start(cfg);
    auto stream = prf.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
    struct rs2_intrinsics intrin = stream.get_intrinsics();

//    Step 1) Align a whole depth frame to the corresponding color frame:
    rs2::frameset frames = pipe.wait_for_frames();

    rs2::align align(rs2_stream::RS2_STREAM_COLOR);
    rs2::frameset aligned_frame = align.process(frames);
    rs2::frame color_frame = frames.get_color_frame();
    rs2::frame depth_frame = aligned_frame.get_depth_frame();

////    Transform a pixel on depth frame to a point on 3D coordinates
//    rs2::depth_frame df = depth_frame.as<rs2::depth_frame>();
//    float d_pt[3] = { 3 };
//    float d_px[2] = { 100, 0 }; // where x and y are 2D coordinates for a pixel on depth frame
//    float depth = df.get_distance(100, 0);
//    rs2_deproject_pixel_to_point(d_pt, &intrin, d_px, depth);
//    d_pt[0], d_pt[1], and d_pt[2] respectively are X, Y, and Z on 3D coordinates in meter
//    std::cout<<(&d_pt[0]);

    rs2::pointcloud pc;
    rs2::points points;
    pc.map_to(color_frame);
    points = pc.calculate(depth_frame);

    QDir dir(path);
    if (!dir.exists()){
         dir.mkpath(".");
    }

    std::string end = path.toUtf8().constData();;
    std::string endPly;
    endPly.append(end);
    endPly.append( "Realsense_" + return_current_time_and_date() + ".ply");
    points.export_to_ply(endPly,color_frame);


}
