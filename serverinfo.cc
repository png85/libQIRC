/// \file
/// \brief Implementation of ServerInfo utility class
///
/// \author png!das-system
#include "ServerInfo"

using namespace QIRC;


/// \brief Construct new ServerInfo
ServerInfo::ServerInfo(QString host, quint16 port) :
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
quint16 ServerInfo::port() {
  return m_port;
}


/// \brief Set port number to new value
void ServerInfo::setPort(quint16 p) {
  if (m_port != p)
    m_port = p;
}


/// \brief String representation for logging/debugging
QString ServerInfo::toString() {
  QString r = "ServerInfo:{host=" + m_host + "; " +
    "port=" + QString::number(m_port) + "}";

  return r;
}


/// \brief Equality operator
bool ServerInfo::operator ==(const ServerInfo& o) const {
  return ((m_host == o.m_host) && (m_port == o.m_port));
}


/// \brief Inequality operator
bool ServerInfo::operator !=(const ServerInfo& o) const {
  return !((*this) == o);
}

