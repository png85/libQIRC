#include "../Connection"
#include "connectiontest.h"
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char** argv) {
  QCoreApplication a(argc, argv);
  ConnectionTest* ct = NULL;

  try {
    ct = new ConnectionTest();
  }

  catch (std::bad_alloc& ex) {
    qCritical() << "Unable to allocate memory for ConnectionTest instance: "
		<< ex.what();
    return EXIT_FAILURE;
  }

  QObject::connect(ct, SIGNAL(finished()), &a, SLOT(quit()));
  QTimer::singleShot(0, ct, SLOT(run()));

  int r = a.exec();

  delete ct;

  return r;
}
