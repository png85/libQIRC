#include <QRegExp>
#include <QStringList>

#include "HostMask"
#include "Connection"

using namespace QIRC;

/// \brief Construct without server information
Connection::Connection() :
  m_currentServer("127.0.0.1", 6667), m_socket(NULL),
  m_serverPassword(""), m_connected(false),
  m_ident("QIRC"), m_nick("QIRC"), m_realName("QIRC"),
  m_tMessageQueue(NULL) {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }

  if (!setupMessageQueue()) {
    qCritical() << "Connection: Unable to setup m_tMessageQueue!";
    exit(1);
  }

}


/// \brief Construct from given ServerInfo
Connection::Connection(const ServerInfo& si) :
  m_currentServer(si), m_socket(NULL),
  m_serverPassword(""), m_connected(false),
  m_ident("QIRC"), m_nick("QIRC"), m_realName("QIRC"),
  m_tMessageQueue(NULL) {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }

  if (!setupMessageQueue()) {
    qCritical() << "Connection: Unable to setup m_tMessageQueue!";
    exit(1);
  }

}


/// \brief Construct from host/port
Connection::Connection(QString h, quint16 p) :
  m_currentServer(h, p), m_socket(NULL),
  m_serverPassword(""), m_connected(false),
  m_ident("QIRC"), m_nick ("QIRC"), m_realName("QIRC"),
  m_tMessageQueue(NULL) {
  if (!setupSocket()) {
    qCritical() << "Connection: Unable to setup m_socket!";
    exit(1);
  }

  if (!setupMessageQueue()) {
    qCritical() << "Connection: Unable to setup m_tMessageQueue!";
    exit(1);
  }

}


Connection::~Connection() {
  if (m_tMessageQueue != NULL) {
    delete m_tMessageQueue;
  }

  if (m_socket != NULL) {
    delete m_socket;
  }
}


/// \brief Connect to IRC server
void Connection::connect() {
  if (m_connected) {
    // already connected to a server; disconnect first
    disconnect();
  }

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
    // disconnect from current server if we're connected
    bool connStatus = m_connected;
    if (connStatus) 
      disconnect();
    m_currentServer = si;

    // only reconnect if we were previously connected
    if (connStatus)
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
  m_connected = true;
  // authenticate to server if we have a password set for
  // the connection
  authenticate();

  emit connected(m_currentServer);
}


/// \brief Slot for m_socket::disconnected()
void Connection::socket_disconnected() {
  m_messageQueue.clear();
  m_connected = false;
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
  qWarning() << "Connection::m_socket (connected to" << m_currentServer
	     << ") generated an error: " << msg;
  emit socketError(err, msg);
}


/// \brief Slot for m_socket::readyRead()
void Connection::socket_readyRead() {
  QString line;

  while (m_socket->canReadLine()) {
    line = m_socket->readLine();
    if (!line.isNull()) {
      if (!parseMessage(line.trimmed())) {
	qDebug() << "Unable to parse message:" << line.trimmed();
      }
    }
  }
}


/// \brief Set server password to new value
void Connection::setServerPassword(QString password) {
  m_serverPassword = password;
}


/// \brief Return current server password
QString Connection::serverPassword() const {
  return m_serverPassword;
}


/// \brief Set ident user ID
void Connection::setIdent(QString ident) {
  m_ident = ident;
}


/// \brief Return current ident user ID
QString Connection::ident() const {
  return m_ident;
}


/// \brief Set nickname to new value
void Connection::setNick(QString nick) {
  m_nick = nick;
}


/// \brief Return current nickname
QString Connection::nick() const {
  return m_nick;
}


/// \brief Set real name to use on IRC
void Connection::setRealName(QString realName) {
  m_realName = realName;
}


/// \brief Return real name used on IRC
QString Connection::realName() const {
  return m_realName;
}


/// \brief Send message to server
///
/// Sends the given message to the IRC server. A message can be either
/// queued or sent right away. The message Queue is checked by m_tMessageQueue
/// every 10ms.
///
/// \param msg Message text
/// \param queued Flag to indicate wether the message should be queued
/// or sent right away
void Connection::sendMessage(QString msg, bool queued=true) {
  if (m_connected) {
    if (!queued) {
      QTextStream s(m_socket);
      s << msg.trimmed() << "\n";
    } else {
      m_messageQueue.append(msg.trimmed() + "\n");
    }
  } else {
    qWarning() << "Tried to use Connection::sendMessage() while "
	       << "m_connected!=true! Message was:" << msg.trimmed();
  }
}


