#include "displayWindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QFileDialog>
#include <QtCore/qmath.h>

#include <iostream>
using namespace std;

#define PGM3D_PATH "../shepp_logan.pgm3d"


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


DisplayWindow::DisplayWindow()
    : m_program(0)
    ,m_angleX(0)
    ,m_angleY(0)
    ,m_triangle(0)
    ,m_point(0)
    ,m_cube(1)
{
    m_translateZ = 100.;
}


void DisplayWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");;
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    //QString filename = QFileDialog::getOpenFileName(0, tr("Open File"), ".", tr("Files (*.pgm3d *.obj)"));
    QString filename = PGM3D_PATH;

    Vertices vertex;
    vertex.readFile(filename);

    m_vertices = vertex.getLimitsCoords();
    m_colors = vertex.getLimitsColors();
    m_size = m_vertices.size();

    m_vertices_triangles = vertex.getLimitsCoordsTriangles();
    m_colors_triangles = vertex.getLimitsColorsTriangles();
    m_size_triangles = m_vertices_triangles.size();

    m_vertices_cubes = vertex.getLimitsCoordsCubes();
    m_colors_cubes = vertex.getLimitsColorsCubes();
    m_size_cubes = m_vertices_cubes.size();
}

void DisplayWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 10000.0f);

    matrix.translate(m_translateX, m_translateY, -m_translateZ);
    matrix.rotate(m_angleX, 1, 0, 0);
    matrix.rotate(m_angleY, 0, 1, 0);


    m_program->setUniformValue(m_matrixUniform, matrix);

    if(m_point){
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, m_vertices.data());
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, m_colors.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_POINTS, 0, m_size / 3);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    else if(m_triangle){
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, m_vertices_triangles.data());
        glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, m_colors_triangles.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, m_size_triangles / 3);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    else if(m_cube){
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, m_vertices_cubes.data());
        glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, m_colors_cubes.data());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, m_size_cubes / 3);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }


    m_program->release();

}

void DisplayWindow::keyPressEvent(QKeyEvent *event)
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
            m_cube = false;
            break;

        case Qt::Key_P :
            m_triangle = false;
            m_point = true;
            m_cube = false;
            break;

        case Qt::Key_C :
            m_triangle = false;
            m_point = false;
            m_cube = true;
            break;

        case Qt::Key_L :
            this->initialize();
            break;

        case Qt::Key_Escape :
            this->destroy();
            break;
    }
}

void DisplayWindow::wheelEvent(QWheelEvent *event)
{
    m_translateZ *= 1.0 + (-1.0 * event->delta() / 1200.0);
}

void DisplayWindow::mousePressEvent(QMouseEvent *event)
{
    m_last_pos = event->pos();
}

void DisplayWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_last_pos.x();
    int dy = event->y() - m_last_pos.y();

    if (event->buttons() & Qt::LeftButton)
    {
          m_angleY += dx;
          m_angleX += dy;
    }
    else if(event->buttons() & Qt::RightButton)
    {
        m_translateY -= dy;
        m_translateX += dx;
    }

    m_last_pos = event->pos();
}