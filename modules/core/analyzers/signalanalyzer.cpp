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

#include "signalanalyzer.h"

#include "../system/log.h"
#include "../system/timer.h"
#include "../config.h"
#include "../settings.h"
#include "../messages/messagehandler.h"
#include "../messages/messageprojectfile.h"
#include "../messages/messagenewfftcalculated.h"
#include "../messages/messagefinalkey.h"
#include "../messages/messagemodechanged.h"
#include "../messages/messagepreliminarykey.h"
#include "../messages/messagekeyselectionchanged.h"
#include "../messages/messagetuningdeviation.h"
#include "../messages/messagesignalanalysis.h"
#include "../audio/recorder/audiorecorder.h"
#include "../math/mathtools.h"

#include <cmath>
#include <iostream>
#include <map>
#include <algorithm>
#include <numeric>

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of the SignalAnalyzer
/// \param recorder : pointer to the implementation of the AudioRecorder
///////////////////////////////////////////////////////////////////////////////

SignalAnalyzer::SignalAnalyzer(AudioRecorder *recorder) :
    mPiano(nullptr),
    mDataBuffer(),
    mAudioRecorder(recorder),
    mRecording(false),
    mKeyRecognizer(this),
    mSelectedKey(-1),
    mKeyForced(false),
    mAnalyzerRole(ROLE_IDLE)
{}


//-----------------------------------------------------------------------------
//                        Initialization procedure
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initializes the SignalAnalyzer and its components
///
/// This function will initialize the KeyRecognizer that could take a while if
/// the fft transform shall be optimized.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::init()
{
#if CONFIG_OPTIMIZE_FFT
    mKeyRecognizer.init(true);
#else
    mKeyRecognizer.init(false);
#endif
    mKeyCountStatistics.clear();
}


//-----------------------------------------------------------------------------
//                          Stop the signal analyzer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the signal analyzer
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::stop()
{
    mKeyRecognizer.stop();
    SimpleThreadHandler::stop();
}

//-----------------------------------------------------------------------------
//			            Message receiver and dispatcher
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message receiver and dispatcer
/// \param m : pointer to the received message
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
    case Message::MSG_PROJECT_FILE:
    {
        // stop the thread
        this->stop();
        auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
        mPiano = &mpf->getPiano();
        updateOverpull();
        break;
    }
    case Message::MSG_RECORDING_STARTED:    // start thread
        start();
        break;
    case Message::MSG_RECORDING_ENDED:
        mRecording = false;            // set a flag to terminate thread
        break;
    case Message::MSG_KEY_SELECTION_CHANGED:
    {
        auto mfkr(std::static_pointer_cast<MessageKeySelectionChanged>(m));
        mSelectedKey = mfkr->getKeyNumber();
        mKeyForced = mfkr->isForced();
        updateDataBufferSize();
        break;
    }
    case Message::MSG_MODE_CHANGED:
    {
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
        switch (mmc->getMode())
        {
        case MODE_RECORDING:
            changeRole(ROLE_RECORD_KEYSTROKE);
            break;
        case MODE_TUNING:
            changeRole(ROLE_ROLLING_FFT);
            updateOverpull();
            break;
        default:
            changeRole(ROLE_IDLE);
            break;
        }

        break;
    }
    default:
        break;
    }
}


//-----------------------------------------------------------------------------
//			            Function to handle role changing
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Changes the current role of the signal analyzer
///
/// This will stop all current actions first, and then adjust all parameters.
///
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::changeRole(AnalyzerRole role)
{
    stop();
    mAnalyzerRole = role;
    updateDataBufferSize();
}


