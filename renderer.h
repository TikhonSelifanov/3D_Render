#pragma once
#include "camera.h"
#include "objects.h"
#include "box.h"
#include <optional>

namespace Renderer {
class World {
    Frustum frustum_;
    std::vector<Object> objects_;
    std::vector<Triangle> triangles_;
    std::vector<Point> points_;
    std::vector<Sector> sectors_;
public:
    World();

    void add_triangle(Triangle triangle);

    void add_point(Point point);

    void add_sector(Sector sector);

    void add_object(Object object);

    void change_frustum_params(double l, double r, double b, double t, double n, double f);

    void process_points(ViewBox& view_box) const;

    void collect_scene_elements();

    void process_sectors(ViewBox& view_box) const;

    void process_triangles(ViewBox& view_box) const;

    void process_clipping_against_plane(const Triangle& triangle, const Plane& plane, std::vector<Triangle>& result) const;

    void handle_partial_clipping(const Triangle& triangle, double p1_dist, double p2_dist,
                                 double p3_dist, const Plane& plane, std::vector<Triangle>& result) const;

    Sector update_sector(const Sector& s, const Eigen::Vector4d& new_point, double p1_dist) const;

    ViewBox make_view_box();


    template <typename T>
    T project(T obj) const {
        return (obj.transform(frustum_.get_projection_matrix()));
    }

    std::optional<Point> clip_point(const Point& p) const;

    std::optional<Sector> clip_sector(Sector s) const;

    std::vector<Triangle> clip_triangle(const Triangle &t) const;
};
}
