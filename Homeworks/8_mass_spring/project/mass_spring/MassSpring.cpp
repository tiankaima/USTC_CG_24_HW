#include "MassSpring.h"

#include <iostream>

namespace USTC_CG::node_mass_spring {
MassSpring::MassSpring(const Eigen::MatrixXd& X, const EdgeSet& E)
{
    this->X = this->init_X = X;
    this->vel = Eigen::MatrixXd::Zero(X.rows(), X.cols());
    this->E = E;

    std::cout << "number of edges: " << E.size() << std::endl;
    std::cout << "init mass spring" << std::endl;

    // Compute the rest pose edge length
    for (const auto& e : E) {
        Eigen::Vector3d x0 = X.row(e.first);
        Eigen::Vector3d x1 = X.row(e.second);
        this->E_rest_length.push_back((x0 - x1).norm());
    }

    // Initialize the mask for Dirichlet boundary condition
    dirichlet_bc_mask.resize(X.rows(), false);

    // (HW_TODO) Fix two vertices, feel free to modify this
    unsigned n_fix = sqrt(X.rows());  // Here we assume the cloth is square
    dirichlet_bc_mask[0] = true;
    dirichlet_bc_mask[n_fix - 1] = true;
}

void MassSpring::step()
{
    Eigen::Vector3d acceleration_ext = gravity + wind_ext_acc;

    unsigned n_vertices = X.rows();

    // The reason to not use 1.0 as mass per vertex: the cloth gets heavier as we increase the
    // resolution
    double mass_per_vertex = mass / n_vertices;

    //----------------------------------------------------
    // (HW Optional) Bonus part: Sphere collision
    Eigen::MatrixXd acceleration_collision =
        getSphereCollisionForce(sphere_center.cast<double>(), sphere_radius);
    //----------------------------------------------------

    if (time_integrator == IMPLICIT_EULER) {
        // Implicit Euler
        TIC(step)

        auto H_elastic = computeHessianSparse(stiffness);  // size = [nx3, nx3]
        Eigen::SparseMatrix<double> H =
            H_elastic +
            mass_per_vertex * Eigen::MatrixXd::Identity(n_vertices * 3, n_vertices * 3) / h / h;


        Eigen::MatrixXd fext = Eigen::MatrixXd(n_vertices, 3);
        fext.rowwise() = acceleration_ext.transpose();
        Eigen::MatrixXd Y = X + h * vel + h * h * fext;
        Eigen::MatrixXd grad_g =  mass_per_vertex * (X - Y) / h / h - computeGrad(stiffness);

        // fix the fixed points
       for (unsigned i = 0; i < n_vertices; i++) {
           if (dirichlet_bc_mask[i]) {
               for (int j = 0; j < 3; j++) {
                   grad_g(i, j) = X(i, j);
               }
           }
       }

        Eigen::MatrixXd grad_g_ = flatten(grad_g);

        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
        solver.compute(H);
        if (solver.info() != Eigen::Success) {
            std::cerr << "Decomposition failed!" << std::endl;
            return;
        }

        Eigen::MatrixXd X_new = solver.solve(grad_g_);
        if (solver.info() != Eigen::Success) {
            std::cerr << "Solving failed!" << std::endl;
            return;
        }

        Eigen::MatrixXd X_new_ = unflatten(X_new);

        X = X_new_;
        vel = (X - init_X) / h;

        TOC(step)
    }
    else if (time_integrator == SEMI_IMPLICIT_EULER) {
        // Semi-implicit Euler
        Eigen::MatrixXd acceleration = -computeGrad(stiffness) / mass_per_vertex;
        acceleration.rowwise() += acceleration_ext.transpose();

        if (enable_sphere_collision) {
            acceleration += acceleration_collision;
        }

        vel += h * acceleration * damping;

        // set fixed points to zero velocity
        for (unsigned i = 0; i < n_vertices; i++) {
            if (dirichlet_bc_mask[i]) {
                vel.row(i).setZero();
            }
        }

        X += h * vel;
    }
    else {
        std::cerr << "Unknown time integrator!" << std::endl;
        return;
    }
}

// There are different types of mass spring energy:
// For this homework we will adopt Prof. Huamin Wang's energy definition introduced in GAMES103
// course Lecture 2 E = 0.5 * stiffness * sum_{i=1}^{n} (||x_i - x_j|| - l)^2 There exist other
// types of energy definition, e.g., Prof. Minchen Li's energy definition
// https://www.cs.cmu.edu/~15769-f23/lec/3_Mass_Spring_Systems.pdf
double MassSpring::computeEnergy(double stiffness)
{
    double sum = 0.;
    unsigned i = 0;
    for (const auto& e : E) {
        auto diff = X.row(e.first) - X.row(e.second);
        auto l = E_rest_length[i];
        sum += 0.5 * stiffness * std::pow((diff.norm() - l), 2);
        i++;
    }
    return sum;
}

Eigen::MatrixXd MassSpring::computeGrad(double stiffness)
{
    Eigen::MatrixXd g = Eigen::MatrixXd::Zero(X.rows(), X.cols());
    unsigned i = 0;
    for (const auto& e : E) {
        auto diff = X.row(e.first) - X.row(e.second);
        auto l = E_rest_length[i];
        auto grad = stiffness * (diff.norm() - l) * diff / diff.norm();
        g.row(e.first) += grad;
        g.row(e.second) -= grad;
        i++;
    }
    return g;
}

Eigen::SparseMatrix<double> MassSpring::computeHessianSparse(double stiffness)
{
    unsigned n_vertices = X.rows();
    Eigen::SparseMatrix<double> H(n_vertices * 3, n_vertices * 3);

    unsigned i = 0;
    const auto I = Eigen::MatrixXd::Identity(3, 3);
    for (const auto& e : E) {
        Eigen::Vector3d diff = X.row(e.first) - X.row(e.second);
        auto l = E_rest_length[i];
        Eigen::MatrixXd H_e =
            stiffness * (1 - l / diff.norm()) * (I - diff * diff.transpose() / diff.squaredNorm()) +
            stiffness * (diff * diff.transpose() / diff.squaredNorm());

        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                H.coeffRef(3 * e.first + j, 3 * e.first + k) += H_e(j, k);
                H.coeffRef(3 * e.first + j, 3 * e.second + k) -= H_e(j, k);
                H.coeffRef(3 * e.second + j, 3 * e.first + k) -= H_e(j, k);
                H.coeffRef(3 * e.second + j, 3 * e.second + k) += H_e(j, k);
            }
        }