//-----------------------------------------------------------------------------
//                         Update data buffer size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Update data buffer size
///
/// In the EPT the signal analyzer can be in different modes. In the recording
/// mode the signal is incrementally analyzed while in the tuning mode short
/// chunks are analyzed (rolling fft) in order to ensure a quick reaction of the
/// tuning indicator. Depending on that mode, different buffer sizes are
/// needed. This function adjusts the actual buffer size.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::updateDataBufferSize()
{
    std::lock_guard<std::mutex> lock(mDataBufferMutex);

    switch (mAnalyzerRole.load())
    {
        case ROLE_IDLE:
            break;
        case ROLE_RECORD_KEYSTROKE:
        {
            // Initialize the local circular buffer which holds about a minute of data
            mDataBuffer.resize(mAudioRecorder->getSampleRate() *
                               AUDIO_BUFFER_SIZE_IN_SECONDS);
            break;
        }
        case ROLE_ROLLING_FFT:
        {
            // Initialize the local circular buffer which holds 0.5...3 seconds of data
            const int globalKey = mSelectedKey + 48 - mPiano->getKeyboard().getKeyNumberOfA4();
            const double timeAtHighest = 0.5;
            const double timeAtLowest = 3;
            const double time = (timeAtHighest - timeAtLowest) * globalKey / 88 + timeAtLowest;
            mDataBuffer.resize(static_cast<size_t>(mAudioRecorder->getSampleRate() * time));
            break;
        }
    }
    mDataBuffer.clear();
}


//-----------------------------------------------------------------------------
//                      Signal analyzer thread function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Thread function of the SignalAnalyzer
///
/// This is the main function of the SignalAnalyzer which is running in an
/// independent thread. It is started by a message from the AudioRecorder when
/// the recording begins. By calling the AudioRecorder via the readAll function
/// the actual data is read and processed.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::workerFunction()
{
    setThreadName("SignalAnalyzer");
    // record the signal
    if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE or mAnalyzerRole == ROLE_ROLLING_FFT)
    {
        recordSignal();

        // Send message
        MessageHandler::send<MessageSignalAnalysis>(MessageSignalAnalysis::Status::STARTED);

        // stop the KeyRecognizer
        mKeyRecognizer.stop();

        // post process after recording
        recordPostprocessing();

        // Send message
        MessageHandler::send<MessageSignalAnalysis>(
                    MessageSignalAnalysis::Status::ENDED,
                    (mInvalidRecoringCounter == 0) ? MessageSignalAnalysis::Result::SUCCESSFULL
                                                   : MessageSignalAnalysis::Result::INVALID,
                    mInvalidRecoringCounter);
    }
}


//-----------------------------------------------------------------------------
//                               Record signal
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Record signal and perform FFT's in regular intervals
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::recordSignal()
{
    if (cancelThread()) {return;}

    mRecording = true;

    mDataBufferMutex.lock();
    mDataBuffer.clear();
    mDataBufferMutex.unlock();

    // Reset the statistics for the majority of recognized keys
    mKeyCountStatistics.clear();

    // Create a shared pointer to a vector containing the powerspectrum
    mPowerspectrum = std::make_shared<FFTData>();
    EptAssert(mPowerspectrum, "powerspectrum is accessed after while loop, be sure it is a valid pointer initially");

    // Define a timer in order to send only a limited rate of FFTs.
    Timer timer;

    // define the packed to store audio data
    AudioRecorder::PacketType packet;

    // read all data from the audio recorder to clear all buffered data
    mAudioRecorder->readAll(packet);

    // get the sampling rate
    uint samplingrate = mAudioRecorder->getSampleRate();

    // Loop that continuously reads the audio stream and performs FFTs
    while (mRecording and not cancelThread())
    {
        timer.reset();
        mAudioRecorder->readAll(packet);    // Read audio data
        if (packet.size() > 0)
        {
            // lock the data puffer if new data available during compile comutation run
            std::lock_guard<std::mutex> lock(mDataBufferMutex);

            for (auto &d : packet) mDataBuffer.push_back(d);

            if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE) {
                if (mDataBuffer.size() == mDataBuffer.maximum_size()) {
                    LogW("Audio buffer size in SignalAnalyzer reached.");
                }
            }

            // If the buffer has accumulated a certain minimum of data
            if (mDataBuffer.size() > (samplingrate * MINIMAL_FFT_INTERVAL_IN_MILLISECONDS) / 1000)
            {

                // Get audio data and make it suitable for analysis
                mProprocessedSignal = mDataBuffer.getOrderedData();

                // check if there is data in the buffer
                bool dataInBuffer = false;
                for (auto &d : mProprocessedSignal) {
                    if (d * d > 0) {
                        // this element contains data
                        dataInBuffer = true;
                        break;
                    }
                }
                if (!dataInBuffer) {// no data in buffer
                    continue;
                }

                // preprocess signal
                signalPreprocessing(mProprocessedSignal);
                CHECK_CANCEL_THREAD;

                // process signal
                signalProcessing(mProprocessedSignal, samplingrate);

                // Finally let's wait until a certain minimal time has elapsed.
                timer.waitUntil (MINIMAL_FFT_INTERVAL_IN_MILLISECONDS);
            }
        }
        timer.wait(50);
    }
    LogI("Recording complete, total FFT size = %d.",static_cast<int>(mPowerspectrum->fft.size()));
}


