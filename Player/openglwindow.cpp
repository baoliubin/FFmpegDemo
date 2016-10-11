#include "openglwindow.h"

#include "libavformat/avformat.h"
#include <QPainter>
#include <string.h>
OpenGLWindow::OpenGLWindow(QWidget *parent)
	: QOpenGLWidget(parent),
	  logger(0),
	  texY(0),
	  texU(0),
	  texV(0),
	  timer(0)
{
	fps = 0;
	frameNumber = 0;
	m_pixFmt = 0;
	arrY.clear();
	arrU.clear();
	arrV.clear();
	vertices.clear();
	texcoords.clear();

	startTimer(50);
}
OpenGLWindow::~OpenGLWindow()
{
	emit work.stop();
	makeCurrent();
	if (logger) {
		logger->stopLogging();
		delete logger;
		logger = NULL;
	}
	if (texY) {
		if (texY->isBound())
			texY->release();
		if (texY->isCreated())
			texY->destroy();
		delete texY;
		texY = nullptr;
	}
	if (texU) {
		if (texU->isBound())
			texU->release();
		if (texU->isCreated())
			texU->destroy();
		delete texU;
		texU = nullptr;
	}
	if (texV) {
		if (texV->isBound())
			texV->release();
		if (texV->isCreated())
			texV->destroy();
		delete texV;
		texV = nullptr;
	}
	if (timer) {
		timer->stop();
		delete timer;
		timer = NULL;
	}
	doneCurrent();
}
void OpenGLWindow::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event)
	int ret = 0;
	VideoData data = work.getData(ret);
	if (0 != ret) {
		processVideoData(data.data, data.width, data.heigth, data.pixfmt);
	}
}
void OpenGLWindow::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	resize(width(), height());

	glDepthMask(GL_TRUE);
	glEnable(GL_TEXTURE_2D);

	initTextures(0);
	initShaders();
	initData();
	logger = new QOpenGLDebugLogger(this->context());

	bool ret = logger->initialize(); // initializes in the current context, i.e. ctx
	if (ret) {
		connect(logger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(handleLoggedMessage(QOpenGLDebugMessage)));
		logger->startLogging();
	} else {
		qDebug() << "logger initialize failed";
	}
	mModelMatrix.setToIdentity();
	//	connect(&work, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SLOT(processVideoData(QByteArray,int,int,int)));
	emit work.work();
}
void OpenGLWindow::handleLoggedMessage(QOpenGLDebugMessage message)
{
	qDebug() << message;
}
void OpenGLWindow::processVideoData(const QByteArray &data, int width, int height, int pixfmt)
{
	arrY.clear();
	arrU.clear();
	arrV.clear();

	switch (pixfmt) {
		case AV_PIX_FMT_YUV444P:
			arrY = data.left(width * height);
			arrU = data.mid(width * height, width * height);
			arrV = data.mid(width * height * 2, width * height);
			m_pixFmt = 1;
			qDebug()<< "yuv444p";
			break;
		case AV_PIX_FMT_YUV420P:
			arrY = data.left(width * height);
			arrU = data.mid(width * height, width * height/4);
			arrV = data.mid(width * height* 5 / 4, width * height/4);
			m_pixFmt = 0;
			qDebug() << "yuv420p";
			break;
		default:
			qDebug() << "this format not support GPU speed up" << pixfmt;
			return ;
			break;
	}

	resize(width, height);
//	initTextures(m_pixFmt);
	update();
}

void OpenGLWindow::paintGL()
{
	glDepthMask(true);
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program.bind();
	draw();
	program.release();

	calcFPS();
	paintFPS();
}
void OpenGLWindow::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	float ratio = (float) w / h;
	float left = -ratio;
	float right = ratio;
	float bottom = -1.0f;
	float top = 1.0f;
	float n  = 1.0f;
	float f = 100.0f;
	mProjectionMatrix.setToIdentity();
	mProjectionMatrix.frustum(-1.0, 1.0, bottom, top, n, f);
}

