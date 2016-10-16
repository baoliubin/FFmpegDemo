#include "ffmpegHeader.h"
#include <QDebug>
#include <QByteArray>
void outError(int num)
{
    QByteArray error;
    error.resize(1024);
    av_strerror(num, error.data(), error.size());
    qDebug() << "FFMPEG ERROR:" << QString(error);
}
