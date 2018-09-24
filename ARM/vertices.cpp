#include "vertices.h"

#include <QFile>
#include <QTextStream>

/* Console output global variable */
QTextStream out(stdout);

/*********************CONSTRUCTORS & DESTRUCTORS*******************************/

/* Default constructor */
Vertices::Vertices()
{
    this->coords = NULL;
    this->colors = NULL;
    this->size = 0;
}

/* Constructor with known size */
Vertices::Vertices(int size)
{
    this->coords = new QVector3D[size * size * size]();
    this->colors = new QColor[size * size * size]();
    this->size = size;
}

/* Destructor */
Vertices::~Vertices()
{
    delete [] coords;
    delete [] colors;
}

/*************************GETTERS & SETTERS************************************/

void Vertices::setColor(unsigned int x, unsigned int y, unsigned int z, int value)
{
    colors[x * size * size + y * size + z] = value;
}

QColor Vertices::getColor(unsigned int x, unsigned int y, unsigned int z)
{
    return colors[x * size * size + y * size + z];
}

void Vertices::setColor(unsigned int index, int value)
{
    colors[index] = value;
}

QColor Vertices::getColor(unsigned int index)
{
    return colors[index];
}

void Vertices::setCoords(unsigned int index, unsigned int x, unsigned int y, unsigned int z)
{
    coords[index].setX(x);
    coords[index].setY(y);
    coords[index].setZ(z);
}

QVector3D Vertices::getCoords(int index)
{
    return coords[index];
}

int Vertices::getSize()
{
    return size;
}

/**************************FILE READING****************************************/

/* Reads all the necessary information from a file whose path is given as an
   argument and creates the corresponding vertices. */
void Vertices::readFile(const char* path)
{
    // Opening the file
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "Error reading from file!" << endl;
        return;
    }

    // First line ("PGM3D")
    QTextStream in(&file);
    QString line;
    if (in.readLine() != "PGM3D") {
        out << "Incorrect file format!" << endl;
        return;
    }

    // Second line (columns, lines, depth)
    unsigned int tmp;
    in >> this->size; //columns

    in >> tmp; //lines
    if (tmp != this->size) {
        out << "PGM3D file needs to be cubic!" << endl;
        return;
    }

    in >> tmp; //depth
    if (tmp != this->size) {
        out << "PGM3D file needs to be cubic!" << endl;
        return;
    }

    this->coords = new QVector3D[size * size * size]();
    this->colors = new QColor[size * size * size]();

    // Third line (max value)
    const short min_value = 0;
    short max_value;
    in >> max_value;

    // Remainling lines (values)
    unsigned int index = 0;
    short value;

    for (unsigned int x = 0; x < this->size; x++)
        for (unsigned int y = 0; y < this->size; y++)
            for (unsigned int z = 0; z < this->size; z++) {
                in >> value;

                // Bounds check
                if (value < min_value || value > max_value)
                    out << "Invalid color value in PGM3D file." << endl;

                // Set values
                this->setCoords(index, x, y, z);
                this->setColor(index, value);

                index++;
            }

    // Closing the file
    file.close();
}
