#include "displaysizedependinggroupbox.h"
#include "displaysize.h"

DisplaySizeDependingGroupBox::DisplaySizeDependingGroupBox(QWidget *parent, QLayout *mainLayout, int operationModeFlags) :
    QWidget(parent),
    mOperationModeFlags(operationModeFlags)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    mMainWidgetContainer = this;

    if (DisplaySizeDefines::getSingleton()->getMainGroupBoxDisplayStyle() == MGBDS_NORMAL) {
        // on normal displays draw a group box around the data
        QVBoxLayout *dummyLayout = new QVBoxLayout;
        this->setLayout(dummyLayout);
        dummyLayout->setMargin(0);
        mGroupBox = new QGroupBox;
        dummyLayout->addWidget(mGroupBox);
        mMainWidgetContainer = mGroupBox;
    } else {
        mainLayout->setMargin(0);
    }

    mMainWidgetContainer->setLayout(mainLayout);
}

void DisplaySizeDependingGroupBox::setTitle(QString title) {
    if (mGroupBox) {
        mGroupBox->setTitle(title);
    }
}

void DisplaySizeDependingGroupBox::onModeChanged(OperationMode mode) {
    setVisible((mOperationModeFlags & toFlag(mode)) > 0);
}

