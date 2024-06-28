#ifndef DEFINE_H
#define DEFINE_H

// for data type
typedef unsigned char uchar;

// for frame
#define WIDTH 1280
#define HEIGHT 720
#define FPS 30

// for encode
#define TIMEBASE 90000

// format frame
enum class ImageFormat { YUV420, NV12, RGB };

// for custom widget render
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define ATTRIB_VERTEX 0
#define ATTRIB_TEXTURE 1

// for packet send and receive
#define PACKET_SIZE 1024

#endif