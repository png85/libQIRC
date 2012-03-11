#include "Connection"

using namespace QIRC;

Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL) {
  if (!setupSocket())
    exit(1);
}


Connection::Connection(QString h, unsigned short p) :
  m_currentServer(h, p), m_socket(NULL) {
  if (!setupSocket())
    exit(1);
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
  ServerInfo si(h, p);
  setServer(si);
}


/// \brief Set up m_socket
///
/// Creates the QTcpSocket instance for m_socket
bool Connection::setupSocket() {
  if (m_socket != NULL) {
    qWarning() << "Called Connection::setupSocket with non-null m_socket!";
    delete m_socket;
  }

  try {
    m_socket = new QTcpSocket(this);
  }

  catch (std::bad_alloc &ex) {
    qCritical() << "Caught std::bad_alloc when trying to setup "
		<< "Connection::m_socket: " << ex.what();
    return false;
  }

  // connect the socket's signals to our slots
  QObject::connect(m_socket, SIGNAL(connected()),
		   this, SLOT(socket_connected()));
  QObject::connect(m_socket, SIGNAL(disconnected()),
		   this, SLOT(socket_disconnected()));
  QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
		   this, SLOT(socket_error(QAbstractSocket::SocketError)));
  QObject::connect(m_socket, SIGNAL(readyRead()),
		   this, SLOT(socket_readyRead()));

  return true;
}


/// \brief Slot for m_socket::connected()
void Connection::socket_connected() {
  qDebug() << "Connection::m_socket connected!";
}


/// b\rief Slot for m_socket::disconnected()
void Connection::socket_disconnected() {
  qDebug() << "Connection::m_socket disconnected!";
}


/// \brief Slot for m_socket::error()
void Connection::socket_error(QAbstractSocket::SocketError err) {
  qDebug() << "Connection::m_socket generated an error:"
	   << err;
}


/// \brief Slot for m_socket::readyRead()
void Connection::socket_readyRead() {
  qDebug() << "Connection::m_socket has data to read!";
}

