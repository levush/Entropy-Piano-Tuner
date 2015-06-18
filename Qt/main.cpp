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

///////////////////////////////////////////////////////////////////////////////
/// \mainpage Entropy-Piano-Tuner
/// \author Christoph Wick and Haye Hinrichsen
/// \author Institute for Physics and Astronomy<br>
/// University of Würzburg<br> 97074 Würzburg, Germany
/// \author e-mail: info at entropy-tuner.org
///
/// The Entropy-Piano-Tuner (EPT) is an experimental software for piano
/// tuning.
/// The source code of the EPT consists mainly of two parts, namely, the
/// graphical user interface, which is implemented here by using Qt,
/// and a GUI-independent part called 'core'.
//////////////////////////////////////////////////////////////////////////////

#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include "tunerapplication.h"
#include "qdebug.h"
#include "../core/config.h"
#include "../core/system/eptexception.h"
#include "settingsforqt.h"
#include "platformtools.h"
#include "runguard.h"

int main(int argc, char *argv[])
{
    RunGuard guard("entropypianotuner_runguard");
    if ( !guard.tryToRun() ) {
        // a QApplication is required for showing message boxes
        QApplication q(argc, argv);
        QMessageBox::warning(nullptr, q.tr("Application can not be started"), q.tr("The Entropy Piano Tuner could not be started because an other instance is already running."));
        return 0;
    }

    int exitCode = -1;

    // basic application properties (needed for settings)
    QCoreApplication::setOrganizationName("tp3");
    QCoreApplication::setOrganizationDomain("entropy-tuner.org");
    QCoreApplication::setApplicationName("Entropy Piano Tuner");

    try {
        // create settings
        (new SettingsForQt())->load();
        // increase run count
        SettingsForQt::getSingleton().increaseApplicationRuns();

        // create our application object
        TunerApplication a(argc, argv);

        // install language files
        QTranslator qtTranslator;

        QString localeName(QString::fromStdString(SettingsForQt::getSingleton().getLanguageId()));
        if (localeName.isEmpty()) {
            // system language
            localeName = QLocale::system().name();
        }
        // set default to be sure that this is not "C"
        QLocale::setDefault(QLocale(localeName));

        // Qt translation
        qtTranslator.load(QLocale(), "qt", "_", ":/languages/translations");
        a.installTranslator(&qtTranslator);

        // application translation
        QTranslator myappTranslator;
        myappTranslator.load(QLocale(), "piano_tuner", "_", ":/languages/translations");
        a.installTranslator(&myappTranslator);

        a.playStartupSound();

        // init all components
        a.init();

        // start the application
        a.start();

        // execute the application
        exitCode = a.exec();
    }
    catch (const EptException &e) {
        qCritical() << "Unhandled exception: ";
        qCritical() << QString::fromStdString(e.getFullDescription());
        exitCode = EXIT_FAILURE;
    }
    catch (const std::exception &e) {
        qCritical() << "Unhandled exception: ";
        qCritical() << QString::fromStdString(e.what());
        exitCode = EXIT_FAILURE;
    }
    catch (...) {
        qCritical() << "Unhandled exception: ";
        qCritical() << "unknown exception";
        exitCode = EXIT_FAILURE;
    }

    platformtools::enableScreensaver();

    return exitCode;
}
