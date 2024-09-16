#include "renderer.h"
#include <SFML/Graphics.hpp>

void draw(const Renderer::Screen &screen) {
    int w = screen.w;
    int h = screen.h;
    sf::RenderWindow window(sf::VideoMode(w, h), "Renderer");

    auto pixels = std::vector<sf::Uint8>(h * w * 4);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);

        sf::Texture texture;
        texture.create(w, h);

        sf::Sprite sprite(texture);

        for (int i = 0; i < w * h * 4; i += 4) {
            pixels[i] = screen.r(i / 4);
            pixels[i + 1] = screen.g(i / 4);
            pixels[i + 2] = screen.b(i / 4);
            pixels[i + 3] = 200;
        }

        texture.update(pixels.data());

        window.draw(sprite);

        window.display();
    }
}

void example_torus() {
    Renderer::World world;

    Renderer::Object torus;

    int slices = 32;
    int rings = 16;
    double major_radius = 2.0;
    double minor_radius = 0.5;

    for (int i = 0; i < slices; ++i) {
        for (int j = 0; j < rings; ++j) {
            double theta1 = i * 2 * M_PI / slices;
            double theta2 = (i + 1) * 2 * M_PI / slices;
            double phi1 = j * 2 * M_PI / rings;
            double phi2 = (j + 1) * 2 * M_PI / rings;

            Eigen::Vector3d p1 {
                (major_radius + minor_radius * cos(phi1)) * cos(theta1),
                minor_radius * sin(phi1),
                (major_radius + minor_radius * cos(phi1)) * sin(theta1)
            };
            Eigen::Vector3d p2 {
                (major_radius + minor_radius * cos(phi2)) * cos(theta1),
                minor_radius * sin(phi2),
                (major_radius + minor_radius * cos(phi2)) * sin(theta1)
            };
            Eigen::Vector3d p3 {
                (major_radius + minor_radius * cos(phi2)) * cos(theta2),
                minor_radius * sin(phi2),
                (major_radius + minor_radius * cos(phi2)) * sin(theta2)
            };
            Eigen::Vector3d p4 {
                (major_radius + minor_radius * cos(phi1)) * cos(theta2),
                minor_radius * sin(phi1),
                (major_radius + minor_radius * cos(phi1)) * sin(theta2)
            };

            torus.add_triangle({p1, p2, p3, {255, 0, 0}});
            torus.add_triangle({p1, p3, p4, {0, 255, 0}});
        }
    }

    torus.rotate_global({0, 1, 0}, 0.5);
    torus.translate({0, 0, -6});

    world.add_object(torus);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}

void example_prism() {
    Renderer::World world;

    double radius = 1.0;
    double height = 2.0;
    int sides = 6;

    Renderer::Object prism;

    std::vector<Eigen::Vector3d> top_vertices, bottom_vertices;
    for (int i = 0; i < sides; ++i) {
        double angle = 2 * M_PI * i / sides;
        top_vertices.push_back({radius * cos(angle), height / 2, radius * sin(angle)});
        bottom_vertices.push_back({radius * cos(angle), -height / 2, radius * sin(angle)});
    }

    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        prism.add_triangle({top_vertices[i], bottom_vertices[i], top_vertices[next], {255, 0, 0}});
        prism.add_triangle({bottom_vertices[i], bottom_vertices[next], top_vertices[next], {0, 255, 0}});
    }

    for (int i = 1; i < sides - 1; ++i) {
        prism.add_triangle({top_vertices[0], top_vertices[i], top_vertices[i + 1], {0, 0, 255}});
        prism.add_triangle({bottom_vertices[0], bottom_vertices[i + 1], bottom_vertices[i], {255, 255, 0}});
    }

    prism.rotate_global({1, 0, 0}, 0.5);
    prism.translate({0, 0, -5});

    world.add_object(prism);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}


void example_pyramid() {
    Renderer::World world;

    Eigen::Vector3d base1 {1, 0, 0}, base2 {0, 1, 0}, base3 {-1, 0, 0}, base4 {0, -1, 0};
    Eigen::Vector3d apex {0, 0, 2};

    Renderer::Object pyramid;


    pyramid.add_triangle({base1, base2, base3, {255, 0, 0}});
    pyramid.add_triangle({base1, base3, base4, {0, 255, 0}});

    pyramid.add_triangle({base1, base2, apex, {0, 0, 255}});
    pyramid.add_triangle({base2, base3, apex, {255, 255, 0}});
    pyramid.add_triangle({base3, base4, apex, {255, 0, 255}});
    pyramid.add_triangle({base4, base1, apex, {0, 255, 255}});

    pyramid.rotate_global({0, 1, 0}, 0.5);
    pyramid.translate({0, 0, -5});

    world.add_object(pyramid);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}

