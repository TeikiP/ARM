#ifndef VERTICES_H
#define VERTICES_H

#include <QtGui>
#include <QVector>

class Vertices
{
private:
    GLfloat *coords;
    GLfloat *colors;
    unsigned int size;

public:
    Vertices();
    Vertices(int size);
    ~Vertices();

    void setColorAt(int x, int y, int z, float value);
    GLfloat getColorAt(int x, int y, int z);

    void setColorAt(unsigned int index, float value);
    void setCustomColorAt(unsigned int index, float value);
    GLfloat getColorAt(unsigned int index);
    GLfloat* getColors();

    void setCoordsAt(unsigned int index, int x, int y, int z);
    GLfloat getCoordsAt(unsigned int index);
    GLfloat* getCoords();

    unsigned int getSize();

    void readFile(const char* path);

    QVector<GLfloat> func();
    QVector<GLfloat> func2();
    //QVector<GLfloat> func_colors(QVector<GLfloat> coords);
};

#endif // VERTICES_H
