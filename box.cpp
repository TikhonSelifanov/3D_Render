#include "box.h"

namespace Renderer {


Screen::Screen(int width, int height) : w(width), h(height) {
    r = Eigen::MatrixXi(w, h).setZero();
    g = Eigen::MatrixXi(w, h).setZero();
    b = Eigen::MatrixXi(w, h).setZero();
    z_buffer = Eigen::MatrixXd(w, h).setConstant(2);
}

std::pair<int, int> Screen::rasterize_point(double x, double y) const {
    int i = std::clamp(static_cast<int>((x + 1.0) * w / 2), 0, w - 1);
    int j = std::clamp(static_cast<int>((y + 1.0) * h / 2), 0, h - 1);
    return {i, j};
}

void Screen::put_pixel(int i, int j, Color color, double z) {
    if (z < z_buffer(i, j)) {
        z_buffer(i, j) = z;
        r(i, j) = color.r;   // красный
        g(i, j) = color.g;   // зеленый
        b(i, j) = color.b;   // синий
    }
}

void Screen::rasterize_and_put(const Point &p) {
    auto [i, j] = rasterize_point(p.p_.x(), p.p_.y());
    put_pixel(i, j, p.color_, p.p_.z());
}

void Screen::draw_line(int x1, int y1, double z1, int x2, int y2, double z2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (x1 != x2 || y1 != y2) {
        double z = z1 + (z2 - z1) * ((dx > dy) ? abs(x1 - x2) / static_cast<double>(dx) : abs(y1 - y2) / static_cast<double>(dy));
        put_pixel(x1, y1, color, z);

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Растеризация сектора и рисование линии на экране
void Screen::rasterize_and_put(const Sector &s) {
    auto [x1, y1] = rasterize_point(s.p1_.x(), s.p1_.y());
    auto [x2, y2] = rasterize_point(s.p2_.x(), s.p2_.y());
    draw_line(x1, y1, s.p1_.z(), x2, y2, s.p2_.z(), s.color_);
}

// Заполнение треугольника на экране с учетом глубины
void Screen::fill_triangle(int x1, int y1, double z1, int x2, int y2, double z2, int x3, int y3, double z3, Color color) {
    if (y1 > y2) std::swap(y1, y2), std::swap(x1, x2), std::swap(z1, z2);
    if (y1 > y3) std::swap(y1, y3), std::swap(x1, x3), std::swap(z1, z3);
    if (y2 > y3) std::swap(y2, y3), std::swap(x2, x3), std::swap(z2, z3);

    int total_height = y3 - y1;
    for (int i = 0; i < total_height; ++i) {
        bool second_half = i > y2 - y1 || y2 == y1;
        int segment_height = second_half ? y3 - y2 : y2 - y1;
        float alpha = static_cast<float>(i) / total_height;
        float beta = static_cast<float>(i - (second_half ? y2 - y1 : 0)) / segment_height;
        int ax = x1 + (x3 - x1) * alpha;
        int bx = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;
        double az = z1 + (z3 - z1) * alpha;
        double bz = second_half ? z2 + (z3 - z2) * beta : z1 + (z2 - z1) * beta;

        if (ax > bx) std::swap(ax, bx), std::swap(az, bz);
        for (int j = ax; j <= bx; ++j) {
            double phi = (bx == ax) ? 1.0 : (j - ax) / static_cast<double>(bx - ax);
            double z = az + phi * (bz - az);
            put_pixel(j, y1 + i, color, z);
        }
    }
}

// Растеризация треугольника и заполнение его на экране
void Screen::rasterize_and_put(const Triangle &t) {
    auto [x1, y1] = rasterize_point(t.p1_.x(), t.p1_.y());
    auto [x2, y2] = rasterize_point(t.p2_.x(), t.p2_.y());
    auto [x3, y3] = rasterize_point(t.p3_.x(), t.p3_.y());
    fill_triangle(x1, y1, t.p1_.z(), x2, y2, t.p2_.z(), x3, y3, t.p3_.z(), t.color_);
}

// Отображение всех элементов из ViewBox на экране
void ViewBox::map_to_pixels(Renderer::Screen &screen) {
    for (const auto &point : points_) {
        screen.rasterize_and_put(point);
    }
    for (const auto &sector : sectors_) {
        screen.rasterize_and_put(sector);
    }
    for (const auto &triangle : triangles_) {
        screen.rasterize_and_put(triangle);
    }
}
}
