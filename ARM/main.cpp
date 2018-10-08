#include "openglwindow.h"
#include "vertices.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

#include <iostream>
using namespace std;

#define PGM3D_PATH "../shepp_logan.pgm3d"

class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize() override;
    void render() override;

protected:

    void keyPressEvent(QKeyEvent *event) override;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    //GLfloat *m_vertices;
    QVector<GLfloat> m_vertices;
    //GLfloat *m_color;
    QVector<GLfloat> m_colors;
    int m_size;

    QOpenGLShaderProgram *m_program;
    int m_frame;

    int m_angleX;
    int m_angleY;
    int m_angleZ;

    bool m_triangle;
    bool m_point;
};

TriangleWindow::TriangleWindow()
    : m_program(0)
    , m_frame(0)
    , m_angleX(0)
    ,m_triangle(0)
    ,m_point(1)
{
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    TriangleWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);



    return app.exec();
}

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

void TriangleWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    Vertices *vertex = new Vertices();
    vertex->readFile(PGM3D_PATH);

    //m_size = vertex->getSize();
    //m_vertices = vertex->func2();
    m_vertices = vertex->getLimitsCoords();
    m_colors = vertex->getLimitsColors();
    m_size = m_vertices.size();
}

void TriangleWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 10000.0f);
    matrix.translate(0, 0, -100);
//    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
    matrix.rotate(m_angleX, 1, 0, 0);
    matrix.rotate(m_angleY, 0, 1, 0);
    matrix.rotate(m_angleZ, 0, 0, 1);

    m_program->setUniformValue(m_matrixUniform, matrix);

    if(m_point){
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, m_vertices.data());
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, m_colors.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnable(GL_PROGRAM_POINT_SIZE);

        //int size = m_size * m_size * m_size;
        int size = m_size / 3;
        glDrawArrays(GL_POINTS, 0, size);

        glDisable(GL_PROGRAM_POINT_SIZE);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
    else if(m_triangle){
        //TODO
        cerr << "Render in mode GL_TRIANGLE is not yet implemented!" << endl;
        throw ;
    }

    m_program->release();

    ++m_frame;
}


void TriangleWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up :
        m_angleX += 1;
        m_angleX %= 360;
        break;
    case Qt::Key_Down :
        m_angleX -= 1;
        m_angleX %= 360;
        break;
    case Qt::Key_Left :
        m_angleY -= 1;
        m_angleY %= 360;
        break;
    case Qt::Key_Right :
        m_angleY += 1;
        m_angleY %= 360;
        break;
    case Qt::Key_T :
        m_triangle = true;
        m_point = false;
        break;
    case Qt::Key_P :
        m_triangle = false;
        m_point = true;
        break;
    case Qt::Key_Escape :
        this->destroy();
        break;
    }
}
