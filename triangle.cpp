#include "triangle.h"
#include "common.h"

#include <qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QScreen>

#include <cmath>

Triangle::Triangle()
    : m_t(0)
    , m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &Triangle::handleWindowChanged);
}

void Triangle::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void Triangle::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Triangle::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Triangle::cleanup, Qt::DirectConnection);
        win->setClearBeforeRendering(false);
    }
}

void Triangle::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

TriangleRenderer::~TriangleRenderer()
{
    delete m_program;
}

void Triangle::sync()
{
    if (!m_renderer) {
        m_renderer = new TriangleRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &TriangleRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void TriangleRenderer::paint()
{
    if (!m_program) {
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

        m_program->link();

        m_posAttr = m_program->attributeLocation("posAttr");
        m_colAttr = m_program->attributeLocation("colAttr");
    }
    glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();

    /// Rotation
    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / m_window->screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    /// Vertexes values
    GLfloat vertices[] = {
        0.0f, 0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    /// Color values
    GLfloat colors[] = {
        (GLfloat)m_t, 0.0f, 0.0f,
        0.0f, (GLfloat)m_t, 0.0f,
        0.0f, 0.0f, (GLfloat)m_t
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}
