#include "EthernetInterface.h"

#include "../udp.hpp"

namespace asb
{
namespace platform
{
namespace udp
{

static EthernetInterface s_net;
static UDPSocket s_socket;

bool eth_open(struct uxrCustomTransport *transport)
{
  (void)transport;

  bool res = false;

  const SocketAddress ip(asb_IP);
  const SocketAddress netmask("255.255.192.0");
  const SocketAddress gateway("192.168.50.1");
  s_net.set_network(ip, netmask, gateway);
  s_net.set_dhcp(false);
  s_net.connect();

  if (NSAPI_ERROR_OK == s_socket.open(&s_net)) {
    const SocketAddress host(AGENT_IP, AGENT_PORT);
    if (NSAPI_ERROR_OK == s_socket.connect(host)) {
      res = true;
    }
  }

  return res;
}

bool eth_close(struct uxrCustomTransport *transport)
{
  (void)transport;

  bool res = false;

  if (NSAPI_ERROR_OK == s_socket.close()) {
    if (NSAPI_ERROR_OK == s_net.disconnect()) {
      res = true;
    }
  }

  return res;
}

size_t eth_write(uxrCustomTransport *transport, const void *buf, size_t len, uint8_t *err)
{
  (void)err;
  (void)transport;
  size_t sent = s_socket.send(buf, len);
  return sent;
}

size_t eth_read(uxrCustomTransport *transport, void *buf, size_t len, int timeout_ms, uint8_t *err)
{
  (void)err;
  (void)timeout_ms;
  (void)transport;

  s_socket.set_timeout(timeout_ms);
  size_t recv = s_socket.recv(buf, len);
  return recv;
}

}  // namespace udp
}  // namespace platform
}  // namespace asb
