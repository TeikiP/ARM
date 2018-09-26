#ifndef VERTICES_H
#define VERTICES_H

#include <QtGui>

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

    void setColorAt(unsigned int x, unsigned int y, unsigned int z, float value);
    GLfloat getColorAt(unsigned int x, unsigned int y, unsigned int z);

    void setColorAt(unsigned int index, float value);
    GLfloat getColorAt(unsigned int index);
    GLfloat* getColors();

    void setCoordsAt(unsigned int index, unsigned int x, unsigned int y, unsigned int z);
    GLfloat getCoordsAt(unsigned int index);
    GLfloat* getCoords();

    unsigned int getSize();

    void readFile(const char* path);
};

#endif // VERTICES_H
