#ifndef QIRC_H
#define QIRC_H 1

#include <QString>

#if defined(_WIN32)
#if defined(QIRC_EXPORTS)
#define QIRC_EXPORT __declspec(dllexport)
#else
#define QIRC_EXPORT __declspec(dllimport)
#endif // !QIRC_EXPORTS
#else
#define QIRC_EXPORT
#endif // !_WIN32

namespace QIRC {
  QString QIRC_EXPORT stripFormat(QString s);
  QString QIRC_EXPORT stripColors(QString s);
};

#endif // !QIRC_H
