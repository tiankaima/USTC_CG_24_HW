#pragma once
#include "particle_system.h"
#include "sph_base.h"
#include "utils.h"
#include <Eigen/Dense>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

class IISPH : public SPHBase {
public:
  IISPH() = default;
  IISPH(const MatrixXd &X, const Vector3d &box_min, const Vector3d &box_max);
  ~IISPH() = default;

  void step() override;
  void compute_pressure() override;

  double pressure_solve_iteration();
  void predict_advection();

  void reset() override;

  int &max_iter() { return max_iter_; }
  double &omega() { return omega_; }

protected:
  int max_iter_ = 3;
  double omega_ = 0.5;

  VectorXd predict_density_;
  VectorXd aii_;
  VectorXd Api_;
  VectorXd last_pressure_;
  std::vector<Eigen::Vector3d> dii_;
};
} // namespace USTC_CG::node_sph_fluid