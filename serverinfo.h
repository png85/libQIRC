/// \file
/// \brief Declaration of ServerInfo utility class
///
/// \author png!das-system
#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QDebug>
#include <QString>

namespace QIRC {
  /// \brief Utility class to store a server address
  class ServerInfo {
  public:
    ServerInfo(QString host, quint16 port);
    ServerInfo(const ServerInfo& o);
    
    QString host() const;
    void setHost(QString h);

    quint16 port() const;
    void setPort(quint16 p);

    QString toString() const;

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


QDebug& operator <<(QDebug& dbg, const QIRC::ServerInfo& si);

#endif // !SERVERINFO_H
