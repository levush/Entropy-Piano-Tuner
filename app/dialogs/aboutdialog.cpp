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

#include "aboutdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QScroller>
#include <QPushButton>
#include <QDateTime>
#include <QThread>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>

#include "core/system/eptexception.h"
#include "core/system/version.h"
#include "core/system/log.h"
#include "core/system/serverinfo.h"
#include "qtconfig.h"


AboutDialog::AboutDialog(QWidget *parent, QString iconPostfix) :
    QDialog(parent, Qt::Window)
{
    EptAssert(parent, "Parent required for size");
    setModal(true);

    QRect r(parent->geometry());
    setGeometry(r.left() + r.width() / 4, r.top() + r.height() / 4, r.width() / 2, r.height() / 2);
    setWindowTitle(tr("About"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    mTitleBarLayout = titleLayout;
    mainLayout->addLayout(titleLayout);

    titleLayout->addWidget(new QLabel(QString("<h1>%1 %2</h1>").arg(tr("Entropy Piano Tuner"), EPT_VERSION_STRING)));
    titleLayout->addStretch();

    QLabel *webpage = new QLabel(QString::fromStdString("<a href=\"" + serverinfo::SERVER_ADDRESS + "\">" + serverinfo::SERVER_NAME + "</a> - <a href=\"mailto:.org\">Feedback</a>"));
    QObject::connect(webpage, SIGNAL(linkActivated(QString)), this, SLOT(onOpenAboutLink(QString)));
    mainLayout->addWidget(webpage);

    QString iconPath = ":/media/images/icon_256x256" + iconPostfix + ".png";
    QTextBrowser *text = new QTextBrowser;
    text->setStyleSheet("background-color: transparent;");
    text->setFrameShape(QFrame::NoFrame);
    text->setOpenLinks(false);
    QObject::connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(onOpenAboutUrl(QUrl)));
    mainLayout->addWidget(text);

    QDateTime compileTime(QDateTime::fromString(__TIMESTAMP__));

    const QString buildText = tr("Built on %1").arg(compileTime.toString(Qt::DefaultLocaleLongDate));
    const QString buildByText = tr("by %1 and %2").arg("Prof. Dr. Haye Hinrichsen", "Christoph Wick M.Sc.");

    QString dependenciesText = tr("Based on");
    dependenciesText.append(" <a href=\"Qt\">Qt</a>, <a href=\"https://gitlab.com/tp3/qtmidi\">QtMidi</a>, <a href=\"http://qwt.sf.net\">Qwt</a>, <a href=\"http://fftw.org\">fftw3</a>");
#ifdef EPT_SHARED_ALGORITHMS
    dependenciesText.append(", <a href=\"http://libuv.org\">libuv</a>");
#endif
#if defined(Q_OS_IOS)
    dependenciesText.append(", <a href=\"https://github.com/petegoodliffe/PGMidi\">PgMidi</a>");
#elif defined(Q_OS_ANDROID)
    dependenciesText.append(", <a href=\"https://github.com/kshoji/USB-MIDI-Driver\">USB-Midi-Driver</a>");
#else
    dependenciesText.append(", <a href=\"http://www.music.mcgill.ca/~gary/rtmidi\">RtMidi</a>");
#endif

    const QString copyrightText = tr("Copyright %1 Dept. of Theor. Phys. III, University of Würzburg. All rights reserved.").arg(compileTime.toString("yyyy"));
    const QString licenseText = tr("This software is licensed under the terms of the %1. The source code can be accessed at %2.").
            arg("<a href=\"http://www.gnu.org/licenses/gpl-3.0-standalone.html\">GPLv3</a>",
                "<a href=\"https://gitlab.com/tp3/Entropy-Piano-Tuner\">GitLab</a>");

    const QString warrantyText = tr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");

    const QString acknowledgementsText = tr("We thank all those who have contributed to the project:") +
           " Prof. Dr. S. R. Dahmen, A. Frick, A. Heilrath, M. Jiminez, Prof. Dr. W. Kinzel, M. Kohl, L. Kusmierz, Prof. Dr. A. C. Lehmann, B. Olbrich., Dr. J. Um, Zhou Ying, Xavier Monnin";

    auto makeParagraphTags = [](const QString &t) {return "<p>" + t + "</p>";};
    QString completeText;
    completeText.append("<html><img src=\"" + iconPath + "\" style=\"float: left;\"/>");

    completeText.append(makeParagraphTags(buildText));
    completeText.append(makeParagraphTags(buildByText));
    completeText.append(makeParagraphTags(dependenciesText));
    completeText.append(makeParagraphTags(copyrightText));
    completeText.append(makeParagraphTags(licenseText));
    completeText.append(makeParagraphTags(warrantyText));
    completeText.append(makeParagraphTags(acknowledgementsText));

    completeText.append("</html>");

    text->setHtml(completeText);

    QHBoxLayout *okButtonLayout = new QHBoxLayout;
    okButtonLayout->setMargin(0);
    mainLayout->addLayout(okButtonLayout);
    okButtonLayout->addStretch();

    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButtonLayout->addWidget(okButton);

    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QScroller::grabGesture(text);
    text->setReadOnly(true);
    text->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    SHOW_DIALOG(this);

}

void AboutDialog::onOpenAboutUrl(QUrl url) {
    if (url.toString() == "Qt") {
        QMessageBox::aboutQt(this);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void AboutDialog::onOpenAboutLink(QString link) {
    QDesktopServices::openUrl(QUrl::fromUserInput(link));
}

