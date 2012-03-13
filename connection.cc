#include "Connection"

using namespace QIRC;

/// \brief Construct without server information
Connection::Connection() :
  m_currentServer("127.0.0.1", 6667), m_socket(NULL),
  m_serverPassword(""),
  m_ident("QIRC"), m_nick("QIRC"), m_realName("QIRC") {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }
}


/// \brief Construct from given ServerInfo
Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL),
  m_serverPassword(""),
  m_ident("QIRC"), m_nick("QIRC"), m_realName("QIRC") {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }
}


/// \brief Construct from host/port
Connection::Connection(QString h, quint16 p) :
  m_currentServer(h, p), m_socket(NULL),
  m_serverPassword(""),
  m_ident("QIRC"), m_nick ("QIRC"), m_realName("QIRC") {
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
  m_socket->connectToHost(m_currentServer.host(), m_currentServer.port());
}


/// \brief Disconnect from IRC server
void Connection::disconnect() {
  m_socket->disconnectFromHost();
}


/// \brief Access current server for this connection
ServerInfo Connection::server() const {
  return m_currentServer;
}


/// \brief Set current server for this connection
///
/// \attention This method will disconnect from the current
/// server and connect to the new one if they differ.
void Connection::setServer(const ServerInfo& si) {
  if (si != m_currentServer) {
    disconnect();
    m_currentServer = si;
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
  // authenticate to server if we have a password set for
  // the connection
  authenticate();

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


void Connection::setServerPassword(QString password) {
  m_serverPassword = password;
}


QString Connection::serverPassword() const {
  return m_serverPassword;
}


void Connection::setIdent(QString ident) {
  m_ident = ident;
}


QString Connection::ident() const {
  return m_ident;
}


void Connection::setNick(QString nick) {
  m_nick = nick;
}


QString Connection::nick() const {
  return m_nick;
}


void Connection::setRealName(QString realName) {
  m_realName = realName;
}


QString Connection::realName() const {
  return m_realName;
}


/// \brief Send message to server
void Connection::sendMessage(QString msg) {
  QTextStream s(m_socket);
  s << msg.trimmed() << "\n";
}


/// \brief Authenticate connection
///
/// Authenticates to the IRC server by sending an USER message.
///
/// If m_serverPassword is set this method will send a PASS
/// message to the server to authenticate with a password.
void Connection::authenticate() {
  if (m_serverPassword.length() > 0) {
    sendMessage("PASS " + m_serverPassword);
  }

  // USER username hostname servername: realname
  sendMessage("USER " + m_ident + " " + m_currentServer.host() + " "
	      + m_currentServer.host() + ": " + m_realName);
}

