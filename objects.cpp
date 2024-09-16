#include "objects.h"
#include "geometry.h"
#include <Eigen/Dense>
#include <utility>

namespace Renderer {

// Конструктор треугольника, сразу нормализует все переданные вершины
Triangle::Triangle(Eigen::Vector4d p1, Eigen::Vector4d p2, Eigen::Vector4d p3, Color color)
    : p1_(normalize_vector(std::move(p1))),
    p2_(normalize_vector(std::move(p2))),
    p3_(normalize_vector(std::move(p3))),
    color_(color) {}

// Трансформация треугольника через матрицу с последующей нормализацией
Triangle Triangle::transform(const Eigen::Matrix4d& m) const {
    return Triangle(apply_transform_and_normalize(p1_, m),
                    apply_transform_and_normalize(p2_, m),
                    apply_transform_and_normalize(p3_, m),
                    color_);
}

// Перегрузка конструктора для работы с 3D-координатами
Triangle::Triangle(Eigen::Vector3d p1, Eigen::Vector3d p2, Eigen::Vector3d p3, Color color)
    : p1_(from_3d_to_4d(std::move(p1))),
    p2_(from_3d_to_4d(std::move(p2))),
    p3_(from_3d_to_4d(std::move(p3))),
    color_(color) {}

// Конструктор точки, с нормализацией вектора
Point::Point(Eigen::Vector4d p, Color color)
    : p_(normalize_vector(std::move(p))),
    color_(color) {}

// Трансформация точки
Point Point::transform(const Eigen::Matrix4d& m) const {
    return Point(apply_transform_and_normalize(p_, m), color_);
}

// Конструктор точки из 3D-координат
Point::Point(Eigen::Vector3d p, Color color)
    : p_(from_3d_to_4d(std::move(p))),
    color_(color) {}

// Конструктор отрезка (сектора), с нормализацией
Sector::Sector(Eigen::Vector4d p1, Eigen::Vector4d p2, Color color)
    : p1_(normalize_vector(std::move(p1))),
    p2_(normalize_vector(std::move(p2))),
    color_(color) {}

// Трансформация сектора
Sector Sector::transform(const Eigen::Matrix4d& m) const {
    return Sector(apply_transform_and_normalize(p1_, m),
                  apply_transform_and_normalize(p2_, m),
                  color_);
}

// Конструктор сектора из 3D-точек
Sector::Sector(Eigen::Vector3d p1, Eigen::Vector3d p2, Color color)
    : p1_(from_3d_to_4d(std::move(p1))),
    p2_(from_3d_to_4d(std::move(p2))),
    color_(color) {}

// Получение вектора трансляции объекта
Eigen::Vector3d Object::get_translation() const {
    return Eigen::Vector3d(transform_matrix_(0, 3),
                           transform_matrix_(1, 3),
                           transform_matrix_(2, 3));
}

// Конструктор объекта с инициализацией единичной матрицей
Object::Object() {
    transform_matrix_.setIdentity();
}

// Добавление точки к объекту
void Object::add_point(Point point) {
    points_.emplace_back(std::move(point));
}

// Добавление сектора к объекту
void Object::add_sector(Sector sector) {
    sectors_.emplace_back(std::move(sector));
}

// Добавление треугольника к объекту
void Object::add_triangle(Triangle triangle) {
    triangles_.emplace_back(std::move(triangle));
}

// Вращение объекта вокруг глобальной оси
void Object::rotate_global(const Eigen::Vector3d& axis, double angle) {
    transform_matrix_ = create_rotation_matrix(axis, angle) * transform_matrix_;
}

// Вращение объекта вокруг локальной оси
void Object::rotate_local(const Eigen::Vector3d& axis, double angle) {
    Eigen::Vector3d translate_vector = get_translation();
    reset_translation();
    transform_matrix_ = create_rotation_matrix(axis, angle) * transform_matrix_;
    restore_translation(translate_vector);
}

// Трансляция объекта
void Object::translate(const Eigen::Vector3d& translation) {
    transform_matrix_(0, 3) += translation[0];
    transform_matrix_(1, 3) += translation[1];
    transform_matrix_(2, 3) += translation[2];
}

// Получение матрицы трансформации объекта
Eigen::Matrix4d Object::get_transform() const {
    return transform_matrix_;
}

// Установка матрицы трансформации
void Object::set_transform(const Eigen::Matrix4d& matrix) {
    transform_matrix_ = std::move(matrix);
}

// Получение всех треугольников объекта с учётом трансформации
std::vector<Triangle> Object::get_triangles() const {
    std::vector<Triangle> transformed_triangles;
    for (const auto& triangle : triangles_) {
        transformed_triangles.push_back(triangle.transform(transform_matrix_));
    }
    return transformed_triangles;
}

// Получение всех точек объекта с учётом трансформации
std::vector<Point> Object::get_points() const {
    std::vector<Point> transformed_points;
    for (const auto& point : points_) {
        transformed_points.push_back(point.transform(transform_matrix_));
    }
    return transformed_points;
}

// Получение всех секторов объекта с учётом трансформации
std::vector<Sector> Object::get_sectors() const {
    std::vector<Sector> transformed_sectors;
    for (const auto& sector : sectors_) {
        transformed_sectors.push_back(sector.transform(transform_matrix_));
    }
    return transformed_sectors;
}

// Применение дополнительной трансформации к объекту
Object& Object::apply_transform(const Eigen::Matrix4d& m) {
    transform_matrix_ = m * transform_matrix_;
    return *this;
}

// Конструктор плоскости из нормали и d
Plane::Plane(Eigen::Vector4d normal_d)
    : nd_(std::move(normal_d)) {}

// Конструктор плоскости из 3D нормали и d
Plane::Plane(const Eigen::Vector3d& normal, double d) {
    Eigen::Vector3d n = normal.normalized();
    nd_ = Eigen::Vector4d{n[0], n[1], n[2], d};
}

// Конструктор плоскости через три точки
Plane::Plane(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3) {
    Eigen::Vector3d normal = (p2 - p1).cross(p3 - p1).normalized();
    nd_ = Eigen::Vector4d{normal[0], normal[1], normal[2], -p1.dot(normal)};
}

// Расстояние от плоскости до точки
double Plane::distance_to_point(const Eigen::Vector4d& point) const {
    return nd_.dot(normalize_vector(point));
}

// Конструктор линии (отрезка) через начало и направление
Line::Line(Eigen::Vector4d start, Eigen::Vector4d direction)
    : s_(std::move(start)), v_(std::move(direction)) {}

// Конструктор линии через сектор
Line::Line(const Sector& sector) {
    s_ = normalize_vector(sector.p1_);
    v_ = normalize_vector(sector.p1_) - normalize_vector(sector.p2_);
}

// Получение точки на линии через параметр t
Eigen::Vector4d Line::operator()(double t) const {
    return s_ + v_ * t;
}

// Пересечение линии с плоскостью
Eigen::Vector4d plane_line_intersection(const Line& line, const Plane& plane) {
    double dot_product = plane.nd_.dot(line.v_);
    if (dot_product == 0) {
        throw std::runtime_error("Line is parallel to plane");
    }
    return line((-plane.nd_.dot(line.s_)) / dot_product);
}

// Вспомогательная функция для сброса трансляции объекта
void Object::reset_translation() {
    transform_matrix_(0, 3) = 0;
    transform_matrix_(1, 3) = 0;
    transform_matrix_(2, 3) = 0;
}

// Восстановление трансляции после локального вращения
void Object::restore_translation(const Eigen::Vector3d& translation) {
    transform_matrix_(0, 3) = translation[0];
    transform_matrix_(1, 3) = translation[1];
    transform_matrix_(2, 3) = translation[2];
}
}
