#include "wcsph.h"
#include <iostream>
using namespace Eigen;

namespace USTC_CG::node_sph_fluid {

WCSPH::WCSPH(const MatrixXd &X, const Vector3d &box_min,
             const Vector3d &box_max)
    : SPHBase(X, box_min, box_max) {}

void WCSPH::compute_density() {
#pragma omp parallel for
  {
    for (auto &p : ps_.particles()) {
      p->density_ = 0.0;
      double w_zero = W_zero(ps_.h());
      p->density_ += ps_.mass() * w_zero;
      for (auto &q : p->neighbors()) {
        double w_ij = W(p->x() - q->x(), ps_.h());
        p->density_ += ps_.mass() * w_ij;
      }
      double rho_0 = ps_.density0();
      p->pressure_ = stiffness_ * (pow(p->density_ / rho_0, exponent_) - 1.0);
    }
  }
}

void WCSPH::step() {
  TIC(step)

  ps_.assign_particles_to_cells();
  ps_.searchNeighbors();

  compute_density();
  compute_non_pressure_acceleration();
  compute_pressure_gradient_acceleration();

  for (auto &p : ps_.particles()) {
    p->acceleration_ += p->acceleration_pressure_;
  }

  advect();

  TOC(step)
}
} // namespace USTC_CG::node_sph_fluid