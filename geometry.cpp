#include "geometry.h"

namespace Renderer {

Eigen::Vector4d normalize_vector(const Eigen::Vector4d& vec) {
    if (vec[3] != 0) {
        return Eigen::Vector4d{ vec[0] / vec[3], vec[1] / vec[3], vec[2] / vec[3], 1 };
    }
    throw std::invalid_argument("Attempting to normalize a vector with w = 0.");
}

Eigen::Vector4d apply_transform_and_normalize(const Eigen::Vector4d& vec, const Eigen::Matrix4d& matrix) {
    Eigen::Vector4d transformed_vec = matrix * vec;
    return normalize_vector(transformed_vec);
}

Eigen::Vector4d from_3d_to_4d(const Eigen::Vector3d& vec3) {
    return Eigen::Vector4d(vec3[0], vec3[1], vec3[2], 1);
}

Eigen::Vector3d from_4d_to_3d(const Eigen::Vector4d& vec4) {
    Eigen::Vector4d normalized_vec = normalize_vector(vec4);
    return Eigen::Vector3d(normalized_vec[0], normalized_vec[1], normalized_vec[2]);
}

// Генерация матрицы поворота вокруг произвольной оси на заданный угол
Eigen::Matrix4d create_rotation_matrix(const Eigen::Vector3d& axis, double angle) {
    double cos_theta = cos(angle);
    double sin_theta = sin(angle);
    double x = axis[0], y = axis[1], z = axis[2];

    return Eigen::Matrix4d {
        {cos_theta + (1 - cos_theta) * x * x, (1 - cos_theta) * x * y - sin_theta * z, (1 - cos_theta) * x * z + sin_theta * y, 0},
        {(1 - cos_theta) * x * y + sin_theta * z, cos_theta + (1 - cos_theta) * y * y, (1 - cos_theta) * y * z - sin_theta * x, 0},
        {(1 - cos_theta) * x * z - sin_theta * y, (1 - cos_theta) * y * z + sin_theta * x, cos_theta + (1 - cos_theta) * z * z, 0},
        {0, 0, 0, 1}
    };
}
}
