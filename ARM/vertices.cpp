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
    this->coords = new GLfloat[size * size * size * 3]();
    this->colors = new GLfloat[size * size * size * 3]();
    this->size = size;
}

/* Destructor */
Vertices::~Vertices()
{
    delete [] coords;
    delete [] colors;
}

/*************************GETTERS & SETTERS************************************/

void Vertices::setColorAt(int x, int y, int z, float value)
{
    colors[x * size + y + z * size * size] = value;
}

GLfloat Vertices::getColorAt(int x, int y, int z)
{
    return colors[x * size + y + z * size * size];
}

void Vertices::setColorAt(unsigned int index, float value)
{
    colors[index] = value;
    colors[index + 1] = value;
    colors[index + 2] = value;
}

void Vertices::setCustomColorAt(unsigned int index, float value)
{
    // WHITE -> RED
    if (value == 255) {
        colors[index] = 255;
        colors[index + 1] = 0;
        colors[index + 2] = 0;
    }

    // DARK GREY -> CYAN
    else if (value == 51) {
        colors[index] = 0;
        colors[index + 1] = 255;
        colors[index + 2] = 255;
    }

    // LIGHT GREY -> YELLOW
    else if (value == 76) {
        colors[index] = 255;
        colors[index + 1] = 255;
        colors[index + 2] = 0;
    }

    // GRAY MIXES -> MAGENTA
    else if (value == 25 || value == 102) {
        colors[index] = 255;
        colors[index + 1] = 0;
        colors[index + 2] = 255;
    }

    //default
    else {
        colors[index] = value;
        colors[index + 1] = value;
        colors[index + 2] = value;
    }
}

GLfloat Vertices::getColorAt(unsigned int index)
{
    return colors[index];
}

GLfloat* Vertices::getColors()
{
    return colors;
}

void Vertices::setCoordsAt(unsigned int index, int x, int y, int z)
{
    coords[index] = x;
    coords[index + 1] = y;
    coords[index + 2] = z;
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
    this->colors = new GLfloat[size * size * size * 3];

    // Third line (max value)
    const short min_value = 0;
    short max_value;
    in >> max_value;

    // Remaining lines (values)
    unsigned int index = 0;
    int offSet = this->size / 2;
    short value;

    for (int x = this->size-1; x >= 0; x--) {
        for (int y = this->size-1; y >= 0; y--) {
            for (int z = this->size-1; z >= 0; z--) {
                in >> value;

                // Bounds check
                if (value < min_value || value > max_value)
                    out << "Invalid color value in PGM3D file." << endl;

                // Set values
                //this->setCoordsAt(index, this->size - x - 1, y, z); // this->size - x - 1 => image in place and bottom left image = center of rendering
                this->setCoordsAt(index, x - offSet, y - offSet, z - offSet);
                //this->setColorAt(index, value); //Correct modelisation
                this->setCustomColorAt(index, value); //Demo and testing modelisation

                index += 3;
            }
        }
    }

    // Closing the file
    file.close();
}

//not finish
QVector<GLfloat> Vertices::func()
{
    QVector<GLfloat> vec(0);
    unsigned int index = 0, indexTop = 0, indexBottom = 0, indexLeft = 0, indexRight = 0, indexFront = 0, indexBehind = 0;

    for (unsigned int z = 0; z < this->size; z++){
        for (unsigned int x = 0; x < this->size; x++){
            for (unsigned int y = 0; y < this->size; y++) {

                if( z == 0 )
                {
                    if(x == 0 && y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                }

                else if (z == this->size - 1)
                {
                    if(x == 0 && y == 0 )
                    {
                        indexRight =  z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                    else
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                    }
                }
                else
                {
                    if(x == 0 && y == 0 )
                    {
                        indexRight =  z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 && y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( y == 0 )
                    {
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( y == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == 0 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else if( x == this->size - 1 )
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                    else
                    {
                        indexLeft = z * size * size * size + x * size + (y - 1);
                        indexRight = z * size * size * size + x * size + (y + 1);
                        indexTop = z * size * size * size + (x - 1) * size + y;
                        indexBottom = z * size * size * size + (x + 1) * size + y;
                        indexFront = (z - 1) * size * size * size + x * size + y;
                        indexBehind = (z + 1) * size * size * size + x * size + y;
                    }
                }

            }
        }
    }

    return vec;
}
