/// \file
/// \brief Implementation of HostMask utility class
///
/// \author png!das-system
#include <QRegExp>
#include "HostMask"

using namespace QIRC;


/// \brief Default constructor from 3 separate strings
///
/// \param nick Nickname part of hostmask
/// \param user Username part of hostmask
/// \param host Hostname part of hostmask
HostMask::HostMask(QString nick, QString user, QString host) :
  m_nick(nick), m_user(user), m_host(host) {}


/// \brief Copy constructor
HostMask::HostMask(const HostMask& other) :
  m_nick(other.m_nick), m_user(other.m_user), m_host(other.m_host) {}


/// \brief Access nickname part
QString HostMask::nick() const {
  return m_nick;
}


/// \brief Set nickname part to new value
void HostMask::setNick(QString n) {
  if (m_nick != n) {
    m_nick = n;
  }
}


/// \brief Access username part
QString HostMask::user() const {
  return m_user;
}


/// \brief Set username part to new value
void HostMask::setUser(QString u) {
  if (m_user != u) {
    m_user = u;
  }
}


/// \brief Access hostname part
QString HostMask::host() const {
  return m_host;
}


/// \brief Set hostname part to new value
void HostMask::setHost(QString h) {
  if (m_host != h) {
    m_host = h;
  }
}


/// \brief String representation for logging/debugging
QString HostMask::toString() const {
  return (m_nick + "!" + m_user + "@" + m_host);
}


/// \brief Equality operator
bool HostMask::operator ==(const HostMask& o) const {
  return ((m_nick == o.m_nick) && (m_user == o.m_user) &&
	  (m_host == o.m_host));
}


/// \brief Inequality operator
bool HostMask::operator !=(const HostMask& o) const {
  return !(*this == o);
}

