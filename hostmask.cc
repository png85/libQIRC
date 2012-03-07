#include <QRegExp>
#include "HostMask"

using namespace QIRC;

HostMask::HostMask(QString nick, QString user, QString host) :
  m_nick(nick), m_user(user), m_host(host) {}


HostMask::HostMask(const HostMask& other) :
  m_nick(other.m_nick), m_user(other.m_user), m_host(other.m_host) {}


QString HostMask::nick() {
  return m_nick;
}


void HostMask::setNick(QString n) {
  if (m_nick != n) {
    m_nick = n;
  }
}


void HostMask::setUser(QString u) {
  if (m_user != u) {
    m_user = u;
  }
}


QString HostMask::host() {
  return m_host;
}


void HostMask::setHost(QString h) {
  if (m_host != h) {
    m_host = h;
  }
}


QString HostMask::toString() {
  return (m_nick + "!" + m_user + "@" + m_host);
}

