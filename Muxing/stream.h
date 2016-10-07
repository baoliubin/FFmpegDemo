#ifndef STREAM_H
#define STREAM_H
#include "ffmpeg_muxer.h"
#include "fileio.h"
typedef struct _OutputStream{
	AVStream *stream;

	AVFrame *videoFrame;
	AVFrame *audioFrame;
	int64_t nextPts;
	float tincr, tincr2;
	struct SwsContext *swsCtx;
	struct SwrContext *swrCtx;
}OutputStream;

extern int Add_audio_video_stream(OutputStream *videoStream, OutputStream *audioStream, AVFormatContext *fmtCtx, AVCodec *videoCodec, AVCodec *audioCodec, const FileIO & fileIO);

#endif // STREAM_H
