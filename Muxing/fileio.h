#ifndef FILEIO_H
#define FILEIO_H
#include <QDebug>
typedef struct _FileIO{
	const char * inputVideoFileName;
	const char * inputAudioFileName;
	const char * outputFileName;
	int width;
	int height;
}FileIO;
static QDebug operator<< (QDebug dbg, const _FileIO & args);

extern void parseArgs(FileIO & fileIO, int argc, char *argv[]);
#endif // FILEIO_H
