#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QUrl>
#include <QHBoxLayout>

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent, QString iconPostfix);

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

};

#endif // ABOUTDIALOG_H