        // fix the fixed points
        if (dirichlet_bc_mask[e.first]) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    H.coeffRef(3 * e.first + j, 3 * e.first + k) = 0;
                }
                H.coeffRef(3 * e.first + j, 3 * e.first + j) = 1;
            }
        }

        i++;
    }

    H.makeCompressed();
    return H;
}

bool MassSpring::checkSPD(const Eigen::SparseMatrix<double>& A)
{
    // Eigen::SimplicialLDLT<SparseMatrix_d> ldlt(A);
    // return ldlt.info() == Eigen::Success;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A);
    const auto& eigen_values = es.eigenvalues();
    return eigen_values.minCoeff() >= 1e-10;
}

void MassSpring::reset()
{
    std::cout << "reset" << std::endl;
    this->X = this->init_X;
    this->vel.setZero();
}

// ----------------------------------------------------------------------------------
// (HW Optional) Bonus part
Eigen::MatrixXd MassSpring::getSphereCollisionForce(const Eigen::Vector3d& center, double radius)
{
    Eigen::MatrixXd force = Eigen::MatrixXd::Zero(X.rows(), X.cols());
    for (int i = 0; i < X.rows(); i++) {
        // (HW Optional) Implement penalty-based force here
    }
    return force;
}
// ----------------------------------------------------------------------------------

}  // namespace USTC_CG::node_mass_spring
