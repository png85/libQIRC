#include "Connection"

using namespace QIRC;

/// \brief Construct from given ServerInfo
Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL) {
  if (!setupSocket())
    exit(1);
}


/// \brief Construct from host/port
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


/// \brief Connect to IRC server
void Connection::connect() {
  Q_ASSERT(m_socket != NULL);
  m_socket->connectToHost(m_currentServer.host(), m_currentServer.port());
}


/// \brief Disconnect from IRC server
void Connection::disconnect() {
  Q_ASSERT(m_socket != NULL);
  m_socket->disconnectFromHost();
}


/// \brief Access current server for this connection
ServerInfo Connection::server() const {
  return m_currentServer;
}


/// \brief Set current server for this connection
///
/// \attention This method currently does NOT reconnect to the
/// new server in case the address differs from the current one!
/// The reconnect has to be initiated manually by calling
/// disconnect() and connect(). This will most likely get changed
/// in future versions. -- png
void Connection::setServer(ServerInfo& si) {
  // FIXME(png): reconnect if server differs from current one
  m_currentServer.setHost(si.host());
  m_currentServer.setPort(si.port());
}


/// \brief Set current server for this connection
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


/// \brief Slot for m_socket::disconnected()
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

