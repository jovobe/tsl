#include "tsl/algorithm/generator.hpp"

namespace tsl {

tmesh tmesh_cube(uint32_t size, double edge_length) {
    tmesh out;
    // =============================
    // Front side
    // =============================

    // Create vertices
    vector<vertex_handle> front_vertices;
    front_vertices.reserve(size * size);
    for (uint32_t x = 0; x < size; ++x)
    {
        for (uint32_t z = 0; z < size; ++z)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>(x * edge_length),
                    static_cast<double>(0),
                    static_cast<double>(z * edge_length)
                )
            );
            front_vertices.push_back(vh);
        }
    }

    // Create faces
    for (uint32_t x = 0; x < size - 1; ++x)
    {
        for (uint32_t z = 0; z < size - 1; ++z)
        {
            // Current vertex
            auto bottom_right = ((x + 1) * size) + z;
            auto top_right = bottom_right + 1;
            auto top_left = (x * size) + (z + 1);
            auto bottom_left = top_left - 1;

            out.add_face(
                {
                    front_vertices[bottom_right],
                    front_vertices[top_right],
                    front_vertices[top_left],
                    front_vertices[bottom_left]
                }
            );
        }
    }

    // =============================
    // Left side
    // =============================

    // Create vertices
    vector<vertex_handle> left_vertices;
    left_vertices.reserve((size - 1) * size);
    for (uint32_t y = 1; y < size; ++y)
    {
        for (uint32_t z = 0; z < size; ++z)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>(0),
                    static_cast<double>(y * edge_length),
                    static_cast<double>(z * edge_length)
                )
            );
            left_vertices.push_back(vh);
        }
    }

    // Add front connected faces
    for (uint32_t z = 0; z < size - 1; ++z)
    {
        out.add_face(
            {
                front_vertices[z],
                front_vertices[z + 1],
                left_vertices[z + 1],
                left_vertices[z]
            }
        );
    }

    // Create faces
    for (uint32_t y = 0; y < size - 2; ++y)
    {
        for (uint32_t z = 0; z < size - 1; ++z)
        {
            // Current vertex
            auto top_right = (y * size) + (z + 1);
            auto bottom_right = top_right - 1;
            auto bottom_left = ((y + 1) * size) + z;
            auto top_left = bottom_left + 1;

            out.add_face(
                {
                    left_vertices[bottom_right],
                    left_vertices[top_right],
                    left_vertices[top_left],
                    left_vertices[bottom_left]
                }
            );
        }
    }

    // =============================
    // Back side
    // =============================

    // Create vertices
    vector<vertex_handle> back_vertices;
    back_vertices.reserve((size - 1) * size);
    for (uint32_t x = 1; x < size; ++x)
    {
        for (uint32_t z = 0; z < size; ++z)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>(x * edge_length),
                    static_cast<double>((size - 1) * edge_length),
                    static_cast<double>(z * edge_length)
                )
            );
            back_vertices.push_back(vh);
        }
    }

    // Add left connected faces
    for (uint32_t z = 0; z < size - 1; ++z)
    {
        out.add_face(
            {
                left_vertices[((size - 2) * size) + z],
                left_vertices[((size - 2) * size) + z + 1],
                back_vertices[z + 1],
                back_vertices[z]
            }
        );
    }

    // Create faces
    for (uint32_t x = 0; x < size - 2; ++x)
    {
        for (uint32_t z = 0; z < size - 1; ++z)
        {
            // Current vertex
            auto top_right = (x * size) + (z + 1);
            auto bottom_right = top_right - 1;
            auto bottom_left = ((x + 1) * size) + z;
            auto top_left = bottom_left + 1;

            out.add_face(
                {
                    back_vertices[bottom_right],
                    back_vertices[top_right],
                    back_vertices[top_left],
                    back_vertices[bottom_left]
                }
            );
        }
    }

    // =============================
    // Right side
    // =============================

    // Create vertices
    vector<vertex_handle> right_vertices;
    right_vertices.reserve((size - 2) * size);
    for (uint32_t y = 1; y < size - 1; ++y)
    {
        for (uint32_t z = 0; z < size; ++z)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>((size - 1) * edge_length),
                    static_cast<double>(y * edge_length),
                    static_cast<double>(z * edge_length)
                )
            );
            right_vertices.push_back(vh);
        }
    }

    // Add back connected faces
    for (uint32_t z = 0; z < size - 1; ++z)
    {
        out.add_face(
            {
                back_vertices[((size - 2) * size) + z],
                back_vertices[((size - 2) * size) + z + 1],
                right_vertices[((size - 3) * size) + z + 1],
                right_vertices[((size - 3) * size) + z]
            }
        );
    }

    // Create faces
    for (uint32_t y = 0; y < size - 3; ++y)
    {
        for (uint32_t z = 0; z < size - 1; ++z)
        {
            // Current vertex
            auto top_right = ((y + 1) * size) + (z + 1);
            auto bottom_right = top_right - 1;
            auto bottom_left = (y * size) + z;
            auto top_left = bottom_left + 1;

            out.add_face(
                {
                    right_vertices[bottom_right],
                    right_vertices[top_right],
                    right_vertices[top_left],
                    right_vertices[bottom_left]
                }
            );
        }
    }

    // Add front connected faces
    for (uint32_t z = 0; z < size - 1; ++z)
    {
        out.add_face(
            {
                right_vertices[z],
                right_vertices[z + 1],
                front_vertices[((size - 1) * size) + z + 1],
                front_vertices[((size - 1) * size) + z]
            }
        );
    }

    // =============================
    // Top side
    // =============================

    // Create vertices
    vector<vertex_handle> top_vertices;
    top_vertices.reserve((size - 2) * (size - 2));
    for (uint32_t x = 1; x < size - 1; ++x)
    {
        for (uint32_t y = 1; y < size - 1; ++y)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>(x * edge_length),
                    static_cast<double>(y * edge_length),
                    static_cast<double>((size - 1) * edge_length)
                )
            );
            top_vertices.push_back(vh);
        }
    }

    // Create faces
    for (uint32_t x = 0; x < size - 3; ++x)
    {
        for (uint32_t y = 0; y < size - 3; ++y)
        {
            // Current vertex
            auto bottom_right = ((x + 1) * (size - 2)) + y;
            auto top_right = bottom_right + 1;
            auto bottom_left = (x * (size - 2)) + y;
            auto top_left = bottom_left + 1;

            out.add_face(
                {
                    top_vertices[bottom_right],
                    top_vertices[top_right],
                    top_vertices[top_left],
                    top_vertices[bottom_left]
                }
            );
        }
    }

    // Add front and back connected faces
    for (uint32_t x = 1; x < size - 2; ++x)
    {
        // front connected
        out.add_face(
            {
                front_vertices[x * (size) + (size - 1)],
                front_vertices[(x + 1) * (size) + (size - 1)],
                top_vertices[x * (size - 2)],
                top_vertices[(x - 1) * (size - 2)]
            }
        );

        // back connected
        out.add_face(
            {
                top_vertices[(x - 1) * (size - 2) + (size - 3)],
                top_vertices[x * (size - 2) + (size - 3)],
                back_vertices[(x * size) + (size - 1)],
                back_vertices[((x - 1) * size) + (size - 1)]
            }
        );
    }

    // Add left and right connected faces
    for (uint32_t y = 1; y < size - 2; ++y)
    {
        // left connected
        out.add_face(
            {
                left_vertices[y * (size) + (size - 1)],
                left_vertices[(y - 1) * (size) + (size - 1)],
                top_vertices[y - 1],
                top_vertices[y]
            }
        );

        // right connected
        out.add_face(
            {
                top_vertices[((size - 2) * (size - 3) + y)],
                top_vertices[((size - 2) * (size - 3) + (y - 1))],
                right_vertices[((y - 1) * size) + (size - 1)],
                right_vertices[(y * size) + (size - 1)]
            }
        );
    }

    // Add missing faces in corners
    out.add_face(
        {
            front_vertices[size - 1],
            front_vertices[(2 * size) - 1],
            top_vertices[0],
            left_vertices[size - 1]
        }
    );
    out.add_face(
        {
            left_vertices[((size - 1) * size) - 1],
            left_vertices[((size - 2) * size) - 1],
            top_vertices[size - 3],
            back_vertices[size - 1]
        }
    );
    out.add_face(
        {
            back_vertices[((size - 1) * size) - 1],
            back_vertices[((size - 2) * size) - 1],
            top_vertices[((size - 2) * (size - 2)) - 1],
            right_vertices[((size - 2) * size) - 1]
        }
    );
    out.add_face(
        {
            front_vertices[((size - 1) * size) - 1],
            front_vertices[((size) * size) - 1],
            right_vertices[size - 1],
            top_vertices[((size - 3) * (size - 2))]
        }
    );

    // =============================
    // Bottom side
    // =============================

    // Create vertices
    vector<vertex_handle> bottom_vertices;
    bottom_vertices.reserve((size - 2) * (size - 2));
    for (uint32_t x = 1; x < size - 1; ++x)
    {
        for (uint32_t y = 1; y < size - 1; ++y)
        {
            auto vh = out.add_vertex(
                vec3(
                    static_cast<double>(x * edge_length),
                    static_cast<double>(y * edge_length),
                    static_cast<double>(0)
                )
            );
            bottom_vertices.push_back(vh);
        }
    }

    // Create faces
    for (uint32_t x = 0; x < size - 3; ++x)
    {
        for (uint32_t y = 0; y < size - 3; ++y)
        {
            // Current vertex
            auto bottom_right = ((x + 1) * (size - 2)) + (y + 1);
            auto top_right = bottom_right - 1;
            auto bottom_left = (x * (size - 2)) + (y + 1);
            auto top_left = bottom_left - 1;

            out.add_face(
                {
                    bottom_vertices[bottom_right],
                    bottom_vertices[top_right],
                    bottom_vertices[top_left],
                    bottom_vertices[bottom_left]
                }
            );
        }
    }

    // Add front and back connected faces
    for (uint32_t x = 1; x < size - 2; ++x)
    {
        // front connected
        out.add_face(
            {
                front_vertices[(x + 1) * (size)],
                front_vertices[x * (size)],
                bottom_vertices[(x - 1) * (size - 2)],
                bottom_vertices[x * (size - 2)]
            }
        );

        // back connected
        out.add_face(
            {
                bottom_vertices[x * (size - 2) + (size - 3)],
                bottom_vertices[(x - 1) * (size - 2) + (size - 3)],
                back_vertices[((x - 1) * size)],
                back_vertices[(x * size)]
            }
        );
    }

    // Add left and right connected faces
    for (uint32_t y = 1; y < size - 2; ++y)
    {
        // left connected
        out.add_face(
            {
                left_vertices[(y - 1) * (size)],
                left_vertices[y * (size)],
                bottom_vertices[y],
                bottom_vertices[y - 1]
            }
        );

        // right connected
        out.add_face(
            {
                bottom_vertices[((size - 2) * (size - 3) + (y - 1))],
                bottom_vertices[((size - 2) * (size - 3) + y)],
                right_vertices[(y * size)],
                right_vertices[((y - 1) * size)]
            }
        );
    }

    // Add missing faces in corners
    out.add_face(
        {
            front_vertices[size],
            front_vertices[0],
            left_vertices[0],
            bottom_vertices[0]
        }
    );
    out.add_face(
        {
            left_vertices[((size - 3) * size)],
            left_vertices[((size - 2) * size)],
            back_vertices[0],
            bottom_vertices[size - 3]
        }
    );
    out.add_face(
        {
            back_vertices[((size - 3) * size)],
            back_vertices[((size - 2) * size)],
            right_vertices[((size - 3) * size)],
            bottom_vertices[((size - 2) * (size - 2)) - 1]
        }
    );
    out.add_face(
        {
            front_vertices[((size - 1) * size)],
            front_vertices[((size - 2) * size)],
            bottom_vertices[((size - 3) * (size - 2))],
            right_vertices[0]
        }
    );

    return out;
}

}
