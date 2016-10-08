#include "window.h"
#include "libavformat/avformat.h"
#include <QPainter>
Window::Window(QWidget *parent) : QOpenGLWidget(parent)
{
	timer = new QTimer(this);
	fps = 0;
	timer->start(1);
}

void Window::initializeGL()
{
	m_width = 512;
	m_height= 256;
	initializeOpenGLFunctions();
	resize(width(), height());
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	mModelMatrix.setToIdentity();
	initTextures();
	initShaders();
	initData();
	connect(&work, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SLOT(processVideoData(QByteArray,int,int,int)));
	emit work.work();
}
void Window::processVideoData(const QByteArray &data, int width, int height, int pixfmt)
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

	update();
}
void Window::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program.bind();
	draw();
	program.release();

	calcFPS();
	paintFPS();
}
void Window::resizeGL(int w, int h)
{
	float ratio = (float) w / h;
	float left = -ratio;
	float right = ratio;
	float bottom = -1.0f;
	float top = 1.0f;
	float n  = 1.0f;
	float f = 10.0f;
	mProjectionMatrix.setToIdentity();
	mProjectionMatrix.frustum(left, right, bottom, top, n, f);
}

void Window::initShaders()
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
	program.bindAttributeLocation("a_Color", 1);
	program.bindAttributeLocation("qt_MultiTexCoord0", 2);
	program.link();
	program.bind();

	mMVPMatrixHandle	= program.uniformLocation("qt_ModelViewProjectionMatrix");
	mVerticesHandle		= program.attributeLocation("qt_Vertex");
	mColorsHandle		= program.attributeLocation("a_Color");
	mTexCoordHandle		= program.attributeLocation("qt_MultiTexCoord0");
}
void Window::initTextures()
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
	texY->setMinificationFilter(QOpenGLTexture::Linear);
	texY->setMagnificationFilter(QOpenGLTexture::Linear);
	texY->setWrapMode(QOpenGLTexture::Repeat);

	texU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texU->setSize(m_width/2, m_height/2);
	texU->setFormat(QOpenGLTexture::LuminanceFormat);
	texU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	texU->setMinificationFilter(QOpenGLTexture::Linear);
	texU->setMagnificationFilter(QOpenGLTexture::Linear);
	texU->setWrapMode(QOpenGLTexture::Repeat);

	texV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	texV->setSize(m_width/2, m_height/2);
	texV->setFormat(QOpenGLTexture::LuminanceFormat);
	texV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
	texV->setMinificationFilter(QOpenGLTexture::Linear);
	texV->setMagnificationFilter(QOpenGLTexture::Linear);
	texV->setWrapMode(QOpenGLTexture::Repeat);
}

void Window::initData()
{
	vertices << QVector3D(-1, -1, 0.0f)
			 << QVector3D(1, -1, 0.0f)
			 << QVector3D(1, 1, 0.0f)
			 << QVector3D(-1, 1, 0.0f);
	texcoords << QVector2D(0, 0)
			  << QVector2D(1, 0)
			  << QVector2D(1, 1)
			  << QVector2D(0, 1);
	colors << QVector4D(1.0f, 0.0f, 0.0f, 1.0f)
		   << QVector4D(0.0f, 0.0f, 1.0f, 1.0f)
		   << QVector4D(0.0f, 1.0f, 0.0f, 1.0f);
	mViewMatrix.setToIdentity();
	mViewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.001f), QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 1.0f, 0.0f));
}
void Window::draw()
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

	//顶点
	program.enableAttributeArray(mVerticesHandle);
	program.setAttributeArray(mVerticesHandle, vertices.constData());
	//颜色
	program.enableAttributeArray(mColorsHandle);
	program.setAttributeArray(mColorsHandle, colors.constData());
	//纹理坐标
	program.enableAttributeArray(mTexCoordHandle);
	program.setAttributeArray(mTexCoordHandle, texcoords.constData());
	//MVP矩阵
	mMVPMatrix = mProjectionMatrix  * mViewMatrix * mModelMatrix;
	program.setUniformValue(mMVPMatrixHandle, mMVPMatrix);

	//pixFmt
	program.setUniformValue("pixFmt", m_pixFmt);
	qDebug() << m_pixFmt;
	//纹理
	//	texture->bind();
	//	program.setUniformValue("qt_Texture0", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texY->textureId());
	//	texY->bind();


	glActiveTexture(GL_TEXTURE1);
	//	texU->bind();
	glBindTexture(GL_TEXTURE_2D, texU->textureId());

	glActiveTexture(GL_TEXTURE2);
	//	texV->bind();
	glBindTexture(GL_TEXTURE_2D, texV->textureId());

	texY->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrY.data());

	texU->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrU.data());
	texV->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, arrV.data());

	program.setUniformValue("tex_y", 0);
	program.setUniformValue("tex_u", 1);
	program.setUniformValue("tex_v", 2);

	glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
	//	texture->release();
	program.disableAttributeArray(mVerticesHandle);
	program.disableAttributeArray(mColorsHandle);
}
void Window::calcFPS()
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
void Window::updateFPS(qreal v)
{
    fps = v;
}
void Window::paintFPS()
{
	QPainter painter(this);
	painter.setPen(Qt::green);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.drawText(10, 10, QString("FPS:%1").arg(QString::number(fps, 'f', 3)));
}
