/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef VERSIONCHECK_H
#define VERSIONCHECK_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "prerequisites.h"

struct VersionInformation {
    QString mAppVersion;
    QString mDepsVersion;
};

class VersionCheck : public QNetworkAccessManager
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
