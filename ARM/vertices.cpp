#include "vertices.h"

Vertices::Vertices(int size)
{
    this->vertCoords = new QVector3D[size]();
    this->vertColors = new QColor[size]();
    this->size = size;
}

Vertices::~Vertices()
{
    delete [] vertCoords;
    delete [] vertColors;
}

void Vertices::setColor(int x, int y, int z, int value)
{
    vertColors[x * size * size + y * size + z] = value;
}

QColor Vertices::getColor(int x, int y, int z)
{
    return vertColors[x * size * size + y * size + z];
}
