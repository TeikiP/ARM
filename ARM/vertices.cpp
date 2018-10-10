#include "vertices.h"

#include <QFile>
#include <QTextStream>

#define TRANSPARENCY_VALUE 0.05

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
        colors[index] = 254;
        colors[index + 1] = 1;
        colors[index + 2] = 1;
    }

    // DARK GREY -> CYAN
    else if (value == 51) {
        colors[index] = 1;
        colors[index + 1] = 254;
        colors[index + 2] = 254;
    }

    // LIGHT GREY -> YELLOW
    else if (value == 76) {
        colors[index] = 253;
        colors[index + 1] = 253;
        colors[index + 2] = 2;
    }

    // GRAY MIXES -> MAGENTA
    else if (value == 25 || value == 102) {
        colors[index] = 252;
        colors[index + 1] = 2;
        colors[index + 2] = 253;
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
void Vertices::readFile(const QString path)
{

    if(path.endsWith(".obj"))
        this->readFileObj(path);

    else if(path.endsWith(".pgm3d"))
        this->readFilePgm3d(path);

    else
        out << "Incorrect file format!" << endl;

}

void Vertices::readFilePgm3d(const QString path){

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

    for (int x = 0; x < (int) this->size; x++) {
        for (int y = 0; y  < (int) this->size; y++) {
            for (int z = 0; z  < (int) this->size; z++) {
                in >> value;

                // Bounds check
                if (value < min_value || value > max_value)
                    out << "Invalid color value in PGM3D file." << endl;

                // Set values
                this->setCoordsAt(index, x - offSet, y - offSet, z - offSet);
                this->setColorAt(index, value); //Correct modelisation
                //this->setCustomColorAt(index, value); //Demo and testing modelisation

                index += 3;
            }
        }
    }

    // Closing the file
    file.close();

    // Determine edges of volumes
    findLimitsIndices();
}

void Vertices::readFileObj(const QString path){

    // Opening the file
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "Error reading from file!" << endl;
        return;
    }
    // TODO obj
}

void Vertices::findLimitsIndices()
{
    QVector<int> limitsIndices(0);
    int index = 0, indexTop = 0, indexBottom = 0, indexLeft = 0, indexRight = 0, indexFront = 0, indexBehind = 0;
    int color = 0, colorTop = 0, colorBottom = 0, colorLeft = 0, colorRight = 0, colorFront = 0, colorBehind = 0;

    for (unsigned int z = 0; z < this->size; z++) {
        for (unsigned int x = 0; x < this->size; x++) {
            for (unsigned int y = 0; y < 3 * this->size; y += 3) {
                // indices
                indexLeft = index - 3;
                indexRight = index + 3;
                indexTop = index - 3*size;
                indexBottom = index + 3*size;
                indexFront = index - 3*size*size;
                indexBehind = index + 3*size*size;

                color = this->colors[index];

                // left & right
                if (indexLeft < 0) {
                    colorLeft = color;
                    colorRight = this->colors[indexRight];
                }
                else if (indexRight >= (int) this->size) {
                    colorLeft = this->colors[indexLeft];
                    colorRight = color;
                }
                else {
                    colorLeft = this->colors[indexLeft];
                    colorRight = this->colors[indexRight];
                }

                // top & bottom
                if (indexTop < 0) {
                    colorTop = color;
                    colorBottom = this->colors[indexBottom];
                }
                else if (indexBottom >= (int) this->size) {
                    colorTop = this->colors[indexTop];
                    colorBottom = color;
                }
                else {
                    colorTop = this->colors[indexTop];
                    colorBottom = this->colors[indexBottom];
                }

                // front & behind
                if (indexFront < 0) {
                    colorFront = color;
                    colorBehind = this->colors[indexBehind];
                }
                else if (indexBehind >= (int) this->size) {
                    colorFront = this->colors[indexFront];
                    colorBehind = color;
                }
                else {
                    colorFront = this->colors[indexFront];
                    colorBehind = this->colors[indexBehind];
                }


                // add coordinates of surfaces
                if (    color != colorLeft  || color != colorRight  ||
                        color != colorTop   || color != colorBottom ||
                        color != colorFront || color != colorBehind ) {
                    limitsIndices.push_back(index);
                    limitsIndices.push_back(index+1);
                    limitsIndices.push_back(index+2);
                }

                index += 3;

            }
        }
    }

    this->limitsIndices = limitsIndices;
}


QVector<GLfloat> Vertices::getLimitsCoords() {
    QVector<GLfloat> vec(0);

    int nb_vertices = this->limitsIndices.size();
    for (int i = 0; i < nb_vertices; i++)
        vec.push_back(this->coords[this->limitsIndices.at(i)]);


    return vec;
}

QVector<GLfloat> Vertices::getLimitsColors() {
    QVector<GLfloat> vec(0);

    int nb_vertices = this->limitsIndices.size();
    for (int i = 0; i < nb_vertices; i++)
        vec.push_back(1);

    return vec;
}


QVector<GLfloat> Vertices::getLimitsCoordsTriangles() {
    QVector<GLfloat> m_vertices_triangles(0);

    QVector<GLfloat> m_vertices = getLimitsCoords();

    int nb_vertices = this->limitsIndices.size();
    for (int i=0; i<nb_vertices; i+=3) {
        GLfloat x = m_vertices.at(i);
        GLfloat y = m_vertices.at(i+1);
        GLfloat z = m_vertices.at(i+2);

        //UNDER 1
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        //UNDER 2
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        //TOP 1
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        //TOP 2
        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        //FRONT 1
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        //FRONT 2
        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        //BACK 1
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        //BACK 2
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        //LEFT 1
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        //LEFT 2
        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);

        //RIGHT 1
        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        //RIGHT 2
        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y);
        m_vertices_triangles.push_back(z+1);

        m_vertices_triangles.push_back(x+1);
        m_vertices_triangles.push_back(y+1);
        m_vertices_triangles.push_back(z+1);
    }

    return m_vertices_triangles;
}

//TODO: Fix and use actual colors instead of placeholder
QVector<GLfloat> Vertices::getLimitsColorsTriangles() {
    QVector<GLfloat> m_colors_triangles(0);

    int nb_vertices = this->limitsIndices.size();
    for (int i=0; i<nb_vertices; i+=3) { // for every voxel
        for (int j=0; j<12; j++) { // for every triangle of the cube around the voxel
            for (int k=0; k<3; k++) { // for every vertice of the triangle
//                m_colors_triangles.push_back(this->colors[this->limitsIndices[i]]/255.0);
//                m_colors_triangles.push_back(this->colors[this->limitsIndices[i+1]]/255.0);
//                m_colors_triangles.push_back(this->colors[this->limitsIndices[i+2]]/255.0);
                m_colors_triangles.push_back(255.0);
                m_colors_triangles.push_back(255.0);
                m_colors_triangles.push_back(255.0);
                m_colors_triangles.push_back(TRANSPARENCY_VALUE);
            }
        }
    }

    return m_colors_triangles;
}
