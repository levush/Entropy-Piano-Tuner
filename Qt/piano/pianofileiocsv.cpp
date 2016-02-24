#include "pianofileiocsv.h"
#include <QTextStream>
#include <cmath>

#include "core/system/eptexception.h"

void PianoFileIOCsv::read(QIODevice *device, Piano &piano) {
    Q_UNUSED(device);
    Q_UNUSED(piano);
    Q_UNIMPLEMENTED();
}

void PianoFileIOCsv::write(QIODevice *device, const Piano &piano) const {
    QTextStream stream(device);
    if (stream.status() != QTextStream::Ok) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, "Could not create QTextStream.");
    }

    stream.setFieldWidth(8);
    stream.setFieldAlignment(QTextStream::AlignRight);

    // only write
    // key index + 1, inharmonicity, recorded frequency, computedAA_AttributeCount frequency, tuned frequency, quality

    // header
    stream << "\"Key index\",\"Inharmonicity\",\"Recorded frequency\",\"Recorded deviation\",\"Computed frequency\",\"Computed deviation\",\"Tuned frequency\",\"Tuned deviation\",\"Quality\"" << endl;

    // data
    const int A4 = piano.getKeyboard().getKeyNumberOfA4();
    const Key &Akey = piano.getKey(A4);
    auto cents = [A4] (int i, double f, double fA4)
    {
        double ET = std::pow(2.0,(i-A4)/12.0);
        if (f == 0 or fA4 == 0) return 0.0;
        else return 1200.0 * std::log(f/fA4/ET)/std::log(2.0);
    };
    for (int i = 0; i < piano.getKeyboard().getNumberOfKeys(); ++i) {
        const Key &key = piano.getKeyboard()[i];
        stream << QString("%1,").arg(i + 1)
               << QString("%1,").arg(key.getMeasuredInharmonicity(), 8, 'g', 5)
               << QString("%1,").arg(key.getRecordedFrequency(), 8, 'g', 5)
               << QString("%1,").arg(cents(i,key.getRecordedFrequency(),Akey.getRecordedFrequency()), 6, 'g', 5)
               << QString("%1,").arg(key.getComputedFrequency(), 8, 'g', 5)
               << QString("%1,").arg(cents(i,key.getComputedFrequency(),Akey.getComputedFrequency()), 6, 'g', 5)
               << QString("%1,").arg(key.getTunedFrequency(), 8, 'g', 5)
               << QString("%1,").arg(cents(i,key.getTunedFrequency(),piano.getConcertPitch()), 6, 'g', 5)
               << QString("%1,").arg(key.getRecognitionQuality(), 6, 'g', 5)
               << endl;
    }

    if (stream.status() == QTextStream::WriteFailed) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, "Could not write to QTextStream when writing a csv file.");
    }
}
