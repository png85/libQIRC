#ifndef CONNECTIONTEST_H
#define CONNECTIONTEST_H 1

#include "../Connection"
#include <QObject>

class ConnectionTest : public QObject {
  Q_OBJECT
 public:
  ConnectionTest(QObject* parent=0);

 public slots:
  void run();

 protected:
  QIRC::Connection* m_connection;

  protected slots:
  void conn_connected(QIRC::ServerInfo si);
  void conn_disconnected(QIRC::ServerInfo si);
  void conn_privmsg(QIRC::HostMask sender, QString target, QString message);

 signals:
  void finished();
};

#endif // !CONNECTIONTEST_H
