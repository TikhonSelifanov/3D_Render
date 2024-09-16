#pragma once
#include <Eigen/Core>
#include <vector>

namespace Renderer {


struct Color {
    int r, g, b;
};


class Point {
public:
    Eigen::Vector4d p_;
    Color color_;

    Point(Eigen::Vector4d p, Color color);
    Point(Eigen::Vector3d p, Color color);

    Point transform(const Eigen::Matrix4d &m) const;
};


class Sector {
public:
    Eigen::Vector4d p1_, p2_;
    Color color_;

    Sector(Eigen::Vector4d p1, Eigen::Vector4d p2, Color color);
    Sector(Eigen::Vector3d p1, Eigen::Vector3d p2, Color color);

    Sector transform(const Eigen::Matrix4d &m) const;
};

class Triangle {
public:
    Eigen::Vector4d p1_, p2_, p3_;
    Color color_;

    Triangle(Eigen::Vector4d p1, Eigen::Vector4d p2, Eigen::Vector4d p3, Color color);
    Triangle(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3, Color color);

    Triangle transform(const Eigen::Matrix4d &m) const;
};


class Object {
    Eigen::Matrix4d transform_matrix_;
    std::vector<Point> points_;
    std::vector<Sector> sectors_;
    std::vector<Triangle> triangles_;

    Eigen::Vector3d get_translation() const;

public:
    Object();

    void reset_translation();
    void restore_translation(const Eigen::Vector3d& translation);


    void add_point(Point point);
    void add_sector(Sector sector);
    void add_triangle(Triangle triangle);

    void rotate_local(const Eigen::Vector3d &axis, double theta);
    void rotate_global(const Eigen::Vector3d &axis, double theta);
    void translate(const Eigen::Vector3d &t);

    Eigen::Matrix4d get_transform() const;
    void set_transform(const Eigen::Matrix4d& matrix);

    Object& apply_transform(const Eigen::Matrix4d& m);

    std::vector<Triangle> get_triangles() const;
    std::vector<Point> get_points() const;
    std::vector<Sector> get_sectors() const;
};

class Line;

class Plane {
    Eigen::Vector4d nd_;

public:
    Plane() = default;
    Plane(Eigen::Vector4d nd);
    Plane(const Eigen::Vector3d& normal, double d);
    Plane(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p3);

    double distance_to_point(const Eigen::Vector4d& point) const;
    friend Eigen::Vector4d plane_line_intersection(const Line& line, const Plane& plane);
};


class Line {
    Eigen::Vector4d s_;
    Eigen::Vector4d v_;

public:
    Line(Eigen::Vector4d s, Eigen::Vector4d v);
    Line(const Sector& sector);
    Eigen::Vector4d operator()(double t) const;
    friend Eigen::Vector4d plane_line_intersection(const Line& line, const Plane& plane);
};

Eigen::Vector4d plane_line_intersection(const Line& line, const Plane& plane);

}

