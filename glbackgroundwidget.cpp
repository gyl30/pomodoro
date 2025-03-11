// glbackgroundwidget.cpp
#include "glbackgroundwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QOpenGLBuffer>

GLBackgroundWidget::GLBackgroundWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    connect(&timer_,
            &QTimer::timeout,
            this,
            [=]()
            {
                time_ += 0.03f;
                update();
            });
    timer_.start(30);
}

void GLBackgroundWidget::initializeGL()
{
    initializeOpenGLFunctions();

    shader_.addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
        attribute vec2 position;
        varying vec2 v_uv;
        void main() {
            v_uv = (position + 1.0) * 0.5;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )");

    shader_.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
        uniform float time;
        uniform vec2 resolution;
        varying vec2 v_uv;
        void main() {
            vec2 uv = v_uv;
            vec3 col = 0.7 + 0.3 * cos(time + vec3(uv.x * 8.0, uv.y * 8.0, uv.x + uv.y));
            gl_FragColor = vec4(col, 1.0);
        }
    )");

    shader_.link();
    shader_.bind();

    static const GLfloat vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

    shader_.enableAttributeArray("position");
    shader_.setAttributeArray("position", GL_FLOAT, vertices, 2);
}

void GLBackgroundWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void GLBackgroundWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    shader_.bind();
    shader_.setUniformValue("time", time_);
    shader_.setUniformValue("resolution", QSize(width(), height()));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    shader_.release();
}
