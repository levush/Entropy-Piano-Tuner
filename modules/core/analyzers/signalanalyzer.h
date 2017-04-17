/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

//=============================================================================
//                               Signal analyzer
//=============================================================================

#ifndef SIGNALANALYZER_H
#define SIGNALANALYZER_H

#include <fftw3.h>

#include "prerequisites.h"

#include "system/simplethreadhandler.h"
#include "messages/messagelistener.h"
#include "audio/circularbuffer.h"
#include "math/fftimplementation.h"

#include "fftanalyzer.h"
#include "keyrecognizer.h"
#include "overpull.h"

class AudioRecorder;

///////////////////////////////////////////////////////////////////////////////
/// \brief Signal analyzer: Fourier transformation of the recorded audio signal
///
/// The signal analyzer receives the audio signal from the audio device.
/// It contains another cyclic buffer which can hold audio data of
/// about a minute. After detecting a keystroke, the AudioRecorderAdapter
/// sends a message to start the SignalAnalyzer. During recording the
/// SignalAnalyzer continuously performs Fourer transformations on the
/// current signal. When the recording is finished, a final Fourier
/// transformation is carried out. Various steps for signal preprocessing
/// are included as well.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN SignalAnalyzer :
        public SimpleThreadHandler,
        public MessageListener,
        private KeyRecognizerCallback
{
public:
    static const int AUDIO_BUFFER_SIZE_IN_SECONDS = 60;             ///< Maximal size of the audio buffer
    static const int MINIMAL_FFT_INTERVAL_IN_MILLISECONDS = 150;    ///< Time interval for at most one FFT

private:

    enum AnalyzerRole
    {
        ROLE_IDLE,                  ///< Idle
        ROLE_RECORD_KEYSTROKE,      ///< Recording a key stroke in recording operation mode
        ROLE_ROLLING_FFT,           ///< Performing rolling ffts in tuning mode
    };

public:
    SignalAnalyzer(AudioRecorder *recorder);
    ~SignalAnalyzer() {}

    void init();
    void exit() {}

    virtual void stop() override;

private:
    void handleMessage (MessagePtr m) override final;               // Message receiver

    void changeRole(AnalyzerRole role);
    void updateDataBufferSize();

    void workerFunction() override final;                           // Thread execution function

    void recordSignal();
    void analyzeSignal();
    void recordPostprocessing();                                    // processing after recording finished
    void updateOverpull();

    double signalPreprocessing(FFTWVector &signal);                 // Preprocessing of incoming signal
    void signalProcessing(FFTWVector &signal, int samplingrate);    // processing of the current data
    bool detectClipping(FFTWVector signal);                         // Clipping detector, not yet implemented
    void PerformFFT (FFTWVector &signal, FFTWVector &powerspec);    // Perform fast Fourier transformation
    void createPolygon (const FFTWVector &powerspec, FFTPolygon &poly) const;   // Create polygon for drawing

    int identifySelectedKey();              ///< identify final key

    // callbacks
    virtual void keyRecognized(int keyIndex, double frequency) override final;


//    void WriteSignal (std::string filename, const FFTWVector &signal);    // Development, will be removed
//    void WriteFFT (std::string filename, const FFTWVector &fft);          // Development, will be removed

private:
    const Piano *mPiano;                    ///< Pointer to the piano
    CircularBuffer<FFTWType> mDataBuffer;   ///< Local audio buffer
    std::mutex mDataBufferMutex;            ///< The data buffer might change its size during recording and key selection, lock it
    AudioRecorder *mAudioRecorder;          ///< Pointer to the audio recorder
    std::atomic<bool> mRecording;           ///< Flag indicating ongoing recording
    FFTWVector mProprocessedSignal;         ///< the current signal (after preprocessing)
    FFTDataPointer mPowerspectrum;          ///< the last recorded powerspectrum

    FFT_Implementation mFFT;                ///< Instance of the Fourier transformer

    FFTAnalyzer mFFTAnalyser;               ///< Instance of the FFT analyzer
    KeyRecognizer mKeyRecognizer;           ///< Instance of the Key recognizer
    OverpullEstimator mOverpull;            ///< Instance of the overpull estimator
    std::map<int,int> mKeyCountStatistics;  ///< Count which key is selected how often
    std::mutex mKeyCountStatisticsMutex;    ///< Corresponding mutex
    int mSelectedKey;                       ///< The selected key by the user
    bool mKeyForced;                        ///< Is the key selection forced
    int mInvalidRecoringCounter = 0;        ///< Number of recordings that failed in the current key

    std::atomic<AnalyzerRole> mAnalyzerRole;
};

#endif // SIGNALANALYZER_H
