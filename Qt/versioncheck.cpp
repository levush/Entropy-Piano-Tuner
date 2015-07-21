/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "thirdparty/tinyxml2/tinyxml2.h"

#include "core/system/eptexception.h"
#include "core/system/version.h"
#include "core/system/log.h"
#include "core/system/serverinfo.h"

using tinyxml2::XMLElement;
using tinyxml2::XMLDocument;

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
                XMLDocument doc;
                doc.Parse(replyString.toStdString().c_str());
                if (doc.Error()) {
                    EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, serverinfo::VERSION_FILENAME + " file could not be parsed.");
                }

                XMLElement *root = doc.FirstChildElement();
                EptAssert(root, "Root must exist.");

                XMLElement *appsElem = root->FirstChildElement("app");
                XMLElement *depsElem = root->FirstChildElement("dependencies");

                if (!appsElem || !depsElem) {
                    LogW("Required nodes not found. Requesting update.");
                    emit updateAvailable({"Unknown", "Unknown"});
                } else {
                    int appRolling = -1, depsRolling = -1;
                    appsElem->QueryIntAttribute("rolling", &appRolling);
                    depsElem->QueryIntAttribute("rolling", &depsRolling);

                    QString appVersion("Unkown"), depsVersion("Unknown");
                    if (appsElem->Attribute("string")) {appVersion = appsElem->Attribute("string");}
                    if (depsElem->Attribute("string")) {depsVersion = depsElem->Attribute("string");}

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

