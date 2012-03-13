/// \file
/// \brief Declaration of ServerInfo utility class
///
/// \author png!das-system
#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QString>

namespace QIRC {
  /// \brief Utility class to store a server address
  class ServerInfo {
  public:
    ServerInfo(QString host, quint16 port);
    ServerInfo(const ServerInfo& o);
    
    QString host();
    void setHost(QString h);

    quint16 port();
    void setPort(quint16 p);

    QString toString();

    ServerInfo& operator =(const ServerInfo& o);
    bool operator ==(const ServerInfo& o) const;
    bool operator !=(const ServerInfo& o) const;

  protected:
    /// \brief Hostname of the IRC server
    QString m_host;

    /// \brief Port number to connect to
    quint16 m_port;

  };
};

#endif // !SERVERINFO_H
