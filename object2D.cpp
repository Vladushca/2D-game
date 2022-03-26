#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"



Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


Mesh* object2D::CreateCircle(const std::string& name, float x, float y, float radius, glm::vec3 color, bool fill)
{
    int n_vertices = 100;
    std::vector<VertexFormat> vertices = {};
    float increment = 2.0f * (float)M_PI / n_vertices;

    for (float i = 0.0f; i <= 2.0f * (float)M_PI; i += increment) {
        vertices.push_back(VertexFormat(glm::vec3(radius * cos(i) + x, radius * sin(i) + y, 0), color));
    }

    Mesh* circle = new Mesh(name);
    std::vector<unsigned int> indices = {};

    for (int j = 0; j < vertices.size(); j++) {
        indices.push_back(j);
    }

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
    }


    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2D::CreateLive(std::string name, glm::vec3 leftBottomCorner, glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {

        VertexFormat(corner + glm::vec3(0.5, 0.5, 0), color),
        VertexFormat(corner + glm::vec3(0.2, 0.7, 0), color),
        VertexFormat(corner + glm::vec3(0.5, 0.7, 0), color),
        VertexFormat(corner + glm::vec3(0.8, 0.7, 0), color),

        VertexFormat(corner + glm::vec3(0.2, 0.9, 0), color),
        VertexFormat(corner + glm::vec3(0.8, 0.9, 0), color),

        VertexFormat(corner + glm::vec3(0.3, 1, 0), color),
        VertexFormat(corner + glm::vec3(0.7, 1, 0), color),

        VertexFormat(corner + glm::vec3(0.45, 1, 0), color),
        VertexFormat(corner + glm::vec3(0.55, 1, 0), color)

    };
    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0,1,2,
                                            0,2,3,
                                            1,2,4,
                                            2,3,5,
                                            2,4,6,
                                            2,5,7,
                                            2,6,8,
                                            2,7,9
    };

    square->InitFromData(vertices, indices);
    return square;
}