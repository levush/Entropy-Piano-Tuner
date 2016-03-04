TEMPLATE = subdirs

include(../entropypianotuner_config.pri)

qwt {
  SUBDIRS += qwt
}

getmemorysize {
  SUBDIRS += getmemorysize
}

rtmidi {
  SUBDIRS += RtMidi
}

timesupport {
  SUBDIRS += timesupport
}

tinyxml2 {
  SUBDIRS += tinyxml2
}
