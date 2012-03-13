#include "Connection"

using namespace QIRC;

/// \brief Construct from given ServerInfo
Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL) {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }
}


/// \brief Construct from host/port
Connection::Connection(QString h, quint16 p) :
  m_currentServer(h, p), m_socket(NULL) {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }
}


Connection::~Connection() {
  if (m_socket != NULL) {
    delete m_socket;
  }
}


/// \brief Connect to IRC server
void Connection::connect() {
  Q_ASSERT(m_socket != NULL);
  if (m_socket != NULL) {
    m_socket->connectToHost(m_currentServer.host(), m_currentServer.port());
  }
}


/// \brief Disconnect from IRC server
void Connection::disconnect() {
  Q_ASSERT(m_socket != NULL);
  if (m_socket != NULL) {
    m_socket->disconnectFromHost();
  }
}


/// \brief Access current server for this connection
ServerInfo Connection::server() const {
  return m_currentServer;
}


/// \brief Set current server for this connection
///
/// \attention This method will disconnect from the current
/// server and connect to the new one if they differ.
void Connection::setServer(ServerInfo& si) {
  if (si != m_currentServer) {
    disconnect();
    m_currentServer.setHost(si.host());
    m_currentServer.setPort(si.port());
    connect();
  }
}


/// \brief Set current server for this connection
void Connection::setServer(QString h, quint16 p) {
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
  emit connected(m_currentServer);
}


/// \brief Slot for m_socket::disconnected()
void Connection::socket_disconnected() {
  emit disconnected(m_currentServer);
}


/// \brief Slot for m_socket::error()
///
/// This slot is connected to the error() signal of m_socket.
/// It fetches the error message belonging to err and re-emits
/// the signal to the controlling application as
/// socketError().
///
/// \param err Socket error as received from m_socket's error() signal
void Connection::socket_error(QAbstractSocket::SocketError err) {
  QString msg = m_socket->errorString();
  qWarning() << "Connection::m_socket (connected to"
	     << m_currentServer.toString() << ") generated an error: "
	     << msg;
  emit socketError(err, msg);
}


/// \brief Slot for m_socket::readyRead()
void Connection::socket_readyRead() {
  QString line;

  while (m_socket->canReadLine()) {
    line = m_socket->readLine();
    if (!line.isNull()) {
      qDebug() << line;
    }
  }
}

