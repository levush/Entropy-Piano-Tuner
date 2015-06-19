#ifndef VOLUMECONTROLGROUPBOX_H
#define VOLUMECONTROLGROUPBOX_H

#include <QHBoxLayout>

#include "displaysizedependinggroupbox.h"
#include "volumecontrollevel.h"

class VolumeControlGroupBox : public DisplaySizeDependingGroupBox
{
    Q_OBJECT
public:
    VolumeControlGroupBox(QWidget *parent);

private:
    VolumeControlLevel *mVolumeControlLevel = nullptr;
    QHBoxLayout *mLinesLayout = nullptr;
    QHBoxLayout *mTextLayout = nullptr;

signals:
    void refreshInputLevels();
    void muteToggled(bool);

public slots:
    void updateLevels(double stopLevel, double onLevel);

private slots:
    void onRefreshClicked();
    void onMuteToggled(bool);
};

#endif // VOLUMECONTROLGROUPBOX_H
