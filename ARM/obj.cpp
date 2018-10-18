#include "obj.h"
#include "tinyobjloader/tiny_obj_loader.h"

#include <iostream>
#include <QString>

#define TINYOBJLOADER_IMPLEMENTATION

Obj::Obj(QString filename)
{
    std::string inputfile = filename.toUtf8().constData();
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    int path_length = filename.length();
    int dir_length = filename.lastIndexOf(QString("/"));
    int file_length = path_length - dir_length - 1;

    filename.chop(file_length);
    std::string mtldir = filename.toUtf8().constData();

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), mtldir.c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];

                coords.push_back(vx);
                coords.push_back(vy);
                coords.push_back(vz);

                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(.5);
            }


            index_offset += fv;
            shapes[s].mesh.material_ids[f];
        }
    }
}


QVector<GLfloat> Obj::getCoords() {
    return coords;
}

QVector<GLfloat> Obj::getColors() {
    return colors;
}
