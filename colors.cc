#include <QRegExp>

#include "qirc.h"

/// \brief Remove mIRC color codes from message
QString QIRC::stripColors(QString s) {
  QRegExp reColorCodes("\\x03(?:\\d{1,2}(?:,\\d{1,2})?)?");
  s = s.replace(reColorCodes, "");
  return s;
}

/// \brief Remove all formatting codes from message
QString QIRC::stripFormat(QString s) {
  // remove \x02 (bold)
  s = s.replace("\x02", "");

  // remove all color codes
  s = QIRC::stripColors(s);

  return s;
}
