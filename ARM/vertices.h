#ifndef VERTICES_H
#define VERTICES_H

#include <QVector3D>
#include <QColor>

class Vertices
{
private:
    QVector3D *coords;
    QColor *colors;
    unsigned int size;

public:
    Vertices();
    Vertices(int size);
    ~Vertices();

    void setColor(unsigned int x, unsigned int y, unsigned int z, int value);
    QColor getColor(unsigned int x, unsigned int y, unsigned int z);

    void setColor(unsigned int index, int value);
    QColor getColor(unsigned int index);

    void setCoords(unsigned int index, unsigned int x, unsigned int y, unsigned int z);
    QVector3D getCoords(int index);

    int getSize();

    void readFile(const char* path);
};

#endif // VERTICES_H
