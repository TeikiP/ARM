#ifndef OBJ_H
#define OBJ_H

#include <QtGui>
#include <QVector>

class Obj
{
private :
    QVector<GLfloat> coords;
    QVector<GLfloat> colors;
    unsigned int size;

public:
    Obj(QString filename);
    QVector<GLfloat> getCoords();
    QVector<GLfloat> getColors();
};

#endif // OBJ_H
