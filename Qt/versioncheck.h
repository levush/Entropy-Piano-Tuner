#ifndef VERSIONCHECK_H
#define VERSIONCHECK_H

#include <QObject>
#include "qtconfig.h"

#if CONFIG_ENABLE_UPDATE_TOOL
#include <QNetworkReply>
#endif

struct VersionInformation {
    QString mAppVersion;
    QString mDepsVersion;
};

class VersionCheck : public QObject
{
    Q_OBJECT
public:
    explicit VersionCheck(QObject *parent = 0);

signals:
    void updateAvailable(VersionInformation info);

private slots:
#if CONFIG_ENABLE_UPDATE_TOOL
    void onNetworkReply(QNetworkReply *reply);
#endif
};

#endif // VERSIONCHECK_H
