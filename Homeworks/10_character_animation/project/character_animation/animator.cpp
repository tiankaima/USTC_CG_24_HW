#include "animator.h"
#include <cassert>

namespace USTC_CG::node_character_animation {

using namespace pxr;

Joint::Joint(int idx, string name, int parent_idx,
             const GfMatrix4f &bind_transform)
    : idx_(idx), name_(name), parent_idx_(parent_idx),
      bind_transform_(bind_transform) {}

void Joint::compute_world_transform() {
  if (parent_) {
    world_transform_ =
        parent_->get_world_transform() * get_local_transform().GetTranspose();
  } else {
    world_transform_ = get_local_transform().GetTranspose();
  }
}

void JointTree::compute_world_transforms_for_each_joint() {
  for (int i = 0; i < joints_.size(); ++i) {
    joints_[i]->compute_world_transform();
  }
}

void JointTree::add_joint(int idx, std::string name, int parent_idx,
                          const GfMatrix4f &bind_transform) {
  auto joint = make_shared<Joint>(idx, name, parent_idx, bind_transform);
  joints_.push_back(joint);
  if (parent_idx < 0) {
    root_ = joint;
  } else {
    joints_[parent_idx]->children_.push_back(joint);

    if (parent_idx < joints_.size())
      joint->parent_ = joints_[parent_idx];
    else {
      std::cout << "[add_joint_error] parent_idx out of range" << std::endl;
      exit(1);
    }
  }
}

void JointTree::update_joint_local_transform(
    const VtArray<GfMatrix4f> &new_local_transforms) {
  assert(new_local_transforms.size() == joints_.size());

  for (int i = 0; i < joints_.size(); ++i) {
    joints_[i]->local_transform_ = new_local_transforms[i];
  }
}

void JointTree::print() {
  for (auto joint_ptr : joints_) {
    std::cout << "Joint idx: " << joint_ptr->idx_
              << " name: " << joint_ptr->name_
              << " parent_idx: " << joint_ptr->parent_idx_ << std::endl;
  }
}

Animator::Animator(const shared_ptr<MeshComponent> mesh,
                   const shared_ptr<SkelComponent> skel)
    : mesh_(mesh), skel_(skel) {
  auto joint_order = skel_->jointOrder;
  auto topology = skel_->topology;
  for (size_t i = 0; i < joint_order.size(); ++i) {
    SdfPath jointPath(joint_order[i]);

    string joint_name = jointPath.GetName();
    int parent_idx = topology.GetParent(i);

    joint_tree_.add_joint(i, joint_name, parent_idx,
                          GfMatrix4f(skel->bindTransforms[i]));
  }
}

void Animator::step(const shared_ptr<SkelComponent> skel) {
  joint_tree_.update_joint_local_transform(skel->localTransforms);

  joint_tree_.compute_world_transforms_for_each_joint();

  update_mesh_vertices();
}

void Animator::update_mesh_vertices() {
  const auto &jointIndices = skel_->jointIndices;
  const auto &jointWeight = skel_->jointWeight;

  int n = mesh_->vertices.size();
  int m = jointIndices.size() / n;
  for (size_t i = 0; i < n; ++i) {
    GfMatrix4f transform;
    transform.SetZero();
    for (size_t j = 0; j < m; ++j) {
      size_t jointIndex = size_t(jointIndices[j + i * m]);
      transform += joint_tree_.get_joint(jointIndex)->get_world_transform() *
                   joint_tree_.get_joint(jointIndex)
                       ->get_bind_transform()
                       .GetTranspose()
                       .GetInverse() *
                   jointWeight[j + i * m];
    }

    mesh_->vertices[i] =
        transform.GetTranspose().TransformAffine(mesh_->vertices[i]);
  }
}

} // namespace USTC_CG::node_character_animation