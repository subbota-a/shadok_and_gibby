#pragma once
#include <cstdint>
#include <Eigen/Core>

namespace domain {

using Scalar = int8_t;
using Position = Eigen::Matrix<Scalar, 2, 1>;
using Vector = Eigen::Matrix<Scalar, 2, 1>;

}
