include(../../../entropypianotuner_func.pri)
$$declareStaticLibrary()

# library settings for uv

TARGET = uv

INCLUDEPATH += include src

HEADERS += \
        src/heap-inl.h \

SOURCES += \
        src/fs-poll.c \
        src/inet.c \
        src/queue.h \
        src/threadpool.c \
        src/uv-common.c \
        src/uv-common.h \
        src/version.c

HEADERS += include/uv.h include/uv-errno.h include/uv-threadpool.h include/uv-version.h

linux|android|mac {
    # unix specific
    SOURCES += \
                   src/unix/async.c \
                   src/unix/atomic-ops.h \
                   src/unix/core.c \
                   src/unix/dl.c \
                   src/unix/fs.c \
                   src/unix/getaddrinfo.c \
                   src/unix/getnameinfo.c \
                   src/unix/internal.h \
                   src/unix/loop-watcher.c \
                   src/unix/loop.c \
                   src/unix/pipe.c \
                   src/unix/poll.c \
                   src/unix/process.c \
                   src/unix/signal.c \
                   src/unix/spinlock.h \
                   src/unix/stream.c \
                   src/unix/tcp.c \
                   src/unix/thread.c \
                   src/unix/timer.c \
                   src/unix/tty.c \
                   src/unix/udp.c
}

linux|android {
    # linux specific

    SOURCES += \
        src/unix/linux-core.c \
        src/unix/linux-inotify.c \
        src/unix/linux-syscalls.c \
        src/unix/linux-syscalls.h \
        src/unix/proctitle.c

    HEADERS += \
        include/uv-linux.h

}

mac {
    # darwin specific

    SOURCES += \
        src/unix/darwin.c \
        src/unix/darwin-proctitle.c \
        src/unix/fsevents.c \
        src/unix/kqueue.c \
        src/unix/proctitle.c

    HEADERS += include/uv-darwin.h

    DEFINES += "_DARWIN_USE_64_BIT_INODE=1"
    DEFINES += "_DARWIN_UNLIMITED_SELECT=1"
}

win32 {
    SOURCES += \
        src/win/async.c \
        src/win/atomicops-inl.h \
        src/win/core.c \
        src/win/dl.c \
        src/win/error.c \
        src/win/fs-event.c \
        src/win/fs.c \
        src/win/getaddrinfo.c \
        src/win/getnameinfo.c \
        src/win/handle.c \
        src/win/handle-inl.h \
        src/win/internal.h \
        src/win/loop-watcher.c \
        src/win/pipe.c \
        src/win/poll.c \
        src/win/process-stdio.c \
        src/win/process.c \
        src/win/req.c \
        src/win/req-inl.h \
        src/win/signal.c \
        src/win/snprintf.c \
        src/win/stream.c \
        src/win/stream-inl.h \
        src/win/tcp.c \
        src/win/thread.c \
        src/win/timer.c \
        src/win/tty.c \
        src/win/udp.c \
        src/win/util.c \
        src/win/winapi.c \
        src/win/winapi.h \
        src/win/winsock.c \
        src/win/winsock.h

    LIBS += -lws2_32 -lpsapi -liphlpapi -lshell32 -luserenv
}
