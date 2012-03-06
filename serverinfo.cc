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
void ServerInfo::setHost(QString host) {
  if (m_host != host)
    m_host = host;
}


/// \brief Access port number
unsigned short ServerInfo::port() {
  return m_port;
}


/// \brief Set port number to new value
void ServerInfo::setPort(unsigned short port) {
  if (m_port != port)
    m_port = port;
}


/// \brief String representation for logging/debugging
QString ServerInfo::toString() {
  QString r = "ServerInfo:{host=" + m_host + "; " +
    "port=" + QString::number(m_port) + "}";

  return r;
}
