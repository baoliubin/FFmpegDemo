#ifndef CONVERT_H
#define CONVERT_H
#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
extern "C"
{
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/samplefmt.h"
}
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/samplefmt.h"
#ifdef __cplusplus
}
#endif
#endif

int convert(unsigned char * src, int src_w, int src_h, AVPixelFormat src_pixfmt, unsigned char *dst, int dst_w, int dst_h, AVPixelFormat dst_pixfmt);

#endif // CONVERT_H
