// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * InterfaceConfigManagerBaseTest.h
 *
 *  Created on: 23.10.2019
 *      Author: u014487
 */

#ifndef NETCONFD_TEST_SRC_INTERFACECONFIGMANAGERBASETEST_H_
#define NETCONFD_TEST_SRC_INTERFACECONFIGMANAGERBASETEST_H_

#include "InterfaceConfigManager.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include <list>

static ::std::uint32_t if_index_counter = 0;
namespace netconf {

class InterfaceConfigManagerBaseTest {
 public:

  class FakeEthernetInterface : public IEthernetInterface {
   public:

    FakeEthernetInterface(::std::string name, InterfaceConfigManagerBaseTest& upper)
        : mac{0},
          name_ { name },
          upper_ { upper },
          state_ {eth::DeviceState::Down},
          link_state_{eth::InterfaceLinkState::Down},
          if_index_{++if_index_counter},
          autoneg_{eth::Autoneg::Off},
          speed_{-1},
          duplex_{eth::Duplex::Half},
          committed_{false}
          {
            upper_.EthernetInterfaceCreated(this);
    }
    ~FakeEthernetInterface() {
      upper_.EthernetInterfaceDeleted(this);
    }
    void UpdateConfig() {};
    ::std::string const& GetName() const { return name_;}
    eth::EthernetMauType GetMauType() const { return eth::EthernetMauType::TYPE_NONE;}
    MacAddress GetMac() const { return MacAddress{mac};}
    bool GetAutonegSupport() const { return true;}
    bool GetAutonegEnabled() const {return autoneg_ == eth::Autoneg::On;}
    ::std::uint32_t GetAutonegCapabilities() const {return 0;}
    eth::MediaType GetMediaType() const {return eth::MediaType::TP;}
    eth::DeviceState GetState() const {return state_;}
    eth::InterfaceLinkState GetLinkState() const {return link_state_;}
    ::std::uint32_t GetInterfaceIndex() const {return if_index_;}
    ::std::uint32_t GetAutonegCapabilitiesXdot3() const {return 0;}
    ::std::uint32_t GetMTU() const {return 1500;}
    void Commit() {committed_ = true;}
    void SetAutoneg(eth::Autoneg autoneg) {autoneg_ = autoneg;}
    void SetState(eth::DeviceState state) {state_ = state;}
    void SetSpeed(::std::int32_t speed) {speed_ = speed;}
    void SetDuplex(eth::Duplex duplex) {duplex_ = duplex;}

    uint8_t mac[6];
    ::std::string name_;
    InterfaceConfigManagerBaseTest& upper_;
    eth::DeviceState state_;
    eth::InterfaceLinkState link_state_;
    ::std::uint32_t if_index_;
    eth::Autoneg autoneg_;
    ::std::int32_t speed_;
    eth::Duplex duplex_;
    bool committed_;

   private :


  };

  class FakeEthernetInterfaceFactory : public IEthernetInterfaceFactory {
   public:

    FakeEthernetInterfaceFactory(InterfaceConfigManagerBaseTest& upper) : upper_{upper} {

    }

    ::std::unique_ptr<IEthernetInterface> getEthernetInterface(std::string name) override {
      return ::std::make_unique<FakeEthernetInterface>(name, upper_);
    }

    InterfaceConfigManagerBaseTest& upper_;
  };

  InterfaceConfigManagerBaseTest() = default;

  void EthernetInterfaceCreated(FakeEthernetInterface* itf) {
    created_ethernet_interfaces.push_back(itf);
  }
  void EthernetInterfaceDeleted(FakeEthernetInterface* itf) {
    created_ethernet_interfaces.remove(itf);
  }

  ::std::list<FakeEthernetInterface*> created_ethernet_interfaces;

  FakeEthernetInterface* FindCreatedEthernetInterface(::std::string name)
  {
    auto name_matches = [&name](FakeEthernetInterface* itf) {
      return name == itf->name_;
    };

    auto found_itf = std::find_if(created_ethernet_interfaces.begin(), created_ethernet_interfaces.end() , name_matches);
    if(found_itf != created_ethernet_interfaces.end())
      return *found_itf;
    return nullptr;
  }

};

} /* namespace netconf */

#endif /* NETCONFD_TEST_SRC_INTERFACECONFIGMANAGERBASETEST_H_ */
