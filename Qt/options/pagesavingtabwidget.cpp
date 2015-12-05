#include "pagesavingtabwidget.h"
#include <QSettings>

PageSavingTabWidget::PageSavingTabWidget(QString id) :
    mID(id)
{
}

void PageSavingTabWidget::storePageToSettings(int index)
{
    QSettings settings;
    settings.setValue("settings/" + mID + "/page", index);
}

void PageSavingTabWidget::restorePageFromSettings()
{
    QObject::connect(this, SIGNAL(currentChanged(int)), this, SLOT(storePageToSettings(int)));

    QSettings settings;
    int index = settings.value("settings/" + mID + "/page", 0).toInt();

    if (index < count()) {
        setCurrentIndex(index);
    }
}
