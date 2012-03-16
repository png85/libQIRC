#include <QDebug>
#include "connectiontest.h"

ConnectionTest::ConnectionTest(QObject* parent) : QObject(parent),
						  m_connection(NULL) {
}

void ConnectionTest::conn_connected(QIRC::ServerInfo si) {
  qDebug() << "Connected to" << si;
  m_connection->setNick("TEST234");
}

void ConnectionTest::conn_disconnected(QIRC::ServerInfo si) {
  qDebug() << "Disconnected from" << si;
  emit finished();
}

void ConnectionTest::run() {
  try {
    QIRC::ServerInfo si("172.16.1.129", 6667);
    m_connection = new QIRC::Connection(si);
  }

  catch (std::bad_alloc& ex) {
    qCritical() << "Unable to allocate memory for m_connection:"
		<< ex.what();
    exit(1);
  }

  QObject::connect(m_connection, SIGNAL(connected(QIRC::ServerInfo)),
		   this, SLOT(conn_connected(QIRC::ServerInfo)));
  QObject::connect(m_connection, SIGNAL(disconnected(QIRC::ServerInfo)),
		   this, SLOT(conn_disconnected(QIRC::ServerInfo)));

  m_connection->connect();
}
