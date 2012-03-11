#include "Connection"

using namespace QIRC;

Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL) {
  try {
    m_socket = new QTcpSocket(this);
  }

  catch (std::bad_alloc& ex) {
    qCritical() << "Caught std::bad_alloc in Connection ctor: "
		<< ex.what();
    exit(1);
  }
}


Connection::Connection(QString h, unsigned short p) :
  m_currentServer(h, p), m_socket(NULL) {
  try {
    m_socket = new QTcpSocket(this);
  }

  catch (std::bad_alloc& ex) {
    qCritical() << "Caught std::bad_alloc in Connection ctor: "
		<< ex.what();
  }
}


Connection::~Connection() {
  if (m_socket != NULL) {
    delete m_socket;
  }
}


void Connection::connect() {
  /// TODO(png): ...
}


void Connection::disconnect() {
  /// TODO(png): ...
}


ServerInfo Connection::server() const {
  return m_currentServer;
}


void Connection::setServer(ServerInfo& si) {
  // FIXME(png): reconnect if server differs from current one
  m_currentServer.setHost(si.host());
  m_currentServer.setPort(si.port());
}


void Connection::setServer(QString h, unsigned short p) {
  // FIXME(png): reconnect if server differs from current one
  m_currentServer.setHost(h);
  m_currentServer.setPort(p);
}
