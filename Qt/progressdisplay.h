#ifndef PROGRESSDISPLAY_H
#define PROGRESSDISPLAY_H

#include <QFrame>
#include <QLabel>
#include <QProgressBar>

#include "core/audio/player/waveformgenerator.h"

class ProgressDisplay :
        public QFrame,
        public WaveformGeneratorStatusCallback
{
    Q_OBJECT
public:
    ProgressDisplay(QWidget *mainWindow);

private:
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

    void queueSizeChanged(size_t size, size_t total_size);

signals:
    void wgPercentageChanged(int percentage);

public slots:
    void updatePosition();

private slots:
    void updateWGPercentage(int percentage);

private:
    QLabel *mWaveformGeneratorStatusLabel;
    QProgressBar *mWaveformGeneratorStatusBar;
};

#endif // PROGRESSDISPLAY_H
