#ifndef WINDOW_H
#define WINDOW_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include "encodertmp.h"
class Window :public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = 0);
protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

	void initShaders();
	void initTextures();

	void initData();
	void draw();
	void calcFPS();
	void updateFPS(qreal);
	void paintFPS();
signals:

public slots:
	void processVideoData(const QByteArray &data, int width, int height, int pixfmt);
private:
	int m_width, m_height;
	GLint m_pixFmt;
	QByteArray arrY, arrU, arrV;
	QOpenGLShaderProgram program;
//	QOpenGLTexture *texture;
	QOpenGLTexture *texY, *texU, *texV;
	QVector<QVector3D> vertices;
	QVector<QVector4D> colors;
	QVector<QVector2D> texcoords;
	int mMVPMatrixHandle;
	int mVerticesHandle;
	int mColorsHandle;
	int mTexCoordHandle;

	QMatrix4x4  mModelMatrix;
	QMatrix4x4 mViewMatrix;
	QMatrix4x4 mProjectionMatrix;
	QMatrix4x4 mMVPMatrix;
	QTimer *timer;
	qreal angleInDegrees;
	qreal fps;

	Work work;
};

#endif // WINDOW_H
