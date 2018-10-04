#include "qkinectsensor.h"
#include "mainwindow.h"
#include <cstdlib>
#include <cmath>
#include <cassert>
#include<QBuffer>
#include <iostream>
#include <cstddef>

#define FREENECT_FRAME_H 480
#define FREENECT_FRAME_W 640
#define FREENECT_FRAME_PIX (FREENECT_FRAME_H * FREENECT_FRAME_W)
#define FREENECT_VIDEO_RGB_SIZE (FREENECT_FRAME_PIX *3)

QKinectSensor *QKinectSensor::s_instace = 0;



QKinectSensor* QKinectSensor::instace(){
    if (s_instace == 0){

        s_instace = new QKinectSensor();
        s_instace -> init();
    }

    return s_instace;
}

QKinectSensor::QKinectSensor()
{}

int QKinectSensor::getNumberDevices(){
    int nr_devices = freenect_num_devices(m_ctx);
    qDebug()<<"numero de devices kinect found"<<nr_devices<<"\n";
    return nr_devices;
}

const char* QKinectSensor::getSerialNumber(){


    if(freenect_open_device(m_ctx, &m_dev, m_userDeviceNumber) < 0){
        qDebug()<<"Não foi possivel abrir o dispositivo";
       // exit(EXIT_FAILURE);

    } else {
        freenect_list_device_attributes(m_ctx, &attribute_list);

        return  attribute_list->camera_serial;
    }

    return "Error";

}


void QKinectSensor::init(){
    if (freenect_init(&m_ctx, NULL) < 0){
        qDebug()<<"freenect_init() failed\n";
        exit(EXIT_FAILURE);
    }
    freenect_set_log_level(m_ctx,FREENECT_LOG_DEBUG);


    int nr_devices = freenect_num_devices (m_ctx);
    qDebug()<<"Number of devices found: "<<nr_devices<<"\n";
    /// now allocate the buffers so we can fill them
    m_userDeviceNumber = 0;
//    m_bufferDepth.resize(FREENECT_RESOLUTION_MEDIUM);
    m_bufferVideo.resize(FREENECT_VIDEO_RGB_SIZE);


    printf("teste");

//    m_bufferVideo((freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB)).bytes);
//     m_bufferDepthRaw.resize(FREENECT_FRAME_PIX);
//     m_bufferDepthRaw16.resize(FREENECT_FRAME_PIX);
    m_gamma.resize(2048);
    /// open the device at present hard coded to device 0 as I only
    /// have 1 kinect
    /// \todo make this support multiple devices at some stage
    if (freenect_open_device(m_ctx, &m_dev, m_userDeviceNumber) < 0)
    {
    qDebug()<<"Could not open device\n";
    exit(EXIT_FAILURE);
    }


    /// build the gamma table used for the depth to rgb conversion
    /// taken from the demo programs
    for (int i=0; i<2048; ++i)
    {
    float v = i/2048.0;
    v = std::pow(v, 3)* 6;
    m_gamma[i] = v*6*256;
    }
    /// init our flags
    m_newRgbFrame=false;
//    m_newDepthFrame=false;
    m_deviceActive=true;
    // set our video formats to RGB by default
    /// \todo make this more flexible at some stage
    freenect_set_video_mode(m_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    //freenect_set_depth_mode(m_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
    /// hook in the callbacks
   // freenect_set_depth_callback(m_dev, depthCallback);
    freenect_set_video_callback(m_dev, videoCallback);
    // start the video and depth sub systems
    startVideo();
   // startDepth();
    // set the thread to be active and start
    m_process = new QKinectProcessEvents(m_ctx);
    m_process->setActive();
    m_process->start();

}

void QKinectSensor::showRGB(int index){

}


bool QKinectSensor::getRGB(std::vector<uint8_t> &o_buffer)
{
    /// this grabs the rgb data we first need to lock our mutex
    /// this will be unlocked on destruction of the locker
    QMutexLocker locker( &m_mutex );
    // do we have a new frame?
    if(m_newRgbFrame)
    {
        // if so swap out and fill
        o_buffer.swap(m_bufferVideo);
        m_newRgbFrame = false;
        return true;
    }
    else
    {
    return false;
    }
}


//----------------------------------------------------------------------------------------------------------------------
//bool QKinectSensor::getDepth(std::vector<uint8_t> &o_buffer)
//{
//    // this fills the depth buffer, first lock our mutex
//    QMutexLocker locker( &m_mutex );
//    if(m_newDepthFrame)
//    {
//        // swap data
//        o_buffer.swap(m_bufferDepth);
//        m_newDepthFrame = false;
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}


void QKinectSensor:: setVideoMode(int _mode )
{
 freenect_video_format vm= FREENECT_VIDEO_RGB;
 switch(_mode)
 {
  case 0 : { vm=FREENECT_VIDEO_RGB; break;}
  case 1 : { vm=FREENECT_VIDEO_YUV_RGB; break;}
  case 2 : { vm=FREENECT_VIDEO_IR_8BIT; break;}
  /*
  /// had issues with these modes so sticking to the 3 that work
   case 1 : { vm=FREENECT_VIDEO_BAYER; break;}
  case 2 : { vm=FREENECT_VIDEO_IR_8BIT; break;}
  case 3 : { vm=FREENECT_VIDEO_IR_10BIT; break;}
  case 4 : { vm=FREENECT_VIDEO_IR_10BIT_PACKED; break;}
  case 5 : { vm=FREENECT_VIDEO_YUV_RGB; break;}
  case 6 : { vm=FREENECT_VIDEO_YUV_RAW; break;}
   */
  default : qDebug()<<"index out of bounds for video mode\n";
       vm=FREENECT_VIDEO_RGB;
  break;
 }
 /// stop the video and set to new mode
 freenect_stop_video(m_dev);
 freenect_set_video_mode(m_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB) );
 freenect_start_video(m_dev);
}


void QKinectSensor::startVideo()
{
    if(freenect_start_video(m_dev) < 0)
    {
        throw std::runtime_error("Cannot start RGB callback");
    }
}

void QKinectSensor::stopVideo()
{
    if(freenect_stop_video(m_dev) < 0)
    {
        throw std::runtime_error("Cannot stop RGB callback");
    }
}
void QKinectSensor::startDepth()
{
    if(freenect_start_depth(m_dev) < 0)
    {
        throw std::runtime_error("Cannot start depth callback");
    }
}

void QKinectSensor::stopDepth()
{
    if(freenect_stop_depth(m_dev) < 0)
    {
        throw std::runtime_error("Cannot stop depth callback");
    }
}

void QKinectSensor::grabVideo(void *_video,uint32_t _timestamp)
{
    // lock our mutex and copy the data from the video buffer
    QMutexLocker locker( &m_mutex );
    uint8_t* rgb = static_cast<uint8_t*>(_video);
    std::copy(rgb, rgb+FREENECT_VIDEO_RGB_SIZE, m_bufferVideo.begin());
    m_newRgbFrame = true;
}

//void QKinectSensor::grabDepth(void *_depth,uint32_t _timestamp )
//{
//// this method fills all the different depth buffers at once
//// modifed from the sample code glview and cppview.cpp
///// lock our mutex
//QMutexLocker locker( &m_mutex );
//// cast the void pointer to the unint16_t the data is actually in
//uint16_t* depth = static_cast<uint16_t*>(_depth);

////TODO: PRECISA VER QUAL O VALOR MAXIMO PARA i
//// now loop and fill data buffers
//for( unsigned int i = 0 ; i < 2048 ; ++i)
//{
//    // first our two raw buffers the first will lose precision and may well
//    // be removed in the next iterations
//    m_bufferDepthRaw[i]=depth[i];
//    m_bufferDepthRaw16[i]=depth[i];

//    // now get the index into the gamma table
//    int pval = m_gamma[depth[i]];
//    // get the lower bit
//    int lb = pval & 0xff;
//    // shift right by 8 and determine which colour value to fill the
//    // array with based on the position
//    switch (pval>>8)
//    {
//    case 0:
//        m_bufferDepth[3*i+0] = 255;
//        m_bufferDepth[3*i+1] = 255-lb;
//        m_bufferDepth[3*i+2] = 255-lb;
//        break;
//    case 1:
//        m_bufferDepth[3*i+0] = 255;
//        m_bufferDepth[3*i+1] = lb;
//        m_bufferDepth[3*i+2] = 0;
//        break;
//    case 2:
//        m_bufferDepth[3*i+0] = 255-lb;
//        m_bufferDepth[3*i+1] = 255;
//        m_bufferDepth[3*i+2] = 0;
//        break;
//    case 3:
//        m_bufferDepth[3*i+0] = 0;
//        m_bufferDepth[3*i+1] = 255;
//        m_bufferDepth[3*i+2] = lb;
//        break;
//    case 4:
//        m_bufferDepth[3*i+0] = 0;
//        m_bufferDepth[3*i+1] = 255-lb;
//        m_bufferDepth[3*i+2] = 255;
//        break;
//    case 5:
//        m_bufferDepth[3*i+0] = 0;
//        m_bufferDepth[3*i+1] = 0;
//        m_bufferDepth[3*i+2] = 255-lb;
//        break;
//    default:
//        m_bufferDepth[3*i+0] = 0;
//        m_bufferDepth[3*i+1] = 0;
//        m_bufferDepth[3*i+2] = 0;
//        break;
//    }
//}
//// flag we have a new frame
//m_newDepthFrame = true;
//}


void QKinectProcessEvents::run()
{
    // loop while we are active and process the kinect event queue
    while(m_active)
    {
        //qDebug()<<"process thread\n";
        if(freenect_process_events(m_ctx) < 0)
        {
            throw std::runtime_error("Cannot process freenect events");
        }
    }
}

