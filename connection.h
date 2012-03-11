#ifndef CONNECTION_H
#define CONNECTION_H 1

#include <QObject>
#include <QTcpSocket>
#include "ServerInfo"

namespace QIRC {
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

  private:
    bool setupSocket();
  };
};

#endif
