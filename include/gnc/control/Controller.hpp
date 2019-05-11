#pragma once

#include <array>
#include <atomic>
#include <list>
#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>
#include <zcm/zcm-cpp.hpp>

#include <common/mavlink/AutopilotInterface.hpp>
#include <common/messages/ctrl_params_t.hpp>
#include <common/messages/path_t.hpp>
#include <common/messages/pid_error_t.hpp>
#include <gnc/State.hpp>
#include <gnc/control/Pid.hpp>
#include <gnc/measurements/Waypoint.hpp>

namespace maav
{
namespace gnc
{
namespace control
{
struct XboxController
{
    int left_joystick_x;
    int left_joystick_y;
    int right_joystick_x;
    int right_joystick_y;
    int left_trigger;
    int right_trigger;
};

class Controller
{
public:
    struct Parameters
    {
        std::array<std::array<double, 3>, 4> pos_gains;         // x, y, z, yaw
        std::array<std::array<double, 3>, 4> rate_gains;        // x, y, z, ems_z
        std::array<std::pair<double, double>, 4> rate_limits;   // x, y, z, yaw
        std::array<std::pair<double, double>, 3> accel_limits;  // x, y, z
        std::array<std::pair<double, double>, 2> angle_limits;  // roll, pitch
        std::pair<double, double> thrust_limits{1, 0};
        double takeoff_alt;
        double setpoint_tol;  //< convergence tolerance for achieving setpoints
        std::string zcm_url;
        double ff_thrust;
    };

    Controller(const YAML::Node& control_config);
    ~Controller();
    void set_path(const path_t& _path);
    void set_current_target(const Waypoint& new_target);
    void add_state(const State& state);
    void add_ems_state(const State& state);
    maav::mavlink::InnerLoopSetpoint flight();
    maav::mavlink::InnerLoopSetpoint run_xbox(const XboxController& xbox_controller);
    void set_control_params(const Parameters&);
    void set_control_params(const ctrl_params_t& ctrl_params);
    maav::mavlink::InnerLoopSetpoint takeoff(const double takeoff_alt);
    maav::mavlink::InnerLoopSetpoint land();
    maav::mavlink::InnerLoopSetpoint ems_land();
    bool at_takeoff_alt();
    void set_yaw_north();

private:
    maav::mavlink::InnerLoopSetpoint move_to_current_target();

    const YAML::Node control_config_;

    State current_state;
    State ems_state;
    double ems_dt;
    double total_distance_to_target;
    float origin_yaw;
    float yaw_north;

    path_t path;
    int16_t path_counter;
    Waypoint current_target;
    double convergence_tolerance = 0.5;  // set with veh params
    bool converged_on_waypoint;

    control::Pid x_position_pid;
    control::Pid x_velocity_pid;
    control::Pid y_position_pid;
    control::Pid y_velocity_pid;
    control::Pid z_position_pid;
    control::Pid z_velocity_pid;
    control::Pid emz_z_velocity_pid;
    Parameters veh_params;

    double takeoff_alt_setpoint;

    // For controller
    float xbox_yaw = 0;

    std::chrono::high_resolution_clock::time_point controller_run_loop_1 =
        std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point controller_run_loop_2 =
        std::chrono::high_resolution_clock::now();

    zcm::ZCM zcm;
    pid_error_t pid_error_msg;
};

}  // namespace control
}  // namespace gnc
}  // namespace maav