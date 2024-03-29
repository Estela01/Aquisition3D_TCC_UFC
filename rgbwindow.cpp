
/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */
#include "rgbwindow.h"
#include <iostream>
#include <QDebug>
#include "qkinectsensor.h"
#include <fstream>
#include <sstream>
#include<QImage>
#include <opencv/cv.h>

//----------------------------------------------------------------------------------------------------------------------
RGBWindow::RGBWindow(QWidget *_parent ) : QGLWidget(_parent)
{

  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
    // start the timer to re-draw as quick as possible
    startTimer(0);
    // create space for our image data
    m_rgb.resize(640*480*3);
    m_depth.resize(640*480*3);
    // default to RGB mode
    m_mode=0;
    count=0;
}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void RGBWindow::initializeGL()
{

      glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
      // enable depth testing for drawing

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0);
    // set the texture data
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, & m_rgbTexture);
    glBindTexture(GL_TEXTURE_2D, m_rgbTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void RGBWindow::resizeGL(
                        int _w,
                        int _h
                       )
{
    glViewport(0,0,_w,_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, 640, 480, 0, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void RGBWindow::paintGL()
{
    // ok this is using immediate mode GL but will do for a sample
    // see my other NGL versions which are much faster

    QKinectSensor *kinect= QKinectSensor::instace();

    kinect->getRGB(m_rgb);
    if(m_mode ==0)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, m_rgbTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_rgb[0]);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(255.0f, 255.0f, 255.0f, 255.0f);
            glTexCoord2f(0, 0); glVertex3f(0,0,0);
            glTexCoord2f(1, 0); glVertex3f(640,0,0);
            glTexCoord2f(1, 1); glVertex3f(640,480,0);
            glTexCoord2f(0, 1); glVertex3f(0,480,0);
        glEnd();

    }else if(m_mode == 1)
    {
        kinect->getDepth(m_depth);
        count++;
    }
    //std::vector<uint8_t> m_rgb;
    //if (saveXYZ = true) {
    //
    //  x = m_rgb[3*i+0];
    //  y = m_rgb[3*i+1];
    //  z = m_rgb[3*i+2];
    //
    //
    //}




}

//----------------------------------------------------------------------------------------------------------------------
void RGBWindow::mouseMoveEvent ( QMouseEvent * _event )
{

  updateGL();
}


//----------------------------------------------------------------------------------------------------------------------
void RGBWindow::mousePressEvent ( QMouseEvent * _event )
{

}

//----------------------------------------------------------------------------------------------------------------------
void RGBWindow::mouseReleaseEvent (QMouseEvent * _event)
{
  // this event is called when the mouse button is released
  // we then set Rotate to false

}

void RGBWindow::timerEvent(QTimerEvent *_event)
{
    // re-draw GL
    updateGL();

}

RGBWindow::~RGBWindow()
{

}

void RGBWindow::saveXYZ(QString path){

    const int width = 640;
    const int height = 480;

    std::vector<uint16_t> m_depth_temp;
    m_depth_temp = m_depth;

    std::vector<uint8_t> m_rgb_temp;
    m_rgb_temp = m_rgb;


     QKinectSensor *kinect= QKinectSensor::instace();
     auto serialNumber = kinect->getSerialNumber(m_devIndex);
     auto dateTime = kinect->date();
    // std::string return_current_time_and_date();




     std::string str =  path.toUtf8().constData() + std::string("/") + std::string(serialNumber) + dateTime.c_str() + ".jpg";

     QImage image(m_rgb_temp.data(), 640, 480, QImage::Format_RGB888);
     image.save(str.c_str());

     std::ofstream myfile ( path.toUtf8().constData() + std::string("/") + std::string(serialNumber) + dateTime.c_str() + ".xyz");
     if (myfile.is_open()){

     for (int i = 0; i < 640; i++){
         for (int j = 0; j < 480; j++){
           int offset = i + j * 640;
           float d = m_depth_temp[offset];
           static const double fx = 572.882768;
           static const double fy = 542.739980;
           static const double cx = 314.649173;
           static const double cy = 240.160459;

           float z = (d) / (1.0f);
           float x = (i - cx) * z / fx;  // X = (x - cx) * d / fx
           float y = (j - cy) * z / fy;  // Y = (y - cy) * d / fy
           myfile << x/1000 << " " << y/1000 << " " << z/1000 << "\n" ;
         }
     }

     myfile.close();
     } else std::cout << "Unable to open file";

     qDebug() << "File saved";

}
