
/*
THIS FILE HAS BEEN AUTOGENERATED. DO NOT EDIT!
*/

#include "types.h"

#include "rte.hpp"

brt::types::torque_Nm const &asb::rte::ACTUAL_TORQUE_Nm::ro() {
    return static_cast<brt::types::torque_Nm const &>(rw());
}

brt::types::torque_Nm &asb::rte::ACTUAL_TORQUE_Nm::rw() {
    static brt::types::torque_Nm data = 0.F;
    return static_cast<brt::types::torque_Nm &>(data);
}

brt::types::torque_Nm const &asb::rte::REQUIRED_TORQUE_Nm::ro() {
    return static_cast<brt::types::torque_Nm const &>(rw());
}

brt::types::torque_Nm &asb::rte::REQUIRED_TORQUE_Nm::rw() {
    static brt::types::torque_Nm data = 0.F;
    return static_cast<brt::types::torque_Nm &>(data);
}

brt::types::eBits8 const &asb::rte::ACS_ERROR::ro() {
    return static_cast<brt::types::eBits8 const &>(rw());
}

brt::types::eBits8 &asb::rte::ACS_ERROR::rw() {
    static brt::types::eBits8 data = brt::types::BIT0;
    return static_cast<brt::types::eBits8 &>(data);
}

brt::types::condition const &asb::rte::FSM_MISSION_REQUEST::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_MISSION_REQUEST::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_INIT_CHECK_UP_REQUEST::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_INIT_CHECK_UP_REQUEST::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_GO_SIGNAL::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_GO_SIGNAL::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_RESET_ASB::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_RESET_ASB::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_ASB_FINISH::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_ASB_FINISH::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_INIT_CHECK_UP_EMERGENCY::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_INIT_CHECK_UP_EMERGENCY::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_INIT_CHECK_UP_OK::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_INIT_CHECK_UP_OK::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}

brt::types::condition const &asb::rte::FSM_RUN_EMERGENCY::ro() {
    return static_cast<brt::types::condition const &>(rw());
}

brt::types::condition &asb::rte::FSM_RUN_EMERGENCY::rw() {
    static brt::types::condition data = false;
    return static_cast<brt::types::condition &>(data);
}