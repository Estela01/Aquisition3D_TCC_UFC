#include "qkinectsensor.h"
#include "mainwindow.h"
#include <cstdlib>
#include <cmath>
#include <cassert>
#include<QBuffer>
#include <iostream>
#include <cstddef>
//#include <chrono>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <iostream>
#define FREENECT_FRAME_H 480
#define FREENECT_FRAME_W 640
#define FREENECT_FRAME_PIX (FREENECT_FRAME_H * FREENECT_FRAME_W)
#define FREENECT_VIDEO_RGB_SIZE (FREENECT_FRAME_PIX *3)



std::string  QKinectSensor::date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
//   qDebug()<< ss.str().c_str();
    return ss.str();
}


std::string return_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

QKinectSensor *QKinectSensor::s_instace = 0;

auto start = std::chrono::system_clock::now();


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

const char* QKinectSensor::getSerialNumber(int index){

    freenect_list_device_attributes(m_ctx, &attribute_list);
    freenect_device_attributes *p;
    int i = 0;
    for(p = attribute_list; p!=NULL ; p = p->next){

        if(i == index){
            return p->camera_serial;
        }
       i++;
    }
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
    //m_bufferDepth.resize(FREENECT_FRAME_PIX);
    m_bufferDepth.resize(FREENECT_FRAME_PIX);
    m_bufferVideo.resize(FREENECT_VIDEO_RGB_SIZE);

    m_bufferDepthRaw.resize(FREENECT_FRAME_PIX);
    m_bufferDepthRaw16.resize(FREENECT_FRAME_PIX);
    m_gamma.resize(2048);
    m_dev.resize(nr_devices);

    /// build the gamma table used for the depth to rgb conversion
    /// taken from the demo programs
    for (int i=0; i<2048; ++i)
    {
    float v = i/2048.0;
    v = std::pow(v, 3)* 6;
    m_gamma[i] = v*6*256;
    }

    m_newRgbFrame=false;
    m_newDepthFrame=false;
    m_deviceActive=true;


    //Open all devices founded
    for (int i = 0; i < nr_devices ; i ++){
        if (freenect_open_device(m_ctx, &m_dev[i], i) < 0)
            {
            qDebug()<<"Could not open device\n";
            //return
            exit(EXIT_FAILURE);
            }
       }
}

void QKinectSensor::setDeviceToShowRGB(int index){

    m_userDeviceNumber = index - 1;
    // set our video formats to RGB by default
    /// \todo make this more flexible at some stage
    freenect_set_video_mode(m_dev[m_userDeviceNumber], freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    freenect_set_depth_mode(m_dev[m_userDeviceNumber], freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));
    /// hook in the callbacks
    freenect_set_depth_callback(m_dev[m_userDeviceNumber], depthCallback);
    freenect_set_video_callback(m_dev[m_userDeviceNumber], videoCallback);
    // start the video and depth sub systems
    startVideo(m_userDeviceNumber);
    startDepth(m_userDeviceNumber);
    // set the thread to be active and start
    m_process = new QKinectProcessEvents(m_ctx);
    m_process->setActive();
    m_process->start();


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



bool QKinectSensor::getDepth(std::vector<uint16_t> &o_buffer)
{
    // this fills the depth buffer, first lock our mutex
    QMutexLocker locker( &m_mutex );
    if(m_newDepthFrame)
    {
        // swap data
        o_buffer.swap(m_bufferDepth);
        m_newDepthFrame = false;
        return true;
    }
    else
    {
        return false;
    }
}


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
 /// TODO: lock out, m_dev[0] only?
 freenect_stop_video(m_dev[0]);
 freenect_set_video_mode(m_dev[0], freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB) );
 freenect_start_video(m_dev[0]);
}


void QKinectSensor::startVideo(int index)
{
    if(freenect_start_video(m_dev[index]) < 0)
    {
        throw std::runtime_error("Cannot start RGB callback");
    }

    auto dateTime = date();

    qDebug() << dateTime.c_str() << " -> Start Video"  ;


}

void QKinectSensor::stopVideo(int index)
{
    if(freenect_stop_video(m_dev[index]) < 0)
    {
        throw std::runtime_error("Cannot stop RGB callback");
    }
    auto dateTime = date();

    qDebug() << dateTime.c_str() <<  " -> Stop Video" ;

}
void QKinectSensor::startDepth(int index)
{
    if(freenect_start_depth(m_dev[index]) < 0)
    {
        throw std::runtime_error("Cannot start depth callback");
    }
    auto dateTime = date();

    qDebug() << dateTime.c_str() <<  " -> Start Depth"  ;
}

void QKinectSensor::stopDepth(int index)
{
    if(freenect_stop_depth(m_dev[index]) < 0)
    {
        throw std::runtime_error("Cannot stop depth callback");
    }

    auto dateTime = date();

    qDebug() << dateTime.c_str() <<  " -> Start Depth"  ;
}

void QKinectSensor::grabVideo(void *_video,uint32_t _timestamp)
{
    // lock our mutex and copy the data from the video buffer
    QMutexLocker locker( &m_mutex );
    uint8_t* rgb = static_cast<uint8_t*>(_video);
    std::copy(rgb, rgb+FREENECT_VIDEO_RGB_SIZE, m_bufferVideo.begin());
    m_newRgbFrame = true;
}

void QKinectSensor::grabDepth(void *_depth,uint32_t _timestamp )
{
// this method fills all the different depth buffers at once
// modifed from the sample code glview and cppview.cpp
/// lock our mutex
QMutexLocker locker( &m_mutex );
// cast the void pointer to the unint16_t the data is actually in
uint16_t* depth = static_cast<uint16_t*>(_depth);

// fill data buffers with data
    copy(depth, depth+ FREENECT_FRAME_PIX, m_bufferDepth.begin());
    m_newDepthFrame = true;

//// now loop and fill data buffers
//for( unsigned int i = 0 ; i < FREENECT_FRAME_H*FREENECT_FRAME_W ; ++i)
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
}

void QKinectSensor::setAngle(int index, double _angle)
{
    // constrain the angle from -30 - +30
    if(_angle > 30)
    {
        _angle = 30;
    }
    else if(_angle <-30)
    {
        _angle=-30;
    }
    freenect_set_tilt_degs(m_dev[index],_angle);
}



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