/// \brief Authenticate connection
///
/// Authenticates to the IRC server by sending USER/NICK messages.
///
/// If m_serverPassword is set this method will also send a PASS
/// message to the server to authenticate with a password before
/// any of the others are sent.
void Connection::authenticate() {
  if (!m_connected) {
    qWarning() << "Tried to use Connection::authenticat() while "
	       << "m_connected!=true!";
  }

  if (m_serverPassword.length() > 0) {
    sendMessage("PASS " + m_serverPassword, false);
  }

  // USER username hostname servername: realname
  sendMessage("USER " + m_ident + " " + m_currentServer.host() + " "
	      + m_currentServer.host() + ": " + m_realName, false);

  // NICK nickname
  sendMessage("NICK " + m_nick, false);
}


/// \brief Are we currently connected?
bool Connection::isConnected() const {
  return m_connected;
}


/// \brief Parse incoming message
bool Connection::parseMessage(QString msg) {
  static const QRegExp reNOTICE_AUTH("^:(.+) NOTICE AUTH :(.+)$");
  if (reNOTICE_AUTH.exactMatch(msg)) {
    QStringList tmp = reNOTICE_AUTH.capturedTexts();
    QString serverName = tmp.value(1);
    QString message = tmp.value(2);

    emit irc_notice_auth(serverName, message);

    return true;
  }

  static const QRegExp reNumeric(":(.+) ([0-9]{3}) (.+) :(.+)$");
  if (reNumeric.exactMatch(msg)) {
    QStringList tmp = reNumeric.capturedTexts();
    QString serverName = tmp.value(1);
    int messageNumber = tmp.value(2).toInt();
    QString target = tmp.value(3);
    QString message = tmp.value(4);

    // TODO(png): handle numeric messages according to RFC1459

    return true;
  }

  static const QRegExp reNOTICE(":(.+)!(.+)@(.+) NOTICE ([\\w#\\-]+) :(.+)$");
  if (reNOTICE.exactMatch(msg)) {
    QStringList tmp = reNOTICE.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString message = tmp.value(5);

    emit irc_notice(sender, target, message);

    return true;
  }

  static const QRegExp reMODE(":(.+)!(.+)@(.+) MODE ([\\w#\\-]+) :(.+)$");
  if (reMODE.exactMatch(msg)) {
    QStringList tmp = reMODE.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString modeString = tmp.value(5);

    // TODO(png): emit signals for user/channel modes separately

    return true;
  }

  // PING: <servername>
  static const QRegExp rePING("^PING :(.+)$");
  if (rePING.exactMatch(msg)) {
    QString serverName = rePING.capturedTexts().value(1);

    sendPong(serverName);
    emit irc_ping(serverName);

    return true;
  }

  return false;
}


/// \brief Send PONG response to PING command
void Connection::sendPong(QString serverName) {
  sendMessage("PONG " + serverName, false);
}


/// \brief Setup queue for outbound messages
bool Connection::setupMessageQueue() {
  try {
    m_tMessageQueue = new QTimer(this);
  }

  catch (std::bad_alloc& ex) {
    qCritical() << "Unable to allocate memory for "
		<< "Connection::m_tMessageQueue:" << ex.what();
    return false;
  }

  m_tMessageQueue->setInterval(10);
  QObject::connect(m_tMessageQueue, SIGNAL(timeout()),
		   this, SLOT(timer_messageQueue()));

  return true;
}


/// \brief Slot for m_tMessageQueue::timeout()
///
/// This slot is connected to the timeout() signal of m_tMessageQueue.
/// It fetches the first message from the queue and sends it to the IRC
/// server. If no messages are queued it simply does nothing.
void Connection::timer_messageQueue() {
  if (!m_messageQueue.isEmpty()) {
    QString message = m_messageQueue.takeFirst();
    QTextStream s(m_socket);
    s << message;
  }
}
