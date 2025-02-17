/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

extern "C" {
#include <opennsl/types.h>
#include <opennsl/port.h>
}

#include "fboss/agent/types.h"
#include "fboss/agent/hw/bcm/BcmPort.h"

#include <mutex>
#include <boost/container/flat_map.hpp>

namespace facebook { namespace fboss {

class BcmSwitch;

class BcmPortTable {
 public:
  explicit BcmPortTable(BcmSwitch *hw);
  ~BcmPortTable();

  /*
   * Initialize the port table from the list of physical switch ports.
   *
   * No other BcmPortTable methods should be accessed before initPorts()
   * completes.
   */
  void initPorts(const opennsl_port_config_t* portConfig, bool warmBoot);

  /*
   * Getters.
   */
  opennsl_port_t getBcmPortId(PortID id) const {
    return static_cast<opennsl_port_t>(id);
  }
  PortID getPortId(opennsl_port_t port) const {
    return PortID(port);
  }

  /*
   * Indicate that a port's link status has changed.
   */
  void setPortStatus(opennsl_port_t id, int status);

  /*
   * Update all ports' statistics.
   */
  void updatePortStats();

  bool portExists(PortID port) const {
    return getBcmPortIf(port) != nullptr;
  }
  bool portExists(opennsl_port_t port) const {
    return getBcmPortIf(port) != nullptr;
  }
 private:
  typedef boost::container::flat_map<opennsl_port_t, std::unique_ptr<BcmPort>>
    BcmPortMap;
  typedef boost::container::flat_map<PortID, BcmPort*> FbossPortMap;

  // throw an error if not found
  BcmPort* getBcmPort(PortID id) const;
  BcmPort* getBcmPort(opennsl_port_t id) const;

  // return nullptr if not found
  BcmPort* getBcmPortIf(PortID id) const;
  BcmPort* getBcmPortIf(opennsl_port_t id) const;

  BcmSwitch *hw_{nullptr};

  // Mappings for the physical ports.
  // The set of physical ports is defined in initPorts(), and is read-only
  // afterwards (since the physical ports on the switch cannot change).
  // Therefore we don't need any locking on this data structure.
  // (Modifiable data in the BcmPort objects themselves does require locking,
  // though.)

  // A mapping from opennsl_port_t to BcmPort.
  BcmPortMap bcmPhysicalPorts_;
  // A mapping from FBOSS PortID to BcmPort.
  FbossPortMap fbossPhysicalPorts_;
};

}} // namespace facebook::fboss
