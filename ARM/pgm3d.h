#ifndef PGM3D_H
#define PGM3D_H

#include <QtGui>
#include <QVector>

class Pgm3d
{
private:

    void findLimitsIndices();

    GLfloat *coords;
    GLfloat *colors;
    QVector<int> limitsIndices;
    unsigned int size;

public:
    Pgm3d();
    Pgm3d(int size);
    Pgm3d(const QString size);
    ~Pgm3d();

    void setColorAt(unsigned int index, float value);
    void setCustomColorAt(unsigned int index, float value);
    GLfloat getColorAt(int x, int y, int z);
    GLfloat getColorAt(unsigned int index);
    GLfloat* getColors();

    void setCoordsAt(unsigned int index, int x, int y, int z);
    GLfloat getCoordsAt(unsigned int index);
    GLfloat* getCoords();

    unsigned int getSize();

    void readFile(const QString path);


    QVector<GLfloat> getLimitsCoords();
    QVector<GLfloat> getLimitsColors();
    QVector<GLfloat> getLimitsCoordsTriangles();
    QVector<GLfloat> getLimitsColorsTriangles();
    QVector<GLfloat> getLimitsCoordsCubes();
    QVector<GLfloat> getLimitsColorsCubes();
};

#endif // PGM3D_H
