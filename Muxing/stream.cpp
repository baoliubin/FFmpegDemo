#include "stream.h"
#define HAVE_VIDEO 1
#define HAVE_AUDIO 1
#define STREAM_FRAME_RATE 25
static int add_video_stream(OutputStream *videoStream, AVFormatContext *fmtCtx, AVCodec **codec, enum AVCodecID codecID, int frameWidth, int frameHeight)
{
	*codec = avcodec_find_encoder(codecID);
	if (!(*codec)) {
		qDebug() << "find codec failed";
		return -1;
	}
	videoStream->stream = avformat_new_stream(fmtCtx, *codec);
	if (!videoStream->stream) {
		qDebug() << "cannot add new video stream";
		return -2;
	}
	videoStream->stream->id		= fmtCtx->nb_streams - 1;
	AVCodecContext *codecCtx	= videoStream->stream->codec;
	codecCtx->codec_id			= codecID;
	codecCtx->bit_rate			= 400000;
	codecCtx->width				= frameWidth;
	codecCtx->height			= frameHeight;
	AVRational r				= {1, STREAM_FRAME_RATE};
	codecCtx->time_base			= r;
	codecCtx->gop_size			= 12;
	codecCtx->pix_fmt			= AV_PIX_FMT_YUV420P;
	if (codecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
		codecCtx->max_b_frames = 2;
	}
	if (codecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
		codecCtx->mb_decision = 2;
	}
	if (fmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
		codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	return 1;
}
static void add_audio_stream()
{

}

int Add_audio_video_stream(OutputStream *videoStream, OutputStream *audioStream, AVFormatContext *fmtCtx, AVCodec *videoCodec, AVCodec *audioCodec, const FileIO & fileIO)
{
	int ret  = 0;
	AVOutputFormat *fmt = fmtCtx->oformat;
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		if (add_video_stream(videoStream, fmtCtx, &videoCodec, fmtCtx->video_codec_id, fileIO.width, fileIO.height)) {
			qDebug() << "add 1 video stream";
		}
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {

	}
}
