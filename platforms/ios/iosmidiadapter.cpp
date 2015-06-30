#include "iosmidiadapter.h"
#include "iosnativewrapper.h"

IOsMidiAdapter::IOsMidiAdapter() {

}

void IOsMidiAdapter::init() {
    iosMidiInit();
}

void IOsMidiAdapter::exit() {
    iosMidiExit();
}

int IOsMidiAdapter::GetNumberOfPorts () {
    return iosMidiGetNumberOfPorts();
}

std::string IOsMidiAdapter::GetPortName(int i) {
    return iosMidiGetPortName(i);
}

bool IOsMidiAdapter::OpenPort(int i, std::string) {
    return iosMidiOpenPort(i);
}

bool IOsMidiAdapter::OpenPort(std::string) {
    return iosMidiOpenPort();
}

int IOsMidiAdapter::getCurrentPort() const {
    return iosMidiGetCurrentPort();
}