//-----------------------------------------------------------------------------
//                           Analyze final signal
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Analyze the final signal (in recording mode only)
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::analyzeSignal()
{
    CHECK_CANCEL_THREAD;

    //  and determine key from the key statistics
    int keynumber = identifySelectedKey();
    LogI("Detected key: %d", keynumber);

    // check the audio signal for possible
    // clipping effects and unusually long strings of zero amplitudes
    detectClipping(mDataBuffer.getOrderedData());

    CHECK_CANCEL_THREAD;

    // if the key is forced, we use the forced key number
    if (mKeyForced) {keynumber = mSelectedKey;}

    // check if a correct key was found
    if (keynumber < 0)
    {
        LogI("Final key could not be found. Cancel analysis.");
        return;
    }

    // check if found key equates the keynumber
    if (keynumber != mSelectedKey)
    {
        ++mInvalidRecoringCounter;
        LogW("Final detected key does not match the selected key. %d invalid recordings. Cancel analysis.", mInvalidRecoringCounter);
        return;
    }

    mInvalidRecoringCounter = 0;

    // If the key was successfully identified start call the FFTAnalyzer
    if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE)
    {
        // returns a pair consisting of error code and key-shared-ptr
        auto result = mFFTAnalyser.analyse(mPiano, mPowerspectrum, keynumber);

        if (result.first != FFTAnalyzerErrorTypes::ERR_NONE) // if error
            MessageHandler::send<MessageNewFFTCalculated>(result.first);
        else
        {
            // send the result
            MessageHandler::send<MessageFinalKey>(keynumber, result.second);
            MessageHandler::send<MessagePreliminaryKey>(-1,0);
        }
    }
    else if (mAnalyzerRole == ROLE_ROLLING_FFT)
    {
        std::shared_ptr<Key> key = std::make_shared<Key>(mPiano->getKey(keynumber));
        FrequencyDetectionResult result = mFFTAnalyser.detectFrequencyOfKnownKey(mPowerspectrum, mPiano, *key, keynumber);

        if (result->error != FFTAnalyzerErrorTypes::ERR_NONE) // if error
            MessageHandler::send<MessageNewFFTCalculated>(result->error);
        else
        {
            key->setTunedFrequency(result->detectedFrequency);
            MessageHandler::send<MessageFinalKey>(keynumber, key);
        }
        result->overpullInCents = key->getOverpull();
        MessageHandler::send<MessageTuningDeviation>(result);
    }
}

