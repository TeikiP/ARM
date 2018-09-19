#ifndef VERTICES_H
#define VERTICES_H

#include <QVector3D>
#include <QColor>

class Vertices
{
protected:
    QVector3D *vertCoords;
    QColor *vertColors;
    int size;

public:
    Vertices(int size);
    ~Vertices();

    void setColor(int x, int y, int z, int value);
    QColor getColor(int x, int y, int z);
};

#endif // VERTICES_H
