/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include <stddef.h>
#include <sys/time.h>
enum v4l2_buf_type { V4L2_BUF_TYPE_VIDEO_CAPTURE = 1 };
#define V4L2_MEMORY_MMAP 1
#define V4L2_PIX_FMT_YUYV 1
#define V4L2_BUF_FLAG_ERROR 1
#define VIDIOC_QBUF 1
#define VIDIOC_S_DQBUF_TIMEOUT 2
#define VIDIOC_STREAMOFF 3
#define VIDIOC_REQBUFS 4
#define VIDIOC_QUERYBUF 5
#define VIDIOC_G_FMT 6
#define VIDIOC_STREAMON 7
#define VIDIOC_DQBUF 8
struct v4l2_buffer {
    int type, memory;
    unsigned int index, flags;
    size_t length, bytesused;
    union {
        size_t offset;
    } m;
};
struct v4l2_requestbuffers {
    unsigned int count;
    int type, memory;
};
struct v4l2_format {
    int type;
    union {
        struct {
            unsigned int width, height, pixelformat, bytesperline;
        } pix;
    } fmt;
};
