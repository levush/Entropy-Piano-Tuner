#include "tuninggroupbox.h"
#include <QHBoxLayout>

#include "zoomedspectrumgraphicsview.h"

TuningGroupBox::TuningGroupBox(QWidget *parent) :
    DisplaySizeDependingGroupBox(parent, new QHBoxLayout, toFlag(MODE_TUNING))
{
    setTitle(tr("Tuning"));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(mMainWidgetContainer->layout());

    ZoomedSpectrumGraphicsView *graph = new ZoomedSpectrumGraphicsView(this);
    mainLayout->addWidget(graph);
}

