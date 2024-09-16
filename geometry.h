#pragma once
#include <Eigen/Core>

namespace Renderer {
Eigen::Vector4d normalize_vector(const Eigen::Vector4d& vec);
Eigen::Vector4d from_3d_to_4d(const Eigen::Vector3d& vec3);
Eigen::Vector3d from_4d_to_3d(const Eigen::Vector4d& vec4);
Eigen::Vector4d apply_transform_and_normalize(const Eigen::Vector4d& vec, const Eigen::Matrix4d& matrix);
Eigen::Matrix4d create_rotation_matrix(const Eigen::Vector3d& axis, double angle);
}
