/*
 * Copyright (c) 2017 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vom/neighbour_cmds.hpp"
#include "vom/api_types.hpp"

namespace VOM {
namespace neighbour_cmds {
create_cmd::create_cmd(HW::item<bool>& item,
                       handle_t itf,
                       const mac_address_t& mac,
                       const boost::asio::ip::address& ip_addr)
  : rpc_cmd(item)
  , m_itf(itf)
  , m_mac(mac)
  , m_ip_addr(ip_addr)
{
}

bool
create_cmd::operator==(const create_cmd& other) const
{
  return ((m_mac == other.m_mac) && (m_ip_addr == other.m_ip_addr) &&
          (m_itf == other.m_itf));
}

rc_t
create_cmd::issue(connection& con)
{
  msg_t req(con.ctx(), std::ref(*this));

  auto& payload = req.get_request().get_payload();
  payload.is_add = 1;
  payload.neighbor.sw_if_index = m_itf.value();
  payload.neighbor.flags = IP_API_NEIGHBOR_FLAG_STATIC;

  to_api(m_mac, payload.neighbor.mac_address);
  to_api(m_ip_addr, payload.neighbor.ip_address);

  VAPI_CALL(req.execute());

  return (wait());
}

std::string
create_cmd::to_string() const
{
  std::ostringstream s;
  s << "nieghbour-create: " << m_hw_item.to_string()
    << " itf:" << m_itf.to_string() << " mac:" << m_mac.to_string()
    << " ip:" << m_ip_addr.to_string();

  return (s.str());
}

delete_cmd::delete_cmd(HW::item<bool>& item,
                       handle_t itf,
                       const mac_address_t& mac,
                       const boost::asio::ip::address& ip_addr)
  : rpc_cmd(item)
  , m_itf(itf)
  , m_mac(mac)
  , m_ip_addr(ip_addr)
{
}

bool
delete_cmd::operator==(const delete_cmd& other) const
{
  return ((m_mac == other.m_mac) && (m_ip_addr == other.m_ip_addr) &&
          (m_itf == other.m_itf));
}

rc_t
delete_cmd::issue(connection& con)
{
  msg_t req(con.ctx(), std::ref(*this));

  auto& payload = req.get_request().get_payload();
  payload.is_add = 0;
  payload.neighbor.sw_if_index = m_itf.value();
  payload.neighbor.flags = IP_API_NEIGHBOR_FLAG_STATIC;

  to_api(m_mac, payload.neighbor.mac_address);
  to_api(m_ip_addr, payload.neighbor.ip_address);

  VAPI_CALL(req.execute());

  wait();
  m_hw_item.set(rc_t::NOOP);

  return rc_t::OK;
}

std::string
delete_cmd::to_string() const
{
  std::ostringstream s;
  s << "neighbour-delete: " << m_hw_item.to_string()
    << " itf:" << m_itf.to_string() << " mac:" << m_mac.to_string()
    << " ip:" << m_ip_addr.to_string();

  return (s.str());
}

dump_cmd::dump_cmd(const handle_t& hdl, const l3_proto_t& proto)
  : m_itf(hdl)
  , m_proto(proto)
{
}

dump_cmd::dump_cmd(const dump_cmd& d)
  : m_itf(d.m_itf)
  , m_proto(d.m_proto)
{
}

bool
dump_cmd::operator==(const dump_cmd& other) const
{
  return (true);
}

rc_t
dump_cmd::issue(connection& con)
{
  m_dump.reset(new msg_t(con.ctx(), std::ref(*this)));

  auto& payload = m_dump->get_request().get_payload();
  payload.sw_if_index = m_itf.value();
  payload.is_ipv6 = m_proto.is_ipv6();

  VAPI_CALL(m_dump->execute());

  wait();

  return rc_t::OK;
}

std::string
dump_cmd::to_string() const
{
  std::ostringstream s;

  s << "neighbour-dump: " << m_itf.to_string() << " " << m_proto.to_string();

  return (s.str());
}
} // namespace neighbour_cmds
} // namespace vom

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "mozilla")
 * End:
 */
