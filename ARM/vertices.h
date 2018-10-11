#ifndef VERTICES_H
#define VERTICES_H

#include <QtGui>
#include <QVector>

class Vertices
{
private:

    void readFileObj(const QString path);
    void readFilePgm3d(const QString path);

    GLfloat *coords;
    GLfloat *colors;
    QVector<int> limitsIndices;
    unsigned int size;

public:
    Vertices();
    Vertices(int size);
    ~Vertices();

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

    void findLimitsIndices();
    QVector<GLfloat> getLimitsCoords();
    QVector<GLfloat> getLimitsColors();
    QVector<GLfloat> getLimitsCoordsTriangles();
    QVector<GLfloat> getLimitsColorsTriangles();
    QVector<GLfloat> getLimitsCoordsCubes();
    QVector<GLfloat> getLimitsColorsCubes();
};

#endif // VERTICES_H
