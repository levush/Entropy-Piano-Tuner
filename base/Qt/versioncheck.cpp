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

#include "versioncheck.h"

#include <QXmlStreamReader>

#include "core/system/eptexception.h"
#include "core/system/version.h"
#include "core/system/log.h"
#include "core/system/serverinfo.h"

VersionCheck::VersionCheck(QObject *parent) : QNetworkAccessManager(parent)
{
#if CONFIG_ENABLE_UPDATE_TOOL
    QUrl versionFileUrl(serverinfo::getVersionFileAddress().c_str());

    QObject::connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));
    this->get(QNetworkRequest(versionFileUrl));
#else
    deleteLater();
#endif
}


void VersionCheck::onNetworkReply(QNetworkReply *reply) {
#if CONFIG_ENABLE_UPDATE_TOOL
    if (reply->error() == QNetworkReply::NoError) {
        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        switch(httpstatuscode) {
        case 301: { // redirecting
            QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().toString();
            this->get(QNetworkRequest(redirect));
            break;
        }
        case 200:  // status code ok
            if (reply->isReadable()) {
                QString replyString = QString::fromUtf8(reply->readAll().data());

                QXmlStreamReader reader(replyString);

                bool ok = false;
                int appRolling = -1, depsRolling = -1;
                QString appVersion, depsVersion;

                // read root
                while (!reader.atEnd()) {
                  reader.readNextStartElement();
                  if (!reader.isStartElement()) {
                      continue;
                  }
                  if (reader.hasError()) {
                      LogE("Error during parsing xml-document");
                      return;
                  }

                  if (reader.name() == "app") {
                    appRolling = reader.attributes().value("rolling").toInt(&ok);
                    if (!ok) {LogE("Empty rolling element"); return;}
                    appVersion = reader.attributes().value("string").toString();
                  } else if (reader.name() == "dependencies") {
                    depsRolling = reader.attributes().value("rolling").toInt(&ok);
                    if (!ok) {LogE("Empty rolling element"); return;}
                    depsVersion = reader.attributes().value("string").toString();
                  }
                }

                // all info read
                if (appVersion.isEmpty() || depsVersion.isEmpty()) {
                    LogW("Required nodes not found. Requesting update.");
                    emit updateAvailable({"Unknown", "Unknown"});
                } else {
                    VersionInformation info = {appVersion, depsVersion};

                    if (appRolling < 0 || depsRolling < 0) {
                        LogW("Required attributes not found. Requesting update.");
                        emit updateAvailable(info);
                    } else if (appRolling > EPT_VERSION_ROLLING || depsRolling > EPT_DEPS_VERSION_ROLLING) {
                        LogD("Update available");
                        emit updateAvailable(info);
                    } else {
                        LogV("No update available.");
                    }
                }
            } else {
                LogE("File not readable");
            }
            // we did our job, delete us
            this->deleteLater();
        break;
        default:
            LogD("Invalid network status. Code (%i)", httpstatuscode);
            break;
        }
    } else {
        LogD("No network reply: %s.", reply->errorString().toStdString().c_str());
    }

    reply->deleteLater();
#else
    Q_UNUSED(reply);
#endif  // CONFIG_ENABLE_UPDATE_TOOL

}

