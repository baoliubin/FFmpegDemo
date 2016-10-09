#include "openglwindow.h"
#include "libavformat/avformat.h"
#include <QPainter>
OpenGLWindow::OpenGLWindow(QWidget *parent) : QOpenGLWidget(parent)
{
	fps = 0;
	frameNumber = 0;
	startTimer(50);
}
OpenGLWindow::~OpenGLWindow()
{
}
void OpenGLWindow::timerEvent(QTimerEvent *event)
{
	int ret = 0;
	VideoData data = work.getData(ret);
	if (0 != ret) {
		processVideoData(data.data, data.width, data.heigth, data.pixfmt);
	}
}
void OpenGLWindow::initializeGL()
{
	m_width = width();
	m_height= height();
	initializeOpenGLFunctions();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	resize(width(), height());

	glDepthMask(GL_TRUE);
	glEnable(GL_TEXTURE_2D);

	initTextures();
	initShaders();
	initData();

	mModelMatrix.setToIdentity();
//	connect(&work, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SLOT(processVideoData(QByteArray,int,int,int)));
	emit work.work();
}
void OpenGLWindow::processVideoData(const QByteArray &data, int width, int height, int pixfmt)
{
	switch (pixfmt) {
		case AV_PIX_FMT_YUV420P: {
			arrY = data.left(width * height);
			arrU = data.mid(width * height, width * height/4);
			arrV = data.mid(width * height* 5 / 4, width * height/4);
			m_pixFmt = 0;
			break;
		}
		case AV_PIX_FMT_YUV444P: {
			arrY = data.left(width * height);
			arrU = data.mid(width * height, width * height);
			arrV = data.mid(width * height * 2, width * height);
			m_pixFmt = 1;
			break;
		}
		default: {
			qDebug() << "this format not support GPU speed up" << pixfmt;
			return ;
			break;
		}
	}
	qDebug() << arrY.size() << arrU.size() << arrV.size();
	resize(width, height);
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
void OpenGLWindow::initTextures()
{
	//    texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
	//    texture->setSize(width(), height());
	//    texture->setFormat(QOpenGLTexture::LuminanceFormat);
	//    texture->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	//    texture->setMinificationFilter(QOpenGLTexture::Linear);
	//    texture->setMagnificationFilter(QOpenGLTexture::Linear);
	//    texture->setWrapMode(QOpenGLTexture::Repeat);
	texY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texY->setSize(m_width, m_height);
	texY->setFormat(QOpenGLTexture::LuminanceFormat);
	texY->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	texY->setMinificationFilter(QOpenGLTexture::Nearest);
	texY->setMagnificationFilter(QOpenGLTexture::Nearest);
	texY->setWrapMode(QOpenGLTexture::Repeat);

	texU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texU->setSize(m_width, m_height);
	texU->setFormat(QOpenGLTexture::LuminanceFormat);
	texU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	texU->setMinificationFilter(QOpenGLTexture::Nearest);
	texU->setMagnificationFilter(QOpenGLTexture::Nearest);
	texU->setWrapMode(QOpenGLTexture::Repeat);

	texV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texV->setSize(m_width, m_height);
	texV->setFormat(QOpenGLTexture::LuminanceFormat);
	texV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	texV->setMinificationFilter(QOpenGLTexture::Nearest);
	texV->setMagnificationFilter(QOpenGLTexture::Nearest);
	texV->setWrapMode(QOpenGLTexture::Repeat);

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
	//	mMVPMatrix = mProjectionMatrix  * mViewMatrix * mModelMatrix;
	//	program.setUniformValue(mMVPMatrixHandle, mMVPMatrix);

	program.setUniformValue(mModelMatHandle, mModelMatrix);
	program.setUniformValue(mViewMatHandle, mViewMatrix);
	program.setUniformValue(mProjectMatHandle, mProjectionMatrix);

	//pixFmt
	program.setUniformValue("pixFmt", m_pixFmt);
	qDebug() << m_pixFmt;
	//纹理
	texY->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrY.data());
	texU->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrU.data());
	texV->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrV.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texY->textureId());
	program.setUniformValue("tex_y", 0);
	texY->bind();

	glActiveTexture(GL_TEXTURE1);
	texU->bind();
	glBindTexture(GL_TEXTURE_2D, texU->textureId());
	program.setUniformValue("tex_u", 1);

	glActiveTexture(GL_TEXTURE2);
	texV->bind();
	glBindTexture(GL_TEXTURE_2D, texV->textureId());
	program.setUniformValue("tex_v", 2);

	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

	program.disableAttributeArray(mVerticesHandle);
	program.disableAttributeArray(mTexCoordHandle);
	texY->release();
	texU->release();
	texV->release();
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