void OpenGLWindow::initShaders()
{
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.vsh")) {
		qDebug() << __FILE__<<__FUNCTION__<< " add vertex shader file failed.";
		return ;
	}
	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.fsh")) {
		qDebug() << __FILE__<<__FUNCTION__<< " add fragment shader file failed.";
		return ;
	}
	program.bindAttributeLocation("qt_Vertex", 0);
	program.bindAttributeLocation("texCoord", 1);
	program.link();
	program.bind();

}
int OpenGLWindow::initTextures(int fmt)
{
	if (texY || texU || texV) {
		qDebug() << "texture has created";
		return 0;
	}

	int w	= width();
	int h	= height();
	if (fmt == 0) {
		//yuv420p
		texY = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texY->setFormat(QOpenGLTexture::LuminanceFormat);
		texY->setFixedSamplePositions(false);


		texY->setSize(w, h);
		texY->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

		texY->setMinificationFilter(QOpenGLTexture::Nearest);
		texY->setMagnificationFilter(QOpenGLTexture::Nearest);
		texY->setWrapMode(QOpenGLTexture::ClampToEdge);

		qDebug() << "texY isCreated:"<<texY->textureId()<<texY->isCreated()<<texY->width() << texY->height()<<texY->isStorageAllocated();

		texU = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texU->setFormat(QOpenGLTexture::LuminanceFormat);
		texU->setFixedSamplePositions(false);
		texU->setSize(w / 2, h / 2);
		texU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

		texU->setMinificationFilter(QOpenGLTexture::Nearest);
		texU->setMagnificationFilter(QOpenGLTexture::Nearest);
		texU->setWrapMode(QOpenGLTexture::ClampToEdge);


		texV = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texV->setFormat(QOpenGLTexture::LuminanceFormat);
		texV->setFixedSamplePositions(false);
		texV->setSize(w / 2, h / 2);
		texV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

		texV->setMinificationFilter(QOpenGLTexture::Nearest);
		texV->setMagnificationFilter(QOpenGLTexture::Nearest);
		texV->setWrapMode(QOpenGLTexture::ClampToEdge);


	} else {
		//yuv444p
		texY = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texY->setFormat(QOpenGLTexture::LuminanceFormat);
		texY->setSize(w, h);
		texY->create();
		texY->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
		texY->setMinificationFilter(QOpenGLTexture::Nearest);
		texY->setMagnificationFilter(QOpenGLTexture::Nearest);
		texY->setWrapMode(QOpenGLTexture::Repeat);

		texU = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texU->setFormat(QOpenGLTexture::LuminanceFormat);
		texU->setSize(w, h);
		texU->create();
		texU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
		texU->setMinificationFilter(QOpenGLTexture::Nearest);
		texU->setMagnificationFilter(QOpenGLTexture::Nearest);
		texU->setWrapMode(QOpenGLTexture::Repeat);

		texV = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texV->setFormat(QOpenGLTexture::LuminanceFormat);
		texV->setSize(w, h);
		texV->create();
		texV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
		texV->setMinificationFilter(QOpenGLTexture::Nearest);
		texV->setMagnificationFilter(QOpenGLTexture::Nearest);
		texV->setWrapMode(QOpenGLTexture::Repeat);
	}
	return 0;
}

void OpenGLWindow::initData()
{
	vertices << QVector3D(-1, 1, 0.0f)
			 << QVector3D(1, 1, 0.0f)
			 << QVector3D(1, -1, 0.0f)
			 << QVector3D(-1, -1, 0.0f);
	texcoords << QVector2D(0, 0)
			  << QVector2D(1, 0)
			  << QVector2D(1, 1)
			  << QVector2D(0, 1);

	mViewMatrix.setToIdentity();
	mViewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.001f), QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 1.0f, 0.0f));
}

void OpenGLWindow::draw()
{

	if (arrY.size() <=0 ) {
		qDebug() << "y array is empty";
		return ;
	}
	if (arrU.size() <=0 ) {
		qDebug() << "u array is empty";
		return ;
	}
	if (arrV.size() <=0 ) {
		qDebug() << "v array is empty";
		return ;
	}
	//	static int once = allocTexture();
	mModelMatHandle		= program.uniformLocation("u_modelMatrix");
	mViewMatHandle		= program.uniformLocation("u_viewMatrix");
	mProjectMatHandle	= program.uniformLocation("u_projectMatrix");
	mVerticesHandle		= program.attributeLocation("qt_Vertex");
	mTexCoordHandle		= program.attributeLocation("texCoord");

	//顶点
	program.enableAttributeArray(mVerticesHandle);
	program.setAttributeArray(mVerticesHandle, vertices.constData());

	//纹理坐标
	program.enableAttributeArray(mTexCoordHandle);
	program.setAttributeArray(mTexCoordHandle, texcoords.constData());

	//MVP矩阵
	program.setUniformValue(mModelMatHandle, mModelMatrix);
	program.setUniformValue(mViewMatHandle, mViewMatrix);
	program.setUniformValue(mProjectMatHandle, mProjectionMatrix);

	//pixFmt
	program.setUniformValue("pixFmt", m_pixFmt);
	qDebug() << "pixfmt" << m_pixFmt;
	//纹理
	if (m_pixFmt == 1) {
		//yuv444p
		texY->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrY.data());
		texU->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrU.data());
		texV->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrV.data());

		//Y
		glActiveTexture(GL_TEXTURE0);
		texY->bind();
		program.setUniformValue("tex_y", 0);

		//U
		glActiveTexture(GL_TEXTURE1);
		texU->bind();
		program.setUniformValue("tex_u", 1);

		//V
		glActiveTexture(GL_TEXTURE2);
		texV->bind();
		program.setUniformValue("tex_v", 2);

	} else {
		//yuv420p

		//Y
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texY->textureId());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width(), height(), 0, GL_RED, GL_UNSIGNED_BYTE, arrY.data());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//U
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texU->textureId());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width()/2, height()/2, 0, GL_RED, GL_UNSIGNED_BYTE, arrU.data());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//V
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texV->textureId());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width()/2, height()/2, 0, GL_RED, GL_UNSIGNED_BYTE, arrV.data());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		program.setUniformValue("tex_y", 0);
		program.setUniformValue("tex_u", 1);
		program.setUniformValue("tex_v", 2);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

	program.disableAttributeArray(mVerticesHandle);
	program.disableAttributeArray(mTexCoordHandle);
	texY->release();
	//	texU->release();
	//	texV->release();
}
void OpenGLWindow::calcFPS()
{
    static QTime time;
    static int once = [=](){time.start(); return 0;}();
    static int frame = 0;
    if (frame++ > 100) {
        qreal elasped = time.elapsed();
        updateFPS(frame/ elasped * 1000);
        time.restart();
        frame = 0;
    }
}
void OpenGLWindow::updateFPS(qreal v)
{
    fps = v;
}
void OpenGLWindow::paintFPS()
{
	frameNumber++;
	QPainter painter(this);
	painter.setPen(Qt::green);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.drawText(10, 10, QString("FPS:%1 %2").arg(QString::number(fps, 'f', 3)).arg(frameNumber));
}

