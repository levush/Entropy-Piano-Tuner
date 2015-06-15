#ifndef VERSIONCHECK_H
#define VERSIONCHECK_H

#include <QObject>
#include "qtconfig.h"

#include <QNetworkReply>

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
    void onNetworkReply(QNetworkReply *reply);
};

#endif // VERSIONCHECK_H
