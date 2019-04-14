#include "qrealsense.h"
#include <iostream>
#include <QTextStream>
#include <QString>
#include <fstream>
#include <sstream>
// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



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

    rs2::context ctx;
    auto list = ctx.query_devices();

    rs2::config cfg;
    rs2::pipeline pipe;



    //rs2::colorizer color_map;




    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGBA8, 60);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 60);



    rs2::pipeline_profile prf = pipe.start(cfg);
    auto stream = prf.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
    rs2::device selected_device = prf.get_device();
    auto depth_sensor = selected_device.first<rs2::depth_sensor>();

//    if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
//            {
//              //depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 1.f); // Enable emitter
//              //depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f); // Disable emitter
//            }
//            if (depth_sensor.supports(RS2_OPTION_LASER_POWER))
//            {
               // Query min and max values:
               //auto range = depth_sensor.get_option_range(RS2_OPTION_LASER_POWER);
              //depth_sensor.set_option(RS2_OPTION_LASER_POWER, range.max); // Set max power
              //depth_sensor.set_option(RS2_OPTION_ACCURACY, 1);
              depth_sensor.set_option(RS2_OPTION_FILTER_OPTION, 4);
              //depth_sensor.set_option(RS2_OPTION_LASER_POWER, 0.f); // Disable laser
//            }

    struct rs2_intrinsics intrin = stream.get_intrinsics();

//    Step 1) Align a whole depth frame to the corresponding color frame:
    for (auto i = 0; i < 30; ++i) pipe.wait_for_frames();
    rs2::frameset frames = pipe.wait_for_frames();

    rs2::align align(rs2_stream::RS2_STREAM_COLOR);
//    rs2::frameset aligned_frame = align.process(frames);
    rs2::frame color_frame = frames.get_color_frame();
    rs2::frame depth_frame = frames.get_depth_frame();



    if (auto vf = depth_frame.as<rs2::video_frame>());


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

    std::string endPng = endPly.append("Realsense_" + return_current_time_and_date() + ".png");
    stbi_write_png(endPng.c_str(), color_frame.as<rs2::video_frame>().get_width(), color_frame.as<rs2::video_frame>().get_height(),
                  color_frame.as<rs2::video_frame>().get_bytes_per_pixel(), color_frame.as<rs2::video_frame>().get_data(),color_frame.as<rs2::video_frame>().get_stride_in_bytes());

    endPly.append( "Realsense_" + return_current_time_and_date() + ".ply");
    points.export_to_ply(endPly,color_frame);

}
