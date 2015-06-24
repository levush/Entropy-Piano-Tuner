#include "volumecontrolgroupbox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLine>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QShortcut>
#include <QKeySequence>

#include "displaysize.h"

VolumeControlGroupBox::VolumeControlGroupBox(QWidget *parent) :
    DisplaySizeDependingGroupBox(parent, new QVBoxLayout,
                                 toFlag(MODE_IDLE) | toFlag(MODE_RECORDING)
                                 | ((DisplaySizeDefines::getSingleton()->showVolumeGroupBoxInTuningMode()) ? toFlag(MODE_TUNING) : 0))
{
    setTitle(tr("Volume control"));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(mMainWidgetContainer->layout());
    mainLayout->setSpacing(0);

    mVolumeControlLevel = new VolumeControlLevel(this);
    mVolumeControlLevel->setFormat(QString());
    if (!mGroupBox) {
        mVolumeControlLevel->setFormat(tr("Volume"));
    }
    mainLayout->addWidget(mVolumeControlLevel);

    QHBoxLayout *linesLayout = new QHBoxLayout;
    mLinesLayout = linesLayout;
    mainLayout->addLayout(linesLayout);

    class Line : public QFrame {
    public:
        Line() {
            setFrameShape(QFrame::VLine);
            setFrameShadow(QFrame::Plain);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            setLineWidth(3);
        }
    private:
        virtual QSize minimumSizeHint() const override final {
            return QSize(DisplaySizeDefines::getSingleton()->getSmallIconSize() / 8,
                         DisplaySizeDefines::getSingleton()->getSmallIconSize() / 4);
        }
        virtual QSize sizeHint() const override final {
            return QSize(DisplaySizeDefines::getSingleton()->getSmallIconSize() / 8,
                         DisplaySizeDefines::getSingleton()->getSmallIconSize());
        }
    };

    Line *offLine = new Line;
    Line *onLine = new Line;

    linesLayout->addStretch();
    linesLayout->addWidget(offLine);
    linesLayout->addStretch();
    linesLayout->addWidget(onLine);
    linesLayout->addStretch();


    QHBoxLayout *textLayout = new QHBoxLayout;
    mTextLayout = textLayout;
    mainLayout->addLayout(textLayout);
    textLayout->setMargin(0);
    textLayout->setSpacing(0);
    textLayout->addStretch();

    QLabel *offLabel = new QLabel("Off");
    offLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    textLayout->addWidget(offLabel);

    QHBoxLayout *onLayout = new QHBoxLayout;
    onLayout->setMargin(0);
    textLayout->addLayout(onLayout);

    QLabel *onLabel = new QLabel("On");
    onLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    onLayout->addWidget(onLabel);
    onLayout->addStretch();

    QToolButton *refreshButton = new QToolButton;
    onLayout->addWidget(refreshButton);
    refreshButton->setIcon(QIcon(":/media/images/refresh.png"));
    refreshButton->setIconSize(QSize(1, 1) * DisplaySizeDefines::getSingleton()->getMediumIconSize());
    QObject::connect(refreshButton, SIGNAL(clicked(bool)), this, SLOT(onRefreshClicked()));

    QToolButton *muteButton = new QToolButton;
    onLayout->addWidget(muteButton);
    muteButton->setCheckable(true);
    QIcon muteIcons;
    muteIcons.addFile(QStringLiteral(":/media/images/micro_active.png"), QSize(), QIcon::Normal, QIcon::Off);
    muteIcons.addFile(QStringLiteral(":/media/images/micro_mute.png"), QSize(), QIcon::Normal, QIcon::On);
    muteButton->setIcon(muteIcons);
    muteButton->setIconSize(QSize(1, 1) * DisplaySizeDefines::getSingleton()->getMediumIconSize());
    QObject::connect(muteButton, SIGNAL(toggled(bool)), this, SLOT(onMuteToggled(bool)));
    new QShortcut(QKeySequence(Qt::Key_M), muteButton, SLOT(toggle()));

    mainLayout->addStretch();

    // some rudementary values
    updateLevels(0.1, 0.5);



    // set what's this textsClick this button to reset the automatic calibration of the input volume.
    this->setWhatsThis(tr("This widgets provides settings and information about the input level of the input device."));
    offLabel->setWhatsThis(tr("If the input level drops below this mark the recorder stops and does not process the input signal."));
    onLabel->setWhatsThis(tr("If the input level reaches this threshold the recorder starts analyzing the signal of the input device until the level drops below the 'Off' mark."));
    refreshButton->setWhatsThis(tr("Click this button to reset the automatic calibration of the input volume."));
    muteButton->setWhatsThis(tr("Click this button to mute the input device."));
    offLine->setWhatsThis(offLabel->whatsThis());
    onLine->setWhatsThis(onLabel->whatsThis());
}

void VolumeControlGroupBox::updateLevels(double stopLevel, double onLevel) {
    int max = 10000;
    int pos0 = stopLevel * max;
    int pos1 = onLevel * max;

    mTextLayout->setStretch(0, pos0);
    mTextLayout->setStretch(1, pos1 - pos0);
    mTextLayout->setStretch(2, max - pos1);

    mLinesLayout->setStretch(0, pos0);
    mLinesLayout->setStretch(1, 0);
    mLinesLayout->setStretch(2, pos1 - pos0);
    mLinesLayout->setStretch(3, 0);
    mLinesLayout->setStretch(4, max - pos1);
}

void VolumeControlGroupBox::onRefreshClicked() {
    emit refreshInputLevels();
}

void VolumeControlGroupBox::onMuteToggled(bool b) {
    emit muteToggled(b);
}
