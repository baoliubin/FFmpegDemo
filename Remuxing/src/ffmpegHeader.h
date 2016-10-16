#ifndef COMMON_H
#define COMMON_H


extern "C" {
#define __STDC_CONSTANT_MACROS
#include "libavutil/timestamp.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

}
extern void outError(int num);
#endif // COMMON_H
