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

#include "platformtools.h"
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "tunerapplication.h"

bool PlatformTools::loadStartupFile(const QStringList args) {
    return openFileFromArgs(args);
}

bool PlatformTools::openFileFromArgs(const QStringList &args) {
    QString startupFile;
    bool cached = false;

    // get startup file from the arguments
    if (args.size() > 1) {
        // first argument is program name, second is path to file, maybe with whitespaces so combine all
        QStringList pathParts(args);
        pathParts.pop_front();  // program name
        startupFile = pathParts.join(" ");
        // search for ' ' that should keep the filename together
        startupFile = startupFile.left(startupFile.lastIndexOf("'"));
        startupFile = startupFile.right(startupFile.length() - startupFile.indexOf("'") - 1);
        QFileInfo f(startupFile);
        if (f.fileName() == startupFile) {
            // search in documents folder by default
            QDir docDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
            startupFile = docDir.absoluteFilePath(f.fileName());
        }
    }
    if (startupFile.isEmpty()) {
        // empty file cannot be opened
        return false;
    }
    return TunerApplication::getSingleton().openFile(startupFile, cached);
}

void PlatformTools::openFile(const char *file, bool cached) {
    QString fileName(file);
    LogI("Opening external file %s", file);
    QMetaObject::invokeMethod(TunerApplication::getSingletonPtr(),
                              "openFile",
                              Qt::QueuedConnection,
                              QGenericReturnArgument(0),
                              Q_ARG(QString, fileName),
                              Q_ARG(bool, cached));
}