//-----------------------------------------------------------------------------
//               process the signal after recording has finished
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Process the singal after recording has finsihed
///
/// In the ROLE_RECORD_KEYSTROKE this will perform the actual analysis of the
/// complete recorded signal while in the ROLE_ROLLING_FFT this wont do anything.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::recordPostprocessing()
{
    CHECK_CANCEL_THREAD;

    if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE)
    {
        // do the actual fft analysis
        analyzeSignal();
        // Debug output for signals
#if CONFIG_ENABLE_XMGRACE
        std::cout << "SignalAnalyzer: Writing xmgrace files" << std::endl;

        // this is only for development and will be removed
        // do the actual fft analysis
        MessageHandler::send(Message::MSG_SIGNAL_ANALYSIS_STARTED);
        WriteSignal("1-preprocessed-signal.dat", mProprocessedSignal);
        WriteSignal("0-raw-signal.dat", mDataBuffer.getOrderedData());
        WriteFFT   ("2-final-fft.dat",mPowerspectrum.get()->fft);
#endif  // CONFIG_ENABLE_XMGRACE

        // sleep a bit to wait for the synthesizer to finish and to display the quality
        msleep(1500);
    }
    else if (mAnalyzerRole == ROLE_ROLLING_FFT) updateOverpull();
}


//-----------------------------------------------------------------------------
//                            Update overpulls
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Update overpulls
///
/// This function calls the overpull module to compute a new overpull value.
/// If the new value is different from the old one the new value is stored
/// and a message is sent to the GUI to update the corresponding marker.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::updateOverpull ()
{
    int K = mPiano->getKeyboard().getNumberOfKeys();
    for (int keynumber=0; keynumber<K; ++keynumber)
    {
        // Compute the new overpull value depending on the current tune
        double overpull = mOverpull.getOverpull(keynumber,mPiano);

        // Get the currently displayed overpull value and compute the change
        double currentoverpull = mPiano->getKey(keynumber).getOverpull();
        double change = overpull-currentoverpull;

        // If more change than 1/10 cents then
        if (fabs(change) >= 0.1 or (currentoverpull!=0 and overpull==0))
        {
            // set new overpull value
            std::shared_ptr<Key> key = std::make_shared<Key>(mPiano->getKey(keynumber));
            double existingtune = key->getTunedFrequency();
            if (existingtune>20 and currentoverpull!=0)
                key->setTunedFrequency(existingtune*pow(2,change/1200.0));
            key->setOverpull(overpull); // this informs the tuning curve window
            MessageHandler::send<MessageFinalKey>(keynumber, key);
        }
    }

}

//-----------------------------------------------------------------------------
//			                Signal preprocessing
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function for signal preprocessing.
///
/// This function performs several steps to make the input signal ready for the
/// Fourier transformation:</br>
/// 1. Cut subsonic waves
/// 2. Modify the input signal in such a way that the volume is constant.
/// 3. Fade in and out at the end of the buffer
/// \param signal : real-valued vector with PCM data
/// \return Average decay time of the envelope, serving as a rough estimate
/// whether a very low or a very high key has been hit.
///////////////////////////////////////////////////////////////////////////////

