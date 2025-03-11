#ifndef GLBACKGROUNDWIDGET_H
#define GLBACKGROUNDWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>

class GLBackgroundWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLBackgroundWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    QOpenGLShaderProgram shader_;
    QTimer timer_;
    float time_ = 0.0f;
};

#endif