void example_sphere() {
    Renderer::World world;

    Renderer::Object sphere;

    int slices = 16;
    int stacks = 16;
    double radius = 1.0;

    for (int i = 0; i < slices; ++i) {
        for (int j = 0; j < stacks; ++j) {
            double theta1 = i * 2 * M_PI / slices;
            double theta2 = (i + 1) * 2 * M_PI / slices;
            double phi1 = j * M_PI / stacks - M_PI / 2;
            double phi2 = (j + 1) * M_PI / stacks - M_PI / 2;

            Eigen::Vector3d p1 {
                radius * cos(phi1) * cos(theta1),
                radius * sin(phi1),
                radius * cos(phi1) * sin(theta1)
            };
            Eigen::Vector3d p2 {
                radius * cos(phi2) * cos(theta1),
                radius * sin(phi2),
                radius * cos(phi2) * sin(theta1)
            };
            Eigen::Vector3d p3 {
                radius * cos(phi2) * cos(theta2),
                radius * sin(phi2),
                radius * cos(phi2) * sin(theta2)
            };
            Eigen::Vector3d p4 {
                radius * cos(phi1) * cos(theta2),
                radius * sin(phi1),
                radius * cos(phi1) * sin(theta2)
            };

            sphere.add_triangle({p1, p2, p3, {255, 0, 0}});
            sphere.add_triangle({p1, p3, p4, {0, 255, 0}});
        }
    }

    sphere.rotate_global({0, 1, 0}, 1.0);
    sphere.translate({0, 0, -5});

    world.add_object(sphere);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}


void example_cube() {
    Renderer::World world;

    Eigen::Vector3d v1 {1, 1, 1}, v2 {1, 1, -1}, v3 {1, -1, 1}, v4 {1, -1, -1};
    Eigen::Vector3d v5 {-1, 1, 1}, v6 {-1, 1, -1}, v7 {-1, -1, 1}, v8 {-1, -1, -1};

    Renderer::Object cube;

    cube.add_triangle({v1, v2, v3, {255, 0, 0}});
    cube.add_triangle({v3, v2, v4, {255, 0, 0}});

    cube.add_triangle({v5, v6, v7, {0, 255, 0}});
    cube.add_triangle({v7, v6, v8, {0, 255, 0}});

    cube.add_triangle({v1, v3, v5, {0, 0, 255}});
    cube.add_triangle({v5, v3, v7, {0, 0, 255}});

    cube.add_triangle({v2, v4, v6, {255, 255, 0}});
    cube.add_triangle({v6, v4, v8, {255, 255, 0}});

    cube.add_triangle({v1, v2, v5, {255, 0, 255}});
    cube.add_triangle({v5, v2, v6, {255, 0, 255}});

    cube.add_triangle({v3, v4, v7, {0, 255, 255}});
    cube.add_triangle({v7, v4, v8, {0, 255, 255}});

    cube.rotate_global({0.5, 1, 0}, 0.5);
    cube.translate({0, 0, -5});

    world.add_object(cube);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}

void example_parallelepiped() {
    Renderer::World world;

    Renderer::Object parallelepiped;

    Eigen::Vector3d v1 {1, 2, 1}, v2 {1, 2, -1}, v3 {1, -2, 1}, v4 {1, -2, -1};
    Eigen::Vector3d v5 {-1, 2, 1}, v6 {-1, 2, -1}, v7 {-1, -2, 1}, v8 {-1, -2, -1};

    parallelepiped.add_triangle({v1, v2, v3, {255, 0, 0}});
    parallelepiped.add_triangle({v3, v2, v4, {255, 0, 0}});

    parallelepiped.add_triangle({v5, v6, v7, {0, 255, 0}});
    parallelepiped.add_triangle({v7, v6, v8, {0, 255, 0}});

    parallelepiped.add_triangle({v1, v2, v5, {0, 0, 255}});
    parallelepiped.add_triangle({v5, v2, v6, {0, 0, 255}});

    parallelepiped.add_triangle({v3, v4, v7, {255, 255, 0}});
    parallelepiped.add_triangle({v7, v4, v8, {255, 255, 0}});

    parallelepiped.add_triangle({v1, v3, v5, {255, 0, 255}});
    parallelepiped.add_triangle({v5, v3, v7, {255, 0, 255}});

    parallelepiped.add_triangle({v2, v4, v6, {0, 255, 255}});
    parallelepiped.add_triangle({v6, v4, v8, {0, 255, 255}});

    parallelepiped.rotate_global({0, 1, 0}, 0.5);
    parallelepiped.translate({0, 0, -6});

    world.add_object(parallelepiped);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}

void example_triangular_prism() {
    Renderer::World world;

    Renderer::Object prism;

    Eigen::Vector3d v1 {1, 1, 1}, v2 {-1, 1, 1}, v3 {0, -1, 1};
    Eigen::Vector3d v4 {1, 1, -1}, v5 {-1, 1, -1}, v6 {0, -1, -1};

    prism.add_triangle({v1, v2, v4, {255, 0, 0}});
    prism.add_triangle({v2, v5, v4, {255, 0, 0}});

    prism.add_triangle({v2, v3, v5, {0, 255, 0}});
    prism.add_triangle({v3, v6, v5, {0, 255, 0}});

    prism.add_triangle({v3, v1, v6, {0, 0, 255}});
    prism.add_triangle({v1, v4, v6, {0, 0, 255}});

    prism.add_triangle({v1, v2, v3, {255, 255, 0}});
    prism.add_triangle({v4, v5, v6, {255, 0, 255}});

    prism.rotate_global({0, 1, 0}, 0.8);
    prism.translate({0, 0, -5});

    world.add_object(prism);

    auto view_box = world.make_view_box();

    int w = 1024;
    int h = 1024;
    Renderer::Screen screen(w, h);
    view_box.map_to_pixels(screen);

    draw(screen);
}


int main() {
    example_torus();
    example_prism();
    example_pyramid();
    example_sphere();
    example_cube();
    example_parallelepiped();
    example_triangular_prism();
}
