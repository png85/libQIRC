#include "ServerInfo"

using namespace QIRC;


/// \brief Construct new ServerInfo
ServerInfo::ServerInfo(QString host, unsigned short port) :
  m_host(host), m_port(port) {}


/// \brief Copy Constructor
ServerInfo::ServerInfo(const ServerInfo& o) :
  m_host(o.m_host), m_port(o.m_port) {}


/// \brief Access host part
QString ServerInfo::host() {
  return m_host;
}


/// \brief Set host part to new value
void ServerInfo::setHost(QString h) {
  if (m_host != h)
    m_host = h;
}


/// \brief Access port number
unsigned short ServerInfo::port() {
  return m_port;
}


/// \brief Set port number to new value
void ServerInfo::setPort(unsigned short p) {
  if (m_port != p)
    m_port = p;
}


/// \brief String representation for logging/debugging
QString ServerInfo::toString() {
  QString r = "ServerInfo:{host=" + m_host + "; " +
    "port=" + QString::number(m_port) + "}";

  return r;
}
