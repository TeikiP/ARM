#include "vertices.h"

#include <iostream>

using namespace std;

#define PGM3D_PATH "../shepp_logan.pgm3d"

int main()
{    
    Vertices vert;
    vert.readFile(PGM3D_PATH);    
    
    cout << vert.getCoords(58917).x() << endl;
    cout << vert.getCoords(58917).y() << endl;
    cout << vert.getCoords(58917).z() << endl;
    cout << vert.getColor(58917).value() << endl;

    return 0;
}
