#ifndef CONNECTION_H
#define CONNECTION_H 1

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QStringList>

#include "ServerInfo"
#include "HostMask"

namespace QIRC {
  /// \brief Connection to an IRC server
  ///
  /// This class maintains a connection to an IRC server.
  class Connection : public QObject {
    Q_OBJECT
  public:
    Connection();
    Connection(const ServerInfo& si);
    Connection(QString h, quint16 p);
    virtual ~Connection();

    ServerInfo server() const;
    void setServer(const ServerInfo& si);
    void setServer(QString h, quint16 p);

    QString serverPassword() const;
    void setServerPassword(QString password);

    QString ident() const;
    void setIdent(QString ident);

    QString nick() const ;
    void setNick(QString nick);

    QString realName() const;
    void setRealName(QString realName);

    void connect();
    void disconnect();
    bool isConnected() const;

    void joinChannel(QString channel);
    void partChannel(QString channel);

    void quit(QString message, bool disconnect=true);

  protected:
    /// \brief ServerInfo for the currently connected server
    ServerInfo m_currentServer;

    /// \brief TCP socket for connection to server
    QTcpSocket* m_socket;

    /// \brief Flag indicating wether we're currently connected
    bool m_connected;

    /// \brief Server password
    QString m_serverPassword;

    /// \brief ident username
    QString m_ident;

    /// \brief Current nickname
    QString m_nick;

    /// \brief Desired nickname for name changes
    QString m_desiredNick;

    /// \brief Real name to use on IRC
    QString m_realName;

    /// \brief Queue for outgoing IRC messages
    QStringList m_messageQueue;

    /// \brief Timer to send queued messages
    QTimer* m_tMessageQueue;

    void sendMessage(QString msg, bool queued=true);
    bool parseMessage(QString msg);
    void authenticate();

    void sendPong(QString serverName);

  protected slots:
    void socket_connected();
    void socket_disconnected();
    void socket_error(QAbstractSocket::SocketError);
    void socket_readyRead();
    void timer_messageQueue();

  signals:
    /// \brief TCP/IP socket error
    ///
    /// This signal gets emitted when the underlying socket for the
    /// connection to the IRC server has generated an error.
    ///
    /// \param err Error as received from the socket
    /// \param msg Error message in human-readable form
    void socketError(QAbstractSocket::SocketError err, QString msg);


    /// \brief Connection to IRC server established
    ///
    /// \param si ServerInfo object describing the server to which the
    /// connection was established
    void connected(QIRC::ServerInfo si);


    /// \brief Connection to IRC server dropped
    ///
    /// \param si ServerInfo object with the server that got disconnected
    void disconnected(QIRC::ServerInfo si);

    /// \brief Got IRC PING message
    ///
    /// This signal gets emitted whenever we receive a PING message from
    /// the IRC server.
    /// The PONG response to this message is sent automatically.
    ///
    /// \param serverName Server name as sent by the IRC server
    void irc_ping(QString serverName);

    /// \brief Got NOTICE AUTH message
    ///
    /// This signal gets emitted whenever we receive a NOTICE AUTH message/
    /// from the server.
    ///
    /// \param serverName Server name as sent by the server
    /// \param message Message contents as string
    void irc_notice_auth(QString serverName, QString message);

    /// \brief Got NOTICE message
    ///
    /// This signal gets emitted whenever we receive a NOTICE message.
    /// A NOTICE can be either sent to a single client or a whole channel
    /// which is indicated by the 'target' parameter.
    ///
    /// \param sender Host mask of the NOTICEs sender
    /// \param target Nick or channel name that the notice was sent to
    /// \param message Message text as string
    void irc_notice(QIRC::HostMask sender, QString target, QString message);


    /// \brief Got PRIVMSG
    ///
    /// This signal gets emitted whenever we receive a PRIVMSG line from the
    /// IRC server. This can be either a private message to the client itself
    /// or to a channel.
    ///
    /// \param sender Host mask of the PRIVMSGs sender
    /// \param target Nick or channel that the message was directed at
    /// \param message Message text as string
    void irc_privmsg(QIRC::HostMask sender, QString target, QString message);


    void irc_mode(QIRC::HostMask sender, QString target, QString modeString);

    /// \brief Successfully changed nickname
    ///
    /// This signal is emitted whenever we receive a NICK message that
    /// indicates that we changed our own nickname.
    ///
    /// \param oldNick old nickname as string
    /// \param newNick new nickname as string
    void nickChanged(QString oldNick, QString newNick);

    /// \brief Nickname change
    ///
    /// This signal is emitted whenever another user on IRC changes their
    /// nickname.
    ///
    /// \param sender Host mask of the user that changed their nick
    /// \param newNick New nickname of the user as string
    void irc_nick(QIRC::HostMask sender, QString newNick);

  private:
    bool setupSocket();
    bool setupMessageQueue();

  };
};

#endif
