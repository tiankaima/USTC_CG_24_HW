#include "sph_base.h"
#include <cmath>
#define M_PI 3.14159265358979323846
#include "colormap_jet.h"
#include <iostream>
#include <omp.h>

namespace USTC_CG::node_sph_fluid {
using namespace Eigen;
using Real = double;

SPHBase::SPHBase(const Eigen::MatrixXd &X, const Vector3d &box_min,
                 const Vector3d &box_max)
    : init_X_(X), X_(X), vel_(MatrixXd::Zero(X.rows(), X.cols())),
      box_max_(box_max), box_min_(box_min), ps_(X, box_min, box_max) {}

double SPHBase::W(const Eigen::Vector3d &r, double h) {
  double h3 = h * h * h;
  double m_k = 8.0 / (M_PI * h3);
  double m_l = 48.0 / (M_PI * h3);
  const double q = r.norm() / h;
  double result = 0.;

  if (q <= 1.0) {
    if (q <= 0.5) {
      const Real q2 = q * q;
      const Real q3 = q2 * q;
      result = m_k * (6.0 * q3 - 6.0 * q2 + 1.0);
    } else {
      result = m_k * (2.0 * pow(1.0 - q, 3.0));
    }
  }
  return result;
}

double SPHBase::W_zero(double h) {
  double h3 = h * h * h;
  double m_k = 8.0 / (M_PI * h3);
  return m_k;
}

Vector3d SPHBase::grad_W(const Vector3d &r, double h) {
  double h3 = h * h * h;
  double m_k = 8.0 / (M_PI * h3);
  double m_l = 48.0 / (M_PI * h3);

  const double rl = r.norm();
  const double q = rl / h;
  Vector3d result = Vector3d::Zero();

  if (q <= 1.0 && rl > 1e-9) {
    Vector3d grad_q = r / rl;
    if (q <= 0.5) {
      result = m_l * q * (3.0 * q - 2.0) * grad_q;
    } else {
      const Real factor = 1.0 - q;
      result = -m_l * factor * factor * grad_q;
    }
  }
  return result;
}

void SPHBase::compute_density() {

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
    }
  }
}

void SPHBase::compute_pressure() {}

void SPHBase::compute_non_pressure_acceleration() {

#pragma omp parallel for
  {
    for (auto &p : ps_.particles()) {
      p->acceleration_ = Vector3d::Zero();

      p->acceleration_ += gravity_;
      for (auto &q : p->neighbors()) {

        p->acceleration_ += compute_viscosity_acceleration(p, q);
      }
    }
  }
}

Vector3d
SPHBase::compute_viscosity_acceleration(const std::shared_ptr<Particle> &p,
                                        const std::shared_ptr<Particle> &q) {
  auto v_ij = p->vel() - q->vel();
  auto x_ij = p->x() - q->x();
  Vector3d grad = grad_W(p->x() - q->x(), ps_.h());

  Vector3d laplace_v =
      2 * (3 + 2) * ps_.mass() / p->density_ *
      (v_ij.dot(x_ij) / (x_ij.squaredNorm() + 0.01 * ps_.h() * ps_.h())) * grad;

  return this->viscosity_ * laplace_v;
}

void SPHBase::compute_pressure_gradient_acceleration() {
#pragma omp parallel for
  {
    for (auto &p : ps_.particles()) {

      p->acceleration_pressure_ = Vector3d::Zero();
      Vector3d delta_p = Vector3d::Zero();
      if (p->density_ > 1e-10) {
        for (auto &q : p->neighbors()) {
          if (q->density_ > 1e-10) {
            delta_p += ps_.mass() *
                       (p->pressure_ / (p->density_ * p->density_) +
                        q->pressure_ / (q->density_ * q->density_)) *
                       grad_W(p->x() - q->x(), ps_.h());
          }
        }
        p->acceleration_pressure_ = -delta_p;
      }
    }
  }
}

void SPHBase::step() {}

void SPHBase::advect() {
#pragma omp parallel for
  {
    for (auto &p : ps_.particles()) {

      p->vel_ = p->vel_ + dt_ * p->acceleration_;
      p->X_ = p->X_ + dt_ * p->vel_;
      check_collision(p);

      vel_.row(p->idx()) = p->vel().transpose();
      X_.row(p->idx()) = p->x().transpose();
    }
  }
}

void SPHBase::check_collision(const std::shared_ptr<Particle> &p) {

  double restitution = 0.2;

  Vector3d eps_ = 0.0001 * (box_max_ - box_min_);

  for (int i = 0; i < 3; i++) {
    if (p->x()[i] < box_min_[i]) {
      p->x()[i] = box_min_[i] + eps_[i];
      p->vel()[i] = -restitution * p->vel()[i];
    }
    if (p->x()[i] > box_max_[i]) {
      p->x()[i] = box_max_[i] - eps_[i];
      p->vel()[i] = -restitution * p->vel()[i];
    }
  }
}

MatrixXd SPHBase::get_vel_color_jet() {
  MatrixXd vel_color = MatrixXd::Zero(vel_.rows(), 3);
  double max_vel_norm = vel_.rowwise().norm().maxCoeff();
  double min_vel_norm = vel_.rowwise().norm().minCoeff();

  auto c = colormap_jet;

  for (int i = 0; i < vel_.rows(); i++) {
    double vel_norm = vel_.row(i).norm();
    int idx = 0;
    if (fabs(max_vel_norm - min_vel_norm) > 1e-6) {
      idx = static_cast<int>(floor((vel_norm - min_vel_norm) /
                                   (max_vel_norm - min_vel_norm) * 255));
    }
    vel_color.row(i) << c[idx][0], c[idx][1], c[idx][2];
  }
  return vel_color;
}

void SPHBase::reset() {
  X_ = init_X_;
  vel_ = MatrixXd::Zero(X_.rows(), X_.cols());

  for (auto &p : ps_.particles()) {
    p->vel() = Vector3d::Zero();
    p->x() = init_X_.row(p->idx()).transpose();
  }
}

} // namespace USTC_CG::node_sph_fluid