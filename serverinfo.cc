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
QString ServerInfo::host() const {
  return m_host;
}


/// \brief Set host part to new value
void ServerInfo::setHost(QString h) {
  if (m_host != h)
    m_host = h;
}


/// \brief Access port number
quint16 ServerInfo::port() const {
  return m_port;
}


/// \brief Set port number to new value
void ServerInfo::setPort(quint16 p) {
  if (m_port != p)
    m_port = p;
}


/// \brief String representation for logging/debugging
QString ServerInfo::toString() const {
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


/// \brief Assignment operator
///
/// Copies the host/port info from another ServerInfo instance.
ServerInfo& ServerInfo::operator =(const ServerInfo& o) {
  if (this != &o) {
    m_host = o.m_host;
    m_port = o.m_port;
  }

  return (*this);
}


/// \brief Output a ServerInfo to a QDebug stream
///
/// Creates a string in the form of 'host:port' and sends it to
/// the given debug stream.
QDebug& operator <<(QDebug& dbg, const ServerInfo& si) {
  QString tmp = si.host() + ":" + QString::number(si.port());
  
  return (dbg << tmp);
}

