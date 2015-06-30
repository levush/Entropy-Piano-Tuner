#ifndef DISPLAYSIZEDEPENDINGGROUPBOX_H
#define DISPLAYSIZEDEPENDINGGROUPBOX_H

#include <QGroupBox>
#include <QLayout>
#include "core/system/prerequisites.h"

class DisplaySizeDependingGroupBox : public QWidget
{
    Q_OBJECT
public:
    DisplaySizeDependingGroupBox(QWidget *parent, QLayout *mainLayout, int operationModeFlags);

    void setTitle(QString title);


protected:
    QGroupBox *mGroupBox = nullptr;

    QWidget *mMainWidgetContainer = nullptr;

private:
    const int mOperationModeFlags = 0;

protected slots:
    virtual void onModeChanged(OperationMode mode);
};

#endif // DISPLAYSIZEDEPENDINGGROUPBOX_H
