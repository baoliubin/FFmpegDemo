#include <QDebug>
#include "common.h"

static int open_encoder_muxer(AVOutputFormat **fmt, AVFormatContext **fmtCtx, const char * fileName)
{
	av_register_all();
	avformat_alloc_output_context2(fmtCtx, NULL, NULL, fileName);
	if (!fmtCtx) {
		qDebug() << "could not deduce output format from file extension. using MPEG";
        avformat_alloc_output_context2(fmtCtx, NULL, "mpeg", fileName);
	}
	if (!fmtCtx) {
		qDebug() << "Error: cannot alloc any format context.";
		return -1;
	}
	*fmt = (*fmtCtx)->oformat;
}
static void ouotError(int number)
{
	QByteArray error;
	error.resize(1024);
	av_strerror(number, error.data(), error.size());
	qDebug() << QString(error);
}
int main(int argc, char *argv[])
{
	FileIO fileIO;
	parseArgs(fileIO, argc, argv);

	AVOutputFormat	*fmt	= nullptr;
	AVFormatContext *fmtCtx	= nullptr;
    AVCodec	*videoCodec 	= nullptr;
    AVCodec *audioCodec     = nullptr;
	OutputStream *audioStream, *videoStream;
    open_encoder_muxer(&fmt, &fmtCtx, fileIO.outputFileName);
    Add_audio_video_stream(videoStream, audioStream,fmtCtx, videoCodec, audioCodec,fileIO);
	return 0;
}