double SignalAnalyzer::signalPreprocessing(FFTWVector &signal)
{
    if (signal.size() == 0) {
        LogW("Empty signal. Cancelling the signal preprocessing");
        return 0 ;
    }

    const uint sr = mAudioRecorder->getSampleRate();
    uint N=(uint)signal.size();

    // 1. Remove dc-Bias and cut subsonic waves
    // For a derivation see Mathematica file in the doc folder
    const double dcBias = std::accumulate(signal.begin(), signal.end(), 0.0) / N;
    //for (auto &s : signal) {s -= dcBias;}

    const double f0 = 5;            // Frequency to be suppressed by 50%
    const double a=10.8828*f0/sr;   // Damping factor
    double follow=0;
    for (auto &s : signal)
    {
        // remove dc-bias
        s -= dcBias;

        // subsonic waves
        follow += a*(s-follow);
        s -= follow;
    }

    // cut silence
    mAudioRecorder->cutSilence(signal);

    // signal size may be changed
    N=(uint)signal.size();

    // 2. Determine the initial energy of the keystroke:
    uint blocksize = std::min(N,sr/5);  // 0.2 sec
    if (blocksize<=0) return 0;
    double E0=0;
    for (uint i=0; i<blocksize; i++) E0+=signal[i]*signal[i];
    E0 *= 2.0/blocksize;

    if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE)
    {
        // 3. Modify the input signal in such a way that the volume is constant
        const double gamma=50.0/sr;
        double E1=E0,E2=E0,E3=E0;
        for (auto &s : signal) {
            E1 += gamma * (s*s-E1);
            E2 += gamma * (E1-E2);
            E3 += gamma * (E2-E3);
            s /= (sqrt(fabs(E3))+0.001);
        }

        // 4. Fade in and out at the end of the buffer
        blocksize = N/50;
        for (uint i=0; i<blocksize; i++) {
            signal[i]     *= (double)i/blocksize;
            signal[N-i-1] *= (double)i/blocksize;
        }
        return N/log(E0/E3)/sr; // return the average decay time of the envelope
    }
    else if (mAnalyzerRole == ROLE_ROLLING_FFT)
    {
        // 4. Apply a hanning window
        for (size_t n = 0; n < N; ++n) {
//            signal[n] *= 0.5 * (1.0 - cos((MathTools::TWO_PI * n) / (N - 1)));
        }

    }

    return 0;
}

//-----------------------------------------------------------------------------
//			                Signal preprocessing
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function for signal processing.
///
/// Create an FFT vector and perform the FFT.
///
/// On ROLE_ROLLING_FFT this will permanentely send FinalFFT
/// On ROLE_RECORD_KEYSTRO this will peramaentely send NewFFT
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::signalProcessing(FFTWVector &signal, int samplingrate) {
    if (signal.size() == 0) {
        LogW("Empty signal. Cancelling the signal processing");
        return;
    } else if (samplingrate <= 0) {
        LogW("Invalid sampling rate. Cancelling the signal processing");
        return;
    }

    mPowerspectrum = std::make_shared<FFTData>();
    mPowerspectrum->samplingRate = samplingrate;
    PerformFFT(signal, mPowerspectrum->fft);
    if (cancelThread()) return;

    // The FFT is too long to be plotted. Therefore, we
    // create here a shorter polygon and transmit it by a message
    std::shared_ptr<FFTPolygon> polygon = std::make_shared<FFTPolygon>();
    createPolygon (mPowerspectrum->fft, *polygon.get());

    MessageHandler::send<MessageNewFFTCalculated>
            ((!mRecording) ? MessageNewFFTCalculated::FFTMessageTypes::FinalFFT :
                       MessageNewFFTCalculated::FFTMessageTypes::NewFFT,
             mPowerspectrum, polygon);

    // recognize key
    mKeyRecognizer.recognizeKey(false, mPiano, mPowerspectrum, mSelectedKey, mKeyForced);

    if (mAnalyzerRole == ROLE_ROLLING_FFT) {
        analyzeSignal();
    }

    // automatic key selection
    if (!Settings::getSingleton().isAutomaticKeySelectionDisabled()) {
        // if the recognizer finds a key next to the current key, change the selected key
        // of course if the user forced the key, this should not work
        // also select the key if there is non selected
        int identifiedKey = identifySelectedKey();
        if ((!mKeyForced && abs(identifiedKey - mSelectedKey) == 1 && identifiedKey != -1)
            || mSelectedKey == -1) {
            MessageHandler::sendUnique<MessageKeySelectionChanged>(identifiedKey, mPiano->getKeyPtr(identifiedKey));
        }
    }
}

