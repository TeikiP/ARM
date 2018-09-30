#include "vertices.h"

#include <QFile>
#include <QTextStream>
#include<iostream>

using namespace std;
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
    this->coords = new GLfloat[size * size * size * 3]();
    this->colors = new GLfloat[size * size * size]();
    this->size = size;
}

/* Destructor */
Vertices::~Vertices()
{
    delete [] coords;
    delete [] colors;
}

/*************************GETTERS & SETTERS************************************/

void Vertices::setColorAt(unsigned int x, unsigned int y, unsigned int z, float value)
{
    colors[x * size + y + z * size * size] = value;
}

GLfloat Vertices::getColorAt(unsigned int x, unsigned int y, unsigned int z)
{
    return colors[x * size + y + z * size * size];
}

void Vertices::setColorAt(unsigned int index, float value)
{
    colors[index] = value;
}

GLfloat Vertices::getColorAt(unsigned int index)
{
    return colors[index];
}

GLfloat* Vertices::getColors()
{
    return colors;
}

void Vertices::setCoordsAt(unsigned int index, unsigned int x, unsigned int y, unsigned int z)
{
    coords[index] = x;
    coords[index+1] = y;
    coords[index+2] = z;
}

GLfloat Vertices::getCoordsAt(unsigned int index)
{
    return coords[index];
}

GLfloat* Vertices::getCoords()
{
    return coords;
}


unsigned int Vertices::getSize()
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

    this->coords = new GLfloat[size * size * size * 3];
    this->colors = new GLfloat[size * size * size];

    // Third line (max value)
    const short min_value = 0;
    short max_value;
    in >> max_value;

    // Remainling lines (values)
    unsigned int indexCoords = 0, indexColors = 0;
    short value;

    for (unsigned int z = 0; z < this->size; z++){
        for (unsigned int x = 0; x < this->size; x++){
            for (unsigned int y = 0; y < this->size; y++) {
                in >> value;

                // Bounds check
                if (value < min_value || value > max_value)
                    out << "Invalid color value in PGM3D file." << endl;

                // Set values
                this->setCoordsAt(indexCoords, this->size - x - 1, y, z); // this->size - x - 1 => image a l'endroit et bas gauche image centre du rendu
                this->setColorAt(indexColors, value);

                indexCoords += 3;
                indexColors++;

            }
        }
    }

    // Closing the file
    file.close();
}
