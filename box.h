#pragma once
#include "objects.h"
namespace Renderer {
class Screen {
public:
    Eigen::MatrixXi r, g, b;
    Eigen::MatrixXd z_buffer;
    int w, h;
    Screen(int w, int h);

    void put_pixel(int i, int j, Color c, double z);
    std::pair<int, int> rasterize_point(double x, double y) const;

    void rasterize_and_put(const Point &p);
    void rasterize_and_put(const Sector &s);
    void rasterize_and_put(const Triangle &t);
    void draw_line(int x1, int y1, double z1, int x2, int y2, double z2, Color color);
    void fill_triangle(int x1, int y1, double z1, int x2, int y2, double z2, int x3, int y3, double z3, Color color);
};

struct ViewBox {
    std::vector<Triangle> triangles_;
    std::vector<Point> points_;
    std::vector<Sector> sectors_;

public:
    void map_to_pixels(Screen &screen);
};

}
