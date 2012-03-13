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
    Connection(const ServerInfo& si);
    Connection(QString h, unsigned short p);
    virtual ~Connection();

    ServerInfo server() const;
    void setServer(ServerInfo& si);
    void setServer(QString h, unsigned short p);

    void connect();
    void disconnect();

  protected:
    /// \brief ServerInfo for the currently connected server
    ServerInfo m_currentServer;

    /// \brief TCP socket for connection to server
    QTcpSocket* m_socket;

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

  private:
    bool setupSocket();
  };
};

#endif
