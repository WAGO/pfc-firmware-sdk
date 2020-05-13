/*
 * BridgeConfigure.cpp
 *
 *  Created on: 17.02.2020
 *      Author: u014487
 */

#include "BridgeConfig.hpp"
#include "NetconfdDbusClient.hpp"
namespace netconf {




  BridgeConfig GetBridgeConfig()
  {
    NetconfdDbusClient client;
    return BridgeConfig{client.GetBridgeConfig()};
  }

  Status SetBridgeConfig(const BridgeConfig &config)
  {
    NetconfdDbusClient client;
    if(client.SetBridgeConfig(config.ToJson()))
    {
      return Status::OK;
    }
    return Status::ERROR;
  }

}  // namespace netconf
