#include "obj.h"
#include "tinyobjloader/tiny_obj_loader.h"

#include <iostream>
#include <QString>

#define TINYOBJLOADER_IMPLEMENTATION

Obj::Obj(QString filename)
{
    std::string inputfile = filename.toUtf8().constData();;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

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
            if (fv == 3) { // If the face is a triangle
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
            }

            else if (fv == 4) { // If the face is a quad, split into 2 triangles
                tinyobj::index_t idx;
                tinyobj::real_t vx, vy, vz;

                // Triangle 1 = {v0, v1, v2}
                for (size_t v = 0; v < 3; v++) {
                    // access to vertex
                    idx = shapes[s].mesh.indices[index_offset + v];
                    vx = attrib.vertices[3*idx.vertex_index+0];
                    vy = attrib.vertices[3*idx.vertex_index+1];
                    vz = attrib.vertices[3*idx.vertex_index+2];

                    coords.push_back(vx);
                    coords.push_back(vy);
                    coords.push_back(vz);

                    colors.push_back(255);
                    colors.push_back(255);
                    colors.push_back(255);
                    colors.push_back(.5);
                }


                // Triangle 1 = {v0, v2, v3}
                idx = shapes[s].mesh.indices[index_offset + 0];
                vx = attrib.vertices[3*idx.vertex_index+0];
                vy = attrib.vertices[3*idx.vertex_index+1];
                vz = attrib.vertices[3*idx.vertex_index+2];

                coords.push_back(vx);
                coords.push_back(vy);
                coords.push_back(vz);

                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(.5);

                idx = shapes[s].mesh.indices[index_offset + 2];
                vx = attrib.vertices[3*idx.vertex_index+0];
                vy = attrib.vertices[3*idx.vertex_index+1];
                vz = attrib.vertices[3*idx.vertex_index+2];

                coords.push_back(vx);
                coords.push_back(vy);
                coords.push_back(vz);

                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(.5);

                idx = shapes[s].mesh.indices[index_offset + 3];
                vx = attrib.vertices[3*idx.vertex_index+0];
                vy = attrib.vertices[3*idx.vertex_index+1];
                vz = attrib.vertices[3*idx.vertex_index+2];

                coords.push_back(vx);
                coords.push_back(vy);
                coords.push_back(vz);

                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(255);
                colors.push_back(.5);

                index_offset += fv;
            }

            else {
                std::cout << fv << "-vertices faces not supported." << std::endl;
                exit(1);
            }
        }
    }
}

QVector<GLfloat> Obj::getCoords() {
    return coords;
}

QVector<GLfloat> Obj::getColors() {
    return colors;
}