//-----------------------------------------------------------------------------
//			            Fast Fourier transform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Perform fast Fourier transform
///
/// This function transforms the real-valued signal of length N to a
/// complex-valued Fourier transform of length N/2+1. Then it computes the
/// power spectrum by computing the intensities (squares).
/// \param signal : reference to the vector of the incoming audio signal.
/// \param powerspectrum : reference to the resulting powerspectrum.
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::PerformFFT (FFTWVector &signal, FFTWVector &powerspectrum)
{
    FFTComplexVector cvec;
    mFFT.calculateFFT(signal,cvec);
    powerspectrum.clear();
    for (auto &c : cvec)
        powerspectrum.push_back (c.real()*c.real()+c.imag()*c.imag());
}


//-----------------------------------------------------------------------------
//			                  Clipping detector
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Clipping detector
///
/// The signature of an overdriven signal is the clipping at certain maximal
/// amplitudes. This is recognized here by counting the PCM values which are
/// close to the maximal amplitude.
///
/// Similarly, some audio devices transmit intermittent data with random
/// strings of zeros in between. This is detected by counting the number of
/// vanishing PCM amplitudes.
/// \param signal : vector holding the incoming audio signal
/// \return true if a problem has been detected, false if not
///////////////////////////////////////////////////////////////////////////////

