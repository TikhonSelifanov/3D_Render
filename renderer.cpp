#include "renderer.h"
#include "objects.h"
#include "box.h"

namespace Renderer {
World::World(): frustum_(-1, 1, -1, 1, 1, 10) {}

void World::add_triangle(Triangle triangle) {
    triangles_.emplace_back(std::move(triangle));
}

void World::add_point(Point point) {
    points_.emplace_back(std::move(point));
}

void World::add_sector(Sector sector) {
    sectors_.emplace_back(std::move(sector));
}

void World::add_object(Object object) {
    objects_.emplace_back(std::move(object));
}

// Создание объекта ViewBox, который содержит все объекты сцены после обработки
ViewBox World::make_view_box() {
    ViewBox view_box;
    collect_scene_elements();

    process_points(view_box);
    process_sectors(view_box);
    process_triangles(view_box);

    return view_box;
}

// Проверка, находятся ли все точки внутри
template<typename... Args>
bool all_points_inside(Args... dists) {
    return ((dists >= 0) && ...);
}

// Копирование элементов из одного вектора в другой
template <typename T>
void append_elements(const std::vector<T>& src, std::vector<T>& dst) {
    dst.insert(dst.end(), src.begin(), src.end());
}

// Сбор всех элементов сцены из объектов мира
void World::collect_scene_elements() {
    for (const auto& object : objects_) {
        append_elements(object.get_points(), points_);
        append_elements(object.get_sectors(), sectors_);
        append_elements(object.get_triangles(), triangles_);
    }
}

// Обработка всех точек и добавление их в ViewBox
void World::process_points(ViewBox& view_box) const {
    for (const auto& point : points_) {
        if (auto clipped = clip_point(point)) {
            view_box.points_.emplace_back(project(*clipped));
        }
    }
}

// Обработка всех секторов и добавление их в ViewBox
void World::process_sectors(ViewBox& view_box) const {
    for (const auto& sector : sectors_) {
        if (auto clipped = clip_sector(sector)) {
            view_box.sectors_.emplace_back(project(*clipped));
        }
    }
}

// Обработка всех треугольников и добавление их в ViewBox
void World::process_triangles(ViewBox& view_box) const {
    for (const auto& triangle : triangles_) {
        auto clipped_triangles = clip_triangle(triangle);
        for (const auto& clipped_triangle : clipped_triangles) {
            view_box.triangles_.emplace_back(project(clipped_triangle));
        }
    }
}

// Обрезка точки
std::optional<Point> World::clip_point(const Point& p) const {
    for (const auto& plane : frustum_.get_frustum_planes()) {
        if (plane.distance_to_point(p.p_) < 0) {
            return {}; // Точка находится вне
        }
    }
    return p; // Точка находится внутри
}

// Обрезка сектора
std::optional<Sector> World::clip_sector(Sector s) const {
    for (const auto& plane : frustum_.get_frustum_planes()) {
        auto p1_dist = plane.distance_to_point(s.p1_);
        auto p2_dist = plane.distance_to_point(s.p2_);

        if (all_points_inside(p1_dist, p2_dist)) {
            return {}; // Сектор находится вне
        }
        if (!all_points_inside(p1_dist, p2_dist)) {
            continue; // Сектор частично выходит за границы
        }

        Eigen::Vector4d new_point = plane_line_intersection(Line(s.p1_, s.p2_), plane);
        s = update_sector(s, new_point, p1_dist); // Обновление сектора с учетом обрезки
    }
    return s;
}

// Обновление сектора после обрезки
Sector World::update_sector(const Sector& s, const Eigen::Vector4d& new_point, double p1_dist) const {
    if (p1_dist < 0) {
        return Sector(new_point, s.p2_, s.color_); // Сектор обрезан с одной стороны
    } else {
        return Sector(s.p1_, new_point, s.color_); // Сектор обрезан с другой стороны
    }
}

// Обрезка треугольника
std::vector<Triangle> World::clip_triangle(const Triangle& t) const {
    std::vector<Triangle> triangles_to_clip{t};
    for (const auto& plane : frustum_.get_frustum_planes()) {
        std::vector<Triangle> clipped_triangles;
        for (const auto& triangle : triangles_to_clip) {
            process_clipping_against_plane(triangle, plane, clipped_triangles);
        }
        triangles_to_clip = std::move(clipped_triangles); // Обновляем треугольники для следующей итерации
    }
    return triangles_to_clip;
}

// Обработка обрезки треугольника относительно плоскости
void World::process_clipping_against_plane(const Triangle& triangle, const Plane& plane, std::vector<Triangle>& result) const {
    auto p1_dist = plane.distance_to_point(triangle.p1_);
    auto p2_dist = plane.distance_to_point(triangle.p2_);
    auto p3_dist = plane.distance_to_point(triangle.p3_);

    if (all_points_inside(p1_dist, p2_dist, p3_dist)) {
        result.push_back(triangle); // Все внутри
    } else if (!all_points_inside(p1_dist, p2_dist, p3_dist)) {
        return; // полностью вне
    } else {
        handle_partial_clipping(triangle, p1_dist, p2_dist, p3_dist, plane, result); // Частичная обрезка
    }
}

// Обработка частичной обрезки треугольника относительно плоскости
void World::handle_partial_clipping(const Triangle& triangle, double p1_dist, double p2_dist, double p3_dist, const Plane& plane, std::vector<Triangle>& result) const {
    Eigen::Vector4d inside1, inside2, outside;

    if (p1_dist < 0) {
        outside = triangle.p1_;
        inside1 = triangle.p2_;
        inside2 = triangle.p3_;
    } else if (p2_dist < 0) {
        outside = triangle.p2_;
        inside1 = triangle.p3_;
        inside2 = triangle.p1_;
    } else {
        outside = triangle.p3_;
        inside1 = triangle.p1_;
        inside2 = triangle.p2_;
    }

    auto intersection1 = plane_line_intersection(Line(inside1, outside), plane);
    auto intersection2 = plane_line_intersection(Line(inside2, outside), plane);

    result.emplace_back(Triangle(inside1, intersection1, inside2, triangle.color_));
    result.emplace_back(Triangle(inside2, intersection1, intersection2, triangle.color_));
}

void World::change_frustum_params(double left, double right, double bottom, double top, double near, double far) {
    frustum_.update_params(left, right, bottom, top, near, far);
}
}
