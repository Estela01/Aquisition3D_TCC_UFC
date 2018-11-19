#ifndef QKINECTSENSOR
#define QKINECTSENSOR


#include <QObject>

#include <QThread>
#include <stdexcept>
#include <QMutex>
#include <QMutexLocker>
#include <vector>

#include "libfreenect.h"
#include <QDebug>
#include <libfreenect_registration.h>
#include <QDir>


class QKinectProcessEvents : public QThread
{
public :
    inline QKinectProcessEvents(freenect_context *_ctx ){m_ctx=_ctx;}
    inline void setActive(){m_active=true;}
    inline void setInActive(){m_active=false;}
protected :
    void run();

private :

    bool m_active;
    freenect_context *m_ctx;

};




class QKinectSensor : public QObject
{
    Q_OBJECT

public:
//    QKinectSensor();
    static QKinectSensor* instace();
    int getNumberDevices();
    const char* getSerialNumber(int index);
    void setDeviceToShowRGB(int index);

    std::vector<freenect_device*> m_dev;


public slots:
    bool getRGB(std::vector<uint8_t> &o_buffer);
    bool getDepth( std::vector<uint16_t> &o_buffer);
    void setVideoMode(int _mode);
    void startDepth(int index);
    void stopDepth(int index);
    void startVideo(int index);
    void stopVideo(int index);
    void setAngle(int index , double _angle);

private:



    static QKinectSensor* s_instace;

    QKinectSensor();



    void init();

    freenect_context *m_ctx;

    freenect_device_attributes *attribute_list;
    int m_userDeviceNumber;
    QMutex m_mutex;
    bool m_newRgbFrame;
    bool m_newDepthFrame;

    std::vector<uint8_t> m_bufferVideo;
    std::vector<uint16_t> m_bufferDepth;
    std::vector<uint8_t> m_bufferDepthRaw;
    std::vector<uint16_t> m_bufferDepthRaw16;

    std::vector<uint16_t> m_gamma;
    bool m_deviceActive;
    QKinectProcessEvents *m_process;
    void grabDepth( void *_depth,uint32_t timestamp );
    void grabVideo(void *_video,uint32_t timestamp );
    static inline void depthCallback(freenect_device *_dev, void *_depth, uint32_t _timestamp=0 ){
             QKinectSensor *kinect=QKinectSensor::instace();
             kinect->grabDepth(_depth,_timestamp);
         }

   static inline void videoCallback(freenect_device *_dev, void *_video,  uint32_t _timestamp=0) {
             QKinectSensor *kinect=QKinectSensor::instace();
             kinect->grabVideo(_video, _timestamp);
         }

};









#endif // QKINECTSENSOR

