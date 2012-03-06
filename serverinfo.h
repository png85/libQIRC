#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QString>

namespace QIRC {
  /// \brief Utility class to store a server address
  class ServerInfo {
  public:
    ServerInfo(QString host, unsigned short port);
    ServerInfo(const ServerInfo& o);
    
    QString host();
    void setHost(QString h);

    unsigned short port();
    void setPort(unsigned short p);

    QString toString();

  protected:
    /// \brief Hostname of the IRC server
    QString m_host;

    /// \brief Port number to connect to
    unsigned short m_port;

  };
};

#endif // !SERVERINFO_H
