#ifndef PAGESAVINGTABWIDGET_H
#define PAGESAVINGTABWIDGET_H

#include <QTabWidget>

///
/// \brief QTabWidget that stores the last visible page
///
/// Inherit and call restorePageFromSettings() as the last
/// command in the parent constructor after creating all tabs
///
/// It will use as QSettings path "settings/" + mID + "/page"
///
class PageSavingTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ///
    /// \brief Constructor
    /// \param id The id of the widget
    ///
    PageSavingTabWidget(QString id);

protected slots:

    ///
    /// \brief Store the given page to the settings
    /// \param index page to store
    ///
    /// Called when the current page changed
    ///
    void storePageToSettings(int index);

    ///
    /// \brief Call this once to restore the settings
    ///
    /// It will also connect currentIndexChanged(int) to
    /// storePageToSettings(index)
    ///
    void restorePageFromSettings();

private:
    /// The id for the page used in QSettings
    const QString mID;
};

#endif // PAGESAVINGTABWIDGET_H
