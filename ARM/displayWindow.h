#include "openglwindow.h"
#include "vertices.h"

class DisplayWindow : public OpenGLWindow
{
public:
    DisplayWindow();

    void initialize() override;
    void render() override;

protected:

    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QVector<GLfloat> m_vertices;
    QVector<GLfloat> m_vertices_triangles;
    QVector<GLfloat> m_colors;
    QVector<GLfloat> m_colors_triangles;

    QOpenGLShaderProgram *m_program;

    int m_size;
    int m_size_triangles;

    //Rotation
    int m_angleX;
    int m_angleY;

    //Translation
    float m_translateX;
    float m_translateY;
    float m_translateZ;


    //Renderer Mode
    bool m_triangle;
    bool m_point;

    //Interaction event
    QPoint m_last_pos;
};
