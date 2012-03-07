/// \file
/// \brief Declaration of HostMask utility class
///
/// \author png!das-system
#ifndef HOSTMASK_H
#define HOSTMASK_H 1

#include <QString>

namespace QIRC {
  /// \brief Utility class to store an user hostmask
  class HostMask {
  public:
    HostMask(QString nick, QString user, QString host);
    HostMask(const HostMask& other);

    QString nick();
    void setNick(QString n);

    QString user();
    void setUser(QString u);

    QString host();
    void setHost(QString h);

    QString toString();

  protected:
    /// \brief Nickname part of hostmask
    QString m_nick;

    /// \brief Username part of hostmask
    QString m_user;

    /// \brief Hostname part of hostmask
    QString m_host;
  };
};

#endif // !HOSTMASK_H
