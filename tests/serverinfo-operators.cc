#include <cstdlib>
#include <QDebug>

#include "../ServerInfo"

using namespace QIRC;

int main(void) {
  ServerInfo a1("127.0.0.1", 6667);
  ServerInfo a2(a1);
  ServerInfo b("irc.das-system-networks.de", 6667);

  qDebug() << "a1:" << a1.toString();
  qDebug() << "a2:" << a2.toString();
  qDebug() << "b :" << b.toString();

  if (a1 != a2) {
    qDebug() << "a1 != a2: inequality operator fail!";
    return EXIT_FAILURE;
  } else {
    qDebug() << "a1 == a2: success!";
  }

  if (a1 == b) {
    qDebug() << "a1 == b: equality operator fail!";
    return EXIT_FAILURE;
  } else {
    qDebug() << "a1 != b: success!";
  }

  qDebug() << "Assigning a2 = b; a2:"
	   << a2.toString() << "; b:"
	   << b.toString();
  a2 = b;
  qDebug() << "a2 after assignment: " << a2.toString();

  if (a2 != b) {
    qDebug() << "a2 != b after assignment!";
    return EXIT_FAILURE;
  } else {
    qDebug() << "Assignment success!";
  }

  return EXIT_SUCCESS;
}
