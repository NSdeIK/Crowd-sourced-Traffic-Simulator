#ifndef VIDEO_PROCESSING_HPP
#define	VIDEO_PROCESSING_HPP

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <opencv/highgui.h>
#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"

// frame size
#define Frame_W 640
#define Frame_H 480

// video file descriptor
static int video_fd;

// traffic density
static int density = 0;

// v4l memory frame buffer
static uint8_t *frame_buffer;

// opencv mem storage
static CvMemStorage *storage;

// frame
static IplImage *frame;

// cascade file
static CvHaarClassifierCascade *cascade;

// inicialize video capture
int init_video(char * cascade_file); 

// get camera frame
int capture_frame();

// detect car & calc density
void detect(IplImage *img);

// request
int wioctl(int fd, unsigned long request, ...);


#endif	/* VIDEO_PROCESSING_HPP */

