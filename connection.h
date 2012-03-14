#ifndef CONNECTION_H
#define CONNECTION_H 1

#include <QObject>
#include <QTcpSocket>
#include "ServerInfo"

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

    void sendMessage(QString msg);
    bool parseMessage(QString msg);
    void authenticate();

  protected slots:
    void socket_connected();
    void socket_disconnected();
    void socket_error(QAbstractSocket::SocketError);
    void socket_readyRead();

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


  private:
    bool setupSocket();
  };
};

#endif
