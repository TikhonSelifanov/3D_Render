#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>
#include "objects.h"

namespace Renderer {
class Frustum {
    public:
        Frustum(double left, double right, double bottom, double top, double near, double far);

        void update_params(double left, double right, double bottom, double top, double near, double far);

        void initialize_planes();

        void compute_projection_matrix();

        Eigen::Matrix4d get_projection_matrix() const;

        std::vector<Plane> get_frustum_planes() const;

    private:
        double left_, right_, bottom_, top_, near_, far_;
        Plane left_plane_, right_plane_, top_plane_, bottom_plane_, far_plane_, near_plane_;
        Eigen::Matrix4d proj_matrix_;
};
}
