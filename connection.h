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

    /// \brief Real name to use on IRC
    QString m_realName;

    /// \brief Queue for outgoing IRC messages
    QStringList m_messageQueue;

    /// \brief Timer to send queued messages
    QTimer* m_tMessageQueue;

    void sendMessage(QString msg, bool queued);
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

  private:
    bool setupSocket();
    bool setupMessageQueue();

  };
};

#endif
