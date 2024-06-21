#include "iisph.h"
#include <iostream>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

IISPH::IISPH(const MatrixXd &X, const Vector3d &box_min,
             const Vector3d &box_max)
    : SPHBase(X, box_min, box_max) {
  predict_density_ = VectorXd::Zero(ps_.particles().size());
  aii_ = VectorXd::Zero(ps_.particles().size());
  Api_ = VectorXd::Zero(ps_.particles().size());
  last_pressure_ = VectorXd::Zero(ps_.particles().size());
  dii_.resize(ps_.particles().size());
}

void IISPH::step() {

  TIC(step)
  ps_.assign_particles_to_cells();
  ps_.searchNeighbors();

  predict_advection();
  compute_pressure();

#pragma omp parallel for
  {
    for (auto &p : ps_.particles()) {
      p->acceleration_ += p->acceleration_pressure_;
      p->vel_ = p->vel_ + dt_ * p->acceleration_pressure_;

      p->X_ = p->X_ + dt_ * p->vel_;
      check_collision(p);
      vel_.row(p->idx()) = p->vel().transpose();
      X_.row(p->idx()) = p->x().transpose();
      last_pressure_[p->idx_] = p->pressure_;
    }
  }

  TOC(step)
}

void IISPH::compute_pressure() {
  double threshold = 0.001;
  for (unsigned iter = 0; iter < max_iter_; iter++) {
    double avg_density_error = pressure_solve_iteration();
    if (avg_density_error < threshold)
      break;
  }
}

void IISPH::predict_advection() {
#pragma omp parallel for
  {

    for (auto &p : ps_.particles()) {
      p->density_ = 0.0;
      double w_zero = W_zero(ps_.h());
      p->density_ += ps_.mass() * w_zero;
      dii_[p->idx_] = Vector3d::Zero();
      for (auto &q : p->neighbors()) {
        double w_ij = W(p->x() - q->x(), ps_.h());
        p->density_ += ps_.mass() * w_ij;

        Vector3d grad = grad_W(p->x() - q->x(), ps_.h());
        dii_[p->idx_] += ps_.mass() * grad;
      }
      dii_[p->idx_] = dii_[p->idx_] / pow(p->density_, 2);
    }

    compute_non_pressure_acceleration();
    for (auto &p : ps_.particles()) {
      p->vel_ += dt_ * p->acceleration_;
    }

    for (auto &p : ps_.particles()) {
      aii_(p->idx_) = 0.0;
      for (auto &q : p->neighbors()) {
        Vector3d grad = grad_W(p->x() - q->x(), ps_.h());
        Vector3d d_ji = ps_.mass() / pow(p->density_, 2) * (-grad);
        aii_(p->idx_) += -ps_.mass() * (dii_[p->idx_] - d_ji).dot(grad);

        p->density_ += -dt_ * ps_.mass() * (q->vel_ - p->vel_).dot(grad);
      }
      p->pressure_ = 0.5 * last_pressure_(p->idx_);
    }
  }
}

double IISPH::pressure_solve_iteration() {
#pragma omp parallel for
  {
    double rho_0 = ps_.density0();
    compute_pressure_gradient_acceleration();
    for (auto &p : ps_.particles()) {
      double b_i = (rho_0 - p->density_) / pow(dt_, 2);
      Api_[p->idx_] = 0.0;
      for (auto &q : p->neighbors()) {
        Vector3d grad = grad_W(p->x() - q->x(), ps_.h());
        Api_[p->idx_] +=
            ps_.mass() *
            (p->acceleration_pressure_ - q->acceleration_pressure_).dot(grad);
      }
      if (aii_[p->idx_] != 0.0)
        p->pressure_ += omega_ / aii_[p->idx_] * (b_i - Api_[p->idx_]);
      else
        p->pressure_ = 0.0;
      p->pressure() = std::clamp(p->pressure(), 0.0, 8e4);
    }

    double average_density_error = 0.0;
    for (auto &p : ps_.particles()) {
      average_density_error +=
          p->density_ - rho_0 + pow(dt_, 2) * Api_[p->idx_];
    }
    average_density_error = average_density_error / ps_.particles().size();
    average_density_error = fabs(average_density_error);
    return average_density_error;
  }
}

void IISPH::reset() {
  SPHBase::reset();

  predict_density_ = VectorXd::Zero(ps_.particles().size());
  aii_ = VectorXd::Zero(ps_.particles().size());
  Api_ = VectorXd::Zero(ps_.particles().size());
  last_pressure_ = VectorXd::Zero(ps_.particles().size());
}
} // namespace USTC_CG::node_sph_fluid