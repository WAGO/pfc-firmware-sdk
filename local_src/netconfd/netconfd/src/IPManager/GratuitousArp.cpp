// SPDX-License-Identifier: GPL-2.0-or-later

#include "GratuitousArp.hpp"

#include "Socket.hpp"

#include <gsl/gsl>

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include "errno.h"

namespace netconf {

#define MAC_LEN 6
#define ETHER_TYPE_LEN 2
#define ETH_HDRLEN (MAC_LEN + MAC_LEN + ETHER_TYPE_LEN)

struct __attribute__((packed)) ArpHeader {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint32_t sender_ip;
  uint8_t target_mac[6];
  uint32_t target_ip;
};


static ArpHeader getArpHeader(const MacAddress src_mac, uint32_t src_ip) {

  ArpHeader header = {};
  header.htype = htons(1);  // Hardware type (16 bits): 1 for ethernet
  header.ptype = htons(ETH_P_IP);
  header.hlen = MAC_LEN;
  header.plen = 4;  //4 bytes for IPv4 address
  header.opcode = htons(2);  // OpCode: 2 for Gratuitous ARP
  memcpy(&header.sender_mac, src_mac.data(), MAC_LEN);
  header.sender_ip = src_ip;
  memset(&header.target_mac, 0xff, MAC_LEN);
  header.target_ip = src_ip;

  return header;
}

static void prepareArpFrame(::gsl::span<uint8_t> frame, const MacAddress src_mac, ArpHeader &arp_header) {

  memset(frame.data(), 0xff, MAC_LEN);  // Set destination MAC address: broadcast address
  memcpy(&frame[6], src_mac.data(), MAC_LEN);
  frame[12] = ETH_P_ARP / 256;
  frame[13] = ETH_P_ARP % 256;
  memcpy(&frame[14], &arp_header, sizeof(arp_header));

}

static struct sockaddr_ll getDevice(MacAddress src_mac, uint itf_index){

  struct sockaddr_ll device = {};
  device.sll_family = AF_PACKET;
  device.sll_ifindex = itf_index;
  memcpy(device.sll_addr, src_mac.data(), MAC_LEN);
  device.sll_halen = static_cast<uint8_t>(htons(MAC_LEN));

  return device;
}

Status GratuitousArp::Send(::std::string ip_address, const NetDevPtr bridge, const NetDevPtr egress_port) {

  uint32_t src_ip;
  if (1 != inet_pton(AF_INET, ip_address.c_str(), &src_ip)) {
    return Status { StatusCode::IPV4_FORMAT, ip_address };
  }

  uint8_t buffer[ETH_HDRLEN + sizeof(ArpHeader)] = {};
  ::gsl::span<uint8_t> frame(buffer);

  ArpHeader arp_hdr = getArpHeader(bridge->GetMac(), src_ip);
  prepareArpFrame(frame, bridge->GetMac(), arp_hdr);
  auto device = getDevice(egress_port->GetMac(), egress_port->GetIndex());

  Socket s(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  int ret = 0;
  if((ret = sendto(s, frame.data(), frame.size(), 0, (struct sockaddr*) &device, sizeof(device))) == -1){
    return Status { StatusCode::SYSTEM_CALL, ::std::string{strerror(errno)} };
  }

  return Status();

}

}
