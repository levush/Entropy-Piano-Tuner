#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QUrl>
#include <QHBoxLayout>
#include "qtconfig.h"

#if CONFIG_ENABLE_UPDATE_TOOL
#include <QNetworkReply>
#endif

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent, QString iconPostfix);

private:
    void addUpdateAvailableButton();

private:
    QHBoxLayout *mTitleBarLayout;

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot called by the about dialog when clicked a url.
    /// \param url : The url to handle.
    ///
    ///  If usually will open a web browser if url is a web link.
    ///////////////////////////////////////////////////////////////////////////////
    void onOpenAboutUrl(QUrl url);

    void onUpdate();
#if CONFIG_ENABLE_UPDATE_TOOL
    void onNetworkReply(QNetworkReply *reply);
#endif
};

#endif // ABOUTDIALOG_H