bool SignalAnalyzer::detectClipping(FFTWVector signal)
{
    int nullcnt=0, maxcnt=0, mincnt=0;
    double maxamp=0, minamp=0;
    for (auto &y : signal)
    {
        if (y>maxamp) maxamp=y;
        else if (y>=maxamp*0.99) maxcnt++;
        if (y<minamp) minamp=y;
        else if (y<=minamp*0.99) mincnt++;
        if (y==0) nullcnt++;
    }
    const int threshold = static_cast<int>(signal.size()) / 50;
    if (maxcnt+mincnt > threshold)
    {
        LogW("SignalAnalyzer: High-amplitude clipping detected");
        return true;
    }
    else if (nullcnt>threshold)
    {
        LogW("SignalAnalyzer: Highly intermittent signal detected (lot of zero amplitudes)");
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
//			         Create polygon for drawing
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a polygon for drawing
///
/// \param powerspec : reference to the power spectrum rendered by the FFT
/// \param poly : reference to a map relating frequency and power (f->I).
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::createPolygon (const FFTWVector &powerspec, FFTPolygon &poly) const
{
    const double fmin = 25;
    const double fmax = 6000;
    const double cents = 10;
    const double factor = pow(2.0,cents/2400);

    size_t fftsize = powerspec.size();
    EptAssert(fftsize>0,"powerspectum has to be non-empty");
    int samplingrate = mAudioRecorder->getSampleRate();
    auto q = [fftsize,samplingrate] (double f) { return 2*fftsize*f/samplingrate; };

    double qs1 = q(fmin/factor);
    int q1 = std::max<int>(0, MathTools::roundToInteger(qs1));
    double leftarea = (q1-qs1+0.5)*powerspec[q1];
    double ymax=0, df = samplingrate / 2 / fftsize;
    for (double f=fmin; f<=fmax; f=std::max(f*factor*factor,f+df))
    {
        const double qs2 = q(f*factor);
        const int q2 = std::min<int>(MathTools::roundToInteger(qs2), static_cast<int>(powerspec.size()) - 1);
        double sum=0;
        for (int q=q1+1; q<=q2; ++q) sum += powerspec[q];
        const double rightarea = (q2-qs2+0.5)*powerspec[q2];
        const double y = sum + leftarea - rightarea;
        if (y>ymax) ymax=y;
        poly[f] = y;
        q1=q2; qs1=qs2; leftarea=rightarea;
    }
    if (ymax <= 0) {
        LogW("Power should be nonzero, possibly empty data.");
    } else {
        for (auto &p : poly) p.second /= ymax; // normalize
    }
}


//-----------------------------------------------------------------------------
//                        Identify the selected key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Identify the selected key.
///
/// The KeyRecognizer has continuously sent messages containing the number
/// of the recognized key. These results have beeen registered by the
/// in the histogram mKeyCount. This function declares a
/// key as selected if more it was recognized for more than 50%.
/// \return Number of the key, -1 if none.
///////////////////////////////////////////////////////////////////////////////

int SignalAnalyzer::identifySelectedKey()
{
    // Take the majority selection from KeyRecognizer
    std::lock_guard<std::mutex> lock(mKeyCountStatisticsMutex);
    if (mKeyCountStatistics.size()==0) return -1;
    auto cmp = [](const std::pair<int,int>& p1, const std::pair<int,int>& p2)
                 { return p1.second < p2.second; };
    auto max = std::max_element(mKeyCountStatistics.begin(),mKeyCountStatistics.end(),cmp);
    if (max->second > static_cast<int>(mKeyCountStatistics.size()/2)) return max->first;

    return -1;
}

//-----------------------------------------------------------------------------
//                    Callback function of the KeyRecognizer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Callback function of the KeyRecognizer
///
/// This callback function is called by the KeyRecognizer in case of a
/// successful key recognition. Its purpose is to inform the SignalAnalyzer
/// about recognized keys because the SignalAnalyzer keeps track of the
/// statistics. In the end the finally recognized key is declared as the one
/// which hat the majority of individual recognitions. This statistical
/// information is held in mKeyCountStatistics.
/// \param keyIndex : Index of the key
/// \param frequency : Recognized frequency
///////////////////////////////////////////////////////////////////////////////

void SignalAnalyzer::keyRecognized(int keyIndex, double frequency)
{
    EptAssert(mPiano, "Piano has to be set.");

    if (mAnalyzerRole == ROLE_RECORD_KEYSTROKE)
    {
        // fetch the current key from the statistics
        if (keyIndex >= 0 and keyIndex < mPiano->getKeyboard().getNumberOfKeys())
        {
            std::lock_guard<std::mutex> lock(mKeyCountStatisticsMutex);
            mKeyCountStatistics[keyIndex]++;
        }
        MessageHandler::send<MessagePreliminaryKey>(identifySelectedKey(),frequency);
    }
    else
    {
        // In all other modes only one key is held in the statistics
        std::lock_guard<std::mutex> lock(mKeyCountStatisticsMutex);
        // this is the only key
        mKeyCountStatistics.clear();
        mKeyCountStatistics[keyIndex]++;
        // send it as a preliminary recognized key
        MessageHandler::send<MessagePreliminaryKey>(keyIndex,frequency);
    }
}

////-----------------------------------------------------------------------------
////			write spectrum to disk in XMGRACE-readable format
////-----------------------------------------------------------------------------

//void SignalAnalyzer::WriteFFT (std::string filename, const FFTWVector &fft)
//{
//    std::ofstream os(filename);
//    uint samplingrate = mAudioRecorder->getSampleRate();
//    uint fftsize = fft.size();
//    auto qtof = [samplingrate,fftsize] (int q) { return (double)samplingrate*q/fftsize/2; };
//    os << "@g0 type logy" << std::endl;
//    os << "@    xaxis  label \"FREQUENCY (HZ)\"" << std::endl;
//    os << "@    yaxis  label \"INTENSITY\"" << std::endl;
//    os << "@    subtitle \"SPECTRUM  OF THE RECORDED SIGNAL\"" << std::endl;
//    for (uint q=0; q<fft.size(); ++q) os << qtof(q) << "\t" << fft[q] << std::endl;
//    os.close();
//}

//void SignalAnalyzer::WriteSignal (std::string filename, const FFTWVector &signal)
//{
//    std::ofstream os(filename);
//    int samplingrate = mAudioRecorder->getSampleRate();
//    os << "@    xaxis  label \"TIME (s)\"" << std::endl;
//    os << "@    yaxis  label \"AMPLITUDE\"" << std::endl;
//    os << "@    subtitle \"RECORDED SIGNAL\"" << std::endl;
//    for (uint i=0; i<signal.size(); ++i) os << (double)i/samplingrate << "\t" << signal[i] << std::endl;
//    os.close();
//}
