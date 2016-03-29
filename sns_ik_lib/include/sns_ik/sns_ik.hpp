/*
 *    Copyright 2016 Rethink Robotics
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
// Author: Ian McMahon

#ifndef SNS_IK_HPP
#define SNS_IK_HPP

#include <string>
#include <memory>
#include <kdl/chain.hpp>
#include <kdl/frames.hpp>
#include <kdl/jntarray.hpp>
#include <kdl/chainfksolverpos_recursive.hpp>
#include <sns_ik/sns_position_ik.hpp>

namespace sns_ik {

  enum VelocitySolveType { SNS,
                           SNS_Optimal,
                           SNS_OptimalScaleMargin,
                           SNS_Fast,
                           SNS_FastOptimal
                         };

  // Forward declare SNS Velocity Base Class
  class SNSVelocityIK;
  class SNS_IK
  {
  public:
    SNS_IK(const std::string& base_link, const std::string& tip_link,
           const std::string& URDF_param="/robot_description",
           double looprate=0.005, double eps=1e-5,
           sns_ik::VelocitySolveType type=sns_ik::SNS);

    SNS_IK(const KDL::Chain& chain,
           const KDL::JntArray& q_min, const KDL::JntArray& q_max,
           const KDL::JntArray& v_max, const KDL::JntArray& a_max,
           const std::vector<std::string>& jointNames,
           double looprate=0.005, double eps=1e-5,
           sns_ik::VelocitySolveType type=sns_ik::SNS);

    ~SNS_IK();

    bool setVelocitySolveType(VelocitySolveType type);

    inline bool getPositionSolver(std::shared_ptr<sns_ik::SNSPositionIK>& positionSolver) {
      positionSolver=m_ik_pos_solver;
      return m_initialized;
    }

    inline bool getVelocitySolver(std::shared_ptr<sns_ik::SNSVelocityIK>& velocitySolver) {
      velocitySolver=m_ik_vel_solver;
      return m_initialized;
    }

    inline bool getKDLChain(KDL::Chain& chain) {
      chain=m_chain;
      return m_initialized;
    }

    inline bool getKDLLimits(KDL::JntArray& lb, KDL::JntArray& ub, KDL::JntArray& vel, KDL::JntArray& accel) {
      lb=m_lower_bounds;
      ub=m_upper_bounds;
      vel=m_velocity;
      accel=m_acceleration;
      return m_initialized;
    }

    inline bool getJointNames(std::vector<std::string> jointNames) {
      jointNames = m_jointNames;
      return m_initialized;
    }

    int CartToJnt(const KDL::JntArray &q_init,
                  const KDL::Frame &p_in,
                  KDL::JntArray &q_out,
                  const KDL::Twist& tolereances=KDL::Twist::Zero())
    { return CartToJnt(q_init, p_in, KDL::JntArray(0), std::vector<std::string>(),
                       q_out, tolereances);
    }

    int CartToJnt(const KDL::JntArray &q_init, const KDL::Frame &p_in,
                  const KDL::JntArray& q_bias,
                  KDL::JntArray &q_out,
                  const KDL::Twist& tolerances=KDL::Twist::Zero())
    { return CartToJnt(q_init, p_in, q_bias, m_jointNames,
                       q_out, tolerances);
    }

    int CartToJnt(const KDL::JntArray &q_init, const KDL::Frame &p_in,
                  const KDL::JntArray& q_bias,
                  const std::vector<std::string>& biasNames,
                  KDL::JntArray &q_out,
                  const KDL::Twist& tolerances=KDL::Twist::Zero());

    int CartToJnt(const KDL::JntArray& q_in,
                  const KDL::Twist& v_in,
                  KDL::JntArray& qdot_out)
    { return CartToJnt(q_in, v_in, KDL::JntArray(0), std::vector<std::string>(), qdot_out); }

    int CartToJnt(const KDL::JntArray& q_in,
                  const KDL::Twist& v_in,
                  const KDL::JntArray& q_bias,
                  KDL::JntArray& qdot_out)
    { return CartToJnt(q_in, v_in, q_bias, m_jointNames, qdot_out); }

    int CartToJnt(const KDL::JntArray& q_in,
                  const KDL::Twist& v_in,
                  const KDL::JntArray& q_bias,
                  const std::vector<std::string>& biasNames,
                  KDL::JntArray& qdot_out);

  private:
    bool m_initialized;
    double m_eps;
    double m_looprate;
    VelocitySolveType m_solvetype;
    KDL::Chain m_chain;
    KDL::JntArray m_lower_bounds, m_upper_bounds, m_velocity, m_acceleration;
    enum JointType { Revolute, Prismatic, Continuous };
    std::vector<JointType> m_types;
    std::vector<std::string> m_jointNames;

    std::vector<KDL::JntArray> m_solutions;
    std::shared_ptr<SNSVelocityIK> m_ik_vel_solver;
    std::shared_ptr<SNSPositionIK> m_ik_pos_solver;
    std::shared_ptr<KDL::ChainJntToJacSolver> m_jacobianSolver;

    void initialize();

    bool nullspaceBiasTask(const KDL::JntArray& q_bias,
                           const std::vector<std::string>& biasNames,
                           MatrixD* jacobian, std::vector<int>* indicies);

  };
}  //namespace
#endif
