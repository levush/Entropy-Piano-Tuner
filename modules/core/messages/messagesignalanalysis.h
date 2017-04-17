#ifndef MESSAGESIGNALANALYSIS_H
#define MESSAGESIGNALANALYSIS_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Message sent whenever the data associated with a single key changes.
///////////////////////////////////////////////////////////////////////////////

class MessageSignalAnalysis : public Message
{
public:
    enum class Status {
        STARTED,
        ENDED
    };

    enum class Result {
        SUCCESSFULL,
        INVALID
    };

    MessageSignalAnalysis(Status status, Result result = Result::SUCCESSFULL, int invalidAttempts = -1)
        : Message(Message::MSG_SIGNAL_ANALYSIS)
        , mStatus(status)
        , mResult(result)
        , mInvalidAttempts(invalidAttempts) {

    }

    Status status() const {return mStatus;}
    Result result() const {return mResult;}
    int invalidAttempts() const {return mInvalidAttempts;}

private:
    const Status mStatus;
    const Result mResult;
    const int mInvalidAttempts;

};

#endif // MESSAGESIGNALANALYSIS_H
