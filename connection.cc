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
  m_desiredNick(""),
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
  m_desiredNick(""),
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
  m_desiredNick(""),
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

  // start timer for outbound message queue
  m_tMessageQueue->start();

  emit connected(m_currentServer);
}


/// \brief Slot for m_socket::disconnected()
void Connection::socket_disconnected() {
  // stop timer for outbound message queue and clear the queue
  m_tMessageQueue->stop();
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
///
/// Changes the current IRC nickname used on this connection. If we're
/// currently connected to a server, a matching NICK message is sent.
///
/// \param nick New nickname
void Connection::setNick(QString nick) {
  if (!isConnected()) {
    m_nick = nick;
  } else {
    m_desiredNick = nick;
    sendMessage("NICK " + m_desiredNick);
  }
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
void Connection::sendMessage(QString msg, bool queued) {
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

  static const QRegExp reNumeric("^:(.+) ([0-9]{3}) (.+) :(.+)$");
  if (reNumeric.exactMatch(msg)) {
    QStringList tmp = reNumeric.capturedTexts();
    QString serverName = tmp.value(1);
    int messageNumber = tmp.value(2).toInt();
    QString target = tmp.value(3);
    QString message = tmp.value(4);

    // TODO(png): handle numeric messages according to RFC1459

    return true;
  }

  static const QRegExp reNOTICE("^:(.+)!(.+)@(.+) NOTICE ([\\w#\\-]+) :(.+)$");
  if (reNOTICE.exactMatch(msg)) {
    QStringList tmp = reNOTICE.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString message = tmp.value(5);

    emit irc_notice(sender, target, message);

    return true;
  }

  static const QRegExp rePRIVMSG("^:(.+)!(.+)@(.+) PRIVMSG ([\\w#\\-]+) :(.+$)");
  if (rePRIVMSG.exactMatch(msg)) {
    QStringList tmp = rePRIVMSG.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString message = tmp.value(5);

    emit irc_privmsg(sender, target, message);

    return true;
  }

  static const QRegExp reMODE("^:(.+)!(.+)@(.+) MODE ([\\w#\\-]+) :(.+)$");
  if (reMODE.exactMatch(msg)) {
    QStringList tmp = reMODE.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString modeString = tmp.value(5);

    emit irc_mode(sender, target, modeString);

    return true;
  }

  static const QRegExp reNICK("^:(.+)!(.+)@(.+) NICK :(.+)$");
  if (reNICK.exactMatch(msg)) {
    QStringList tmp = reNICK.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString newNick = tmp.value(4);

    if (sender.nick() == m_nick && newNick == m_desiredNick) {
      // we changed our own nick
      QString oldNick = m_nick;
      m_nick = newNick;
      m_desiredNick = "";

      emit nickChanged(oldNick, newNick);
    } else {
      // another user changed their nickname
      emit irc_nick(sender, newNick);
    }

    return true;
  }

  static const QRegExp reJOIN("^:(.+)!(.+)@(.+) JOIN :(.+)$");
  if (reJOIN.exactMatch(msg)) {
    QStringList tmp = reJOIN.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString channel = tmp.value(4);

    if (sender.nick() != m_nick) {
      emit irc_join(sender, channel);
    } else {
      emit partedChannel(channel);
    }

    return true;
  }

  static const QRegExp rePART("^:(.+)!(.+)@(.+) PART :(.+)$");
  if (rePART.exactMatch(msg)) {
    QStringList tmp = rePART.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString channel = tmp.value(4);

    if (sender.nick() != m_nick) {
      emit irc_part(sender, channel);
    } else {
      emit partedChannel(channel);
    }

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

  static const QRegExp reTOPIC("^:(.+)!(.+)@(.+) TOPIC (.+) :(.+)$");
  if (reTOPIC.exactMatch(msg)) {
    QStringList tmp = reTOPIC.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString channel = tmp.value(4);
    QString newTopic = tmp.value(5);

    emit irc_topic(sender, channel, newTopic);

    return true;
  }

  static const QRegExp reINVITE("^:(.+)!(.+)@(.+) INVITE (.+) (.+)$");
  if (reINVITE.exactMatch(msg)) {
    QStringList tmp = reINVITE.capturedTexts();
    HostMask sender(tmp.value(1), tmp.value(2), tmp.value(3));
    QString target = tmp.value(4);
    QString channel = tmp.value(5);

    emit irc_invite(sender, target, channel);

    return true;
  }

  static const QRegExp
    reChannelInfo("^:(.+) 333 (.+) (.+) (.+)!(.+)@(.+) ([0-9]+)$");
  if (reChannelInfo.exactMatch(msg)) {
    QStringList tmp = reChannelInfo.capturedTexts();
    QString serverName = tmp.value(1);
    QString target = tmp.value(2);
    QString channel = tmp.value(3);
    HostMask creator(tmp.value(4), tmp.value(5), tmp.value(6));
    quint32 channelTS = tmp.value(7).toInt();

    emit irc_channelInfo(channel, creator, channelTS);

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


/// \brief Attempt to join a channel
void Connection::joinChannel(QString channel, QString key) {
  if (isConnected()) {
    if (key != "") {
      sendMessage("JOIN " + channel + " " + key);
    } else {
      sendMessage("JOIN " + channel);
    }
  } else {
    qWarning() << "Connection::joinChannel(" << channel << ") used "
	       << "while Connection instance isn't connected!";
  }
}


/// \brief Attempt to leave a channel
void Connection::partChannel(QString channel) {
  if (isConnected()) {
    sendMessage("PART " + channel);
  } else {
    qWarning() << "Connection::partChannel(" << channel << ") used "
	       << "while Connection instance isn't connected!";
  }
}


/// \brief Quit IRC
///
/// Sends a QUIT message to the IRC server and thereby disconnects
/// from it.
///
/// \param message Message text to use in QUIT command
/// \param disconnect Flag that indicates wether we want to immediately
/// disconnect after sending the QUIT command instead of waiting for
/// the server to close the connection.
void Connection::quit(QString message, bool disconnect) {
  if (isConnected()) {
    sendMessage("QUIT :" + message, false);
    if (disconnect) {
      this->disconnect();
    }
  } else {
    qWarning() << "Tried to use Connection::quit() on a Connection "
	       << "instance that isn't currently connected!";
  }
}


/// \brief Get channel topic
void Connection::getChannelTopic(QString channel) {
  if (isConnected()) {
    sendMessage("TOPIC " + channel);
  } else {
    qWarning() << "Tried to use Connection::getChannelTopic("
	       << channel << ") while connection instance isn't connected!";
  }
}


/// \brief Change channel topic
void Connection::setChannelTopic(QString channel, QString topic) {
  if (isConnected()) {
    sendMessage("TOPIC " + channel + " :" + topic);
  } else {
    qWarning() << "Tried to use Connection::setChannelTopic("
	       << channel << "," << topic << ") while Connection instance "
	       << "isn't connected!";
  }
}


/// \brief Get list of users on a channel
void Connection::getChannelNames(QString channel) {
  if (isConnected()) {
    sendMessage("NAMES " + channel);
  } else {
    qWarning() << "Tried to use Connection::getChannelMembers("
	       << channel << ") while connection instance isn't connected!";
  }
}

