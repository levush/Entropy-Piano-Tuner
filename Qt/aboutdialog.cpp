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
#include <QProcess>

#include "core/system/eptexception.h"
#include "core/system/version.h"
#include "core/system/log.h"


AboutDialog::AboutDialog(QWidget *parent, QString iconPostfix) :
    QDialog(parent, Qt::Window)
{
    EptAssert(parent, "Parent required for size");

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

    QString iconPath = ":/media/images/icon_256x256" + iconPostfix + ".png";
    QTextBrowser *text = new QTextBrowser;
    text->setStyleSheet("background-color: transparent;");
    text->setFrameShape(QFrame::NoFrame);
    text->setOpenLinks(false);
    QObject::connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(onOpenAboutUrl(QUrl)));
    mainLayout->addWidget(text);

    const QString buildText = tr("Built on %1").arg(QDateTime::fromString(__TIMESTAMP__).toString(Qt::DefaultLocaleLongDate));
    const QString buildByText = tr("by %1 and %2").arg("Prof. Dr. Haye Hinrichsen", "Christoph Wick M.Sc.");

    QString dependenciesText = tr("Based on");
    dependenciesText.append(" <a href=\"Qt\">Qt</a>, <a href=\"http://fftw.org\">fftw3</a>");
    dependenciesText.append(", <a href=\"http://www.grinninglizard.com/tinyxml2\">tinyxml2</a>");
    dependenciesText.append(", <a href=\"http://www.music.mcgill.ca/~gary/rtmidi\">RtMidi</a>");
#ifdef __ANDROID__
    dependenciesText.append(", <a href=\"https://github.com/kshoji/USB-MIDI-Driver\">USB-Midi-Driver</a>");
#endif

    const QString copyrightText = tr("Copyright 2015 Dept. of Theor. Phys. III, University of Würzburg. All rights reserved.");
    const QString licenseText = tr("This software is licensed unter the terms of the %1. The source code can be accessed at %2.").
            arg("<a href=\"http://www.gnu.org/licenses/gpl-3.0-standalone.html\">GPLv3</a>",
                "<a href=\"https://gitlab.com/entropytuner/Entropy-Piano-Tuner\">GitLab</a>");

    const QString warrantyText = tr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");

    const QString acknowledgementsText = tr("We thank all those who have contributed to the project:") +
           " Prof. Dr. S. R. Dahmen, A. Frick, A. Heilrath, M. Jiminez, Prof. Dr. W. Kinzel, M. Kohl, L. Kusmierz, Prof. Dr. A. C. Lehmann, B. Olbrich.";

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


#if CONFIG_ENABLE_UPDATE_TOOL
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QUrl versionFileUrl("http://www.physik.uni-wuerzburg.de/~hinrichsen/ept/Resources/Public/Downloads/version.info");

    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));
    nam->get(QNetworkRequest(versionFileUrl));
#endif
}

void AboutDialog::addUpdateAvailableButton() {
    QPushButton *updateButton = new QPushButton;
    updateButton->setToolTip(tr("Update available"));
    updateButton->setIcon(QIcon(":/media/icons/update-information.png"));
    updateButton->setFlat(true);
    mTitleBarLayout->addWidget(updateButton);

    QObject::connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(onUpdate()));
}

void AboutDialog::onOpenAboutUrl(QUrl url) {
    if (url.toString() == "Qt") {
        QMessageBox::aboutQt(this);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void AboutDialog::onUpdate() {
    // run maintenace tool
    if (QProcess::startDetached("maintenancetool") == false) {
        LogW("Maintenace tool could not be started.");
    }

    // let the user press the button just once
    QObject::disconnect(sender(), SIGNAL(clicked(bool)), this, SLOT(onUpdate()));
}

#if CONFIG_ENABLE_UPDATE_TOOL

void AboutDialog::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        switch(httpstatuscode) {
        case 200:  // status code ok
            if (reply->isReadable()) {
                QString replyString = QString::fromUtf8(reply->readAll().data());
                // first line contains the app version code
                // second line contains the dependencies version code
                QStringList splitted = replyString.split(QRegExp("\\s"), QString::SkipEmptyParts);
                if (splitted.size() != 2) {
                    // we dont expect this atm. Therefore there must be an update!
                    LogD("Online version file has non expected format. Requesting update.");
                    addUpdateAvailableButton();
                    return;
                } else {
                    int appVersion = splitted[0].mid(splitted[0].indexOf("=") + 1).toInt();
                    int depsVersion = splitted[1].mid(splitted[1].indexOf("=") + 1).toInt();
                    LogD("Online version is: Apps %i, Dependencies %i.", appVersion, depsVersion);
                    if (appVersion > EPT_VERSION_ROLLING || depsVersion > EPT_DEPS_VERSION_ROLLING) {
                        LogD("Update available");
                        // updates available
                        addUpdateAvailableButton();
                    }
                }
            } else {
                LogE("File not readable");
            }
        break;
        default:
            LogD("Invalid network status. Code (%i)", httpstatuscode);
            break;
        }
    } else {
        LogD("No network reply: %s.", reply->errorString().toStdString().c_str());
    }

    reply->deleteLater();
}

#endif  // CONFIG_ENABLE_UPDATE_TOOL
