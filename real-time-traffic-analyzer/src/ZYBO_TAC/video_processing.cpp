#include "video_processing.hpp"

int wioctl(int fd, unsigned long request, ...) {

    int ioc;
    va_list list;

    while (((ioc = ioctl(fd, request, list)) == -1) && (EINTR == errno));

    return ioc;
}

int init_video(char * cascade_file) {

    struct v4l2_requestbuffers v4l2_req_buff;
    struct v4l2_buffer v4l2_buff;

    if (video_fd = open("/dev/video0", O_RDWR) == -1) {
        perror("Open Video: Failed to Open!");
        return -1;
    }

    storage = cvCreateMemStorage(0);
    cascade = (CvHaarClassifierCascade*) cvLoad(cascade_file);

    assert(cascade && storage);

    v4l2_req_buff.count = 1;
    v4l2_req_buff.memory = V4L2_MEMORY_MMAP;
    v4l2_req_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    v4l2_buff.index = 0;
    v4l2_buff.memory = V4L2_MEMORY_MMAP;
    v4l2_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (wioctl(video_fd, VIDIOC_REQBUFS, &v4l2_req_buff) == -1) {
        perror("Request Buffer failed!");
        return -2;
    }

    if (wioctl(video_fd, VIDIOC_QUERYBUF, &v4l2_buff) == -1) {
        perror("Query Buffer failed!");
        return -3;
    }

    frame_buffer = (uint8_t*) mmap(NULL, v4l2_buff.length, PROT_WRITE | PROT_READ, MAP_SHARED, video_fd, v4l2_buff.m.offset);

    return 0;
}

int capture_frame() {

    fd_set fd_s;
    struct timeval tv = {0};
    struct v4l2_buffer v4l2_buff;

    FD_ZERO(&fd_s);
    FD_SET(video_fd, &fd_s);

    v4l2_buff.index = 0;
    v4l2_buff.memory = V4L2_MEMORY_MMAP;
    v4l2_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (wioctl(video_fd, VIDIOC_QBUF, &v4l2_buff) == -1) {
        perror("Query Buffer failed!");
        return -1;
    }

    if (wioctl(video_fd, VIDIOC_STREAMON, &v4l2_buff.type) == -1) {
        perror("Stream failed!");
        return -2;
    }

    tv.tv_sec = 2;

    if (select(video_fd + 1, &fd_s, NULL, NULL, &tv) == -1) {
        perror("Frame failed!");
        return -3;
    }

    if (wioctl(video_fd, VIDIOC_DQBUF, &v4l2_buff)) {
        perror("Req frame failed!");
        return -4;
    }

    CvMat cvmat = cvMat(Frame_H, Frame_W, CV_8UC3, (void*) frame_buffer);

    frame = cvDecodeImage(&cvmat, 1);

    return 0;
}

void detect(IplImage *img) {

    CvSeq *object = cvHaarDetectObjects(
            img,
            cascade,
            storage,
            2, //	SCALE FACTOR
            1, //	MIN NEIGHBOURS
            CV_HAAR_DO_CANNY_PRUNING, //	CV_HAAR_DO_CANNY_PRUNING
            cvSize(35, 35), //	MIN OBJECT SIZE
            cvSize(45, 45) //	MAX OBJECT SIZE
            );

    density += object->total;
}
