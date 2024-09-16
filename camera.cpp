#include "camera.h"

namespace Renderer {

Frustum::Frustum(double left, double right, double bottom, double top, double near, double far)
    : left_(left), right_(right), bottom_(bottom), top_(top), near_(near), far_(far) {
    initialize_planes();
    assert(left_ != right_);
    assert(bottom_ != top_);
    assert(near_ != far_);
    compute_projection_matrix();
}

void Frustum::update_params(double left, double right, double bottom, double top, double near, double far) {
    left_ = left;
    right_ = right;
    bottom_ = bottom;
    top_ = top;
    near_ = near;
    far_ = far;
    initialize_planes();
    compute_projection_matrix();
}

void Frustum::compute_projection_matrix() {
    proj_matrix_ = Eigen::Matrix4d {
        {2 * near_ / (right_ - left_), 0, (right_ + left_) / (right_ - left_), 0},
        {0, 2 * near_ / (top_ - bottom_), (top_ + bottom_) / (top_ - bottom_), 0},
        {0, 0, -(far_ + near_) / (far_ - near_), -2 * far_ * near_ / (far_ - near_)},
        {0, 0, -1., 0}
    };
}

void Frustum::initialize_planes() {
    near_plane_ = Plane({left_, bottom_, near_}, {left_, top_, -near_}, {right_, bottom_, -near_});
    left_plane_ = Plane({0, 0, 0}, {left_, bottom_, -near_}, {left_, top_, -near_});
    right_plane_ = Plane({0, 0, 0}, {right_, top_, -near_}, {right_, bottom_, -near_});
    top_plane_ = Plane({0, 0, 0}, {left_, top_, -near_}, {right_, top_, -near_});
    bottom_plane_ = Plane({0, 0, 0}, {right_, bottom_, -near_}, {left_, bottom_, -near_});
    far_plane_ = Plane({left_, bottom_, -far_}, {right_, bottom_, -far_}, {left_, top_, -far_});
}

Eigen::Matrix4d Frustum::get_projection_matrix() const {
    return proj_matrix_;
}

std::vector<Plane> Frustum::get_frustum_planes() const {
    return {near_plane_, left_plane_, right_plane_, top_plane_, bottom_plane_, far_plane_};
}
}
