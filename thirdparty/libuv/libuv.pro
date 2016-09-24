TEMPLATE = lib
CONFIG += dll c++11 noqt
CONFIG -= qt
DESTDIR = $$shadowed($$PWD)

# library settings for uv
TARGET = uv

DEFINES += BUILDING_UV_SHARED

INCLUDEPATH += libuv/include libuv/src

HEADERS += \
        libuv/src/heap-inl.h \

SOURCES += \
        libuv/src/fs-poll.c \
        libuv/src/inet.c \
        libuv/src/queue.h \
        libuv/src/threadpool.c \
        libuv/src/uv-common.c \
        libuv/src/uv-common.h \
        libuv/src/version.c

HEADERS += libuv/include/uv.h libuv/include/uv-errno.h libuv/include/uv-threadpool.h libuv/include/uv-version.h

linux|android|mac {
    # unix specific
    SOURCES += \
                   libuv/src/unix/async.c \
                   libuv/src/unix/atomic-ops.h \
                   libuv/src/unix/core.c \
                   libuv/src/unix/dl.c \
                   libuv/src/unix/fs.c \
                   libuv/src/unix/getaddrinfo.c \
                   libuv/src/unix/getnameinfo.c \
                   libuv/src/unix/internal.h \
                   libuv/src/unix/loop-watcher.c \
                   libuv/src/unix/loop.c \
                   libuv/src/unix/pipe.c \
                   libuv/src/unix/poll.c \
                   libuv/src/unix/process.c \
                   libuv/src/unix/signal.c \
                   libuv/src/unix/spinlock.h \
                   libuv/src/unix/stream.c \
                   libuv/src/unix/tcp.c \
                   libuv/src/unix/thread.c \
                   libuv/src/unix/timer.c \
                   libuv/src/unix/tty.c \
                   libuv/src/unix/udp.c
}

linux|android {
    # linux specific

    SOURCES += \
        libuv/src/unix/linux-core.c \
        libuv/src/unix/linux-inotify.c \
        libuv/src/unix/linux-syscalls.c \
        libuv/src/unix/linux-syscalls.h \
        libuv/src/unix/proctitle.c

    HEADERS += \
        libuv/include/uv-linux.h

}

mac {
    # darwin specific

    SOURCES += \
        libuv/src/unix/darwin.c \
        libuv/src/unix/darwin-proctitle.c \
        libuv/src/unix/fsevents.c \
        libuv/src/unix/kqueue.c \
        libuv/src/unix/proctitle.c

    HEADERS += libuv/include/uv-darwin.h

    DEFINES += "_DARWIN_USE_64_BIT_INODE=1"
    DEFINES += "_DARWIN_UNLIMITED_SELECT=1"
}

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN
    DEFINES += "_WIN32_WINNT=0x0600"

    INCLUDEPATH += $$PWD/libuv/src/win

    HEADERS += \
        libuv/include/uv-win.h \
        libuv/include/tree.h

    SOURCES += \
        libuv/src/win/async.c \
        libuv/src/win/atomicops-inl.h \
        libuv/src/win/core.c \
        libuv/src/win/detect-wakeup.c \
        libuv/src/win/dl.c \
        libuv/src/win/error.c \
        libuv/src/win/fs-event.c \
        libuv/src/win/fs.c \
        libuv/src/win/getaddrinfo.c \
        libuv/src/win/getnameinfo.c \
        libuv/src/win/handle.c \
        libuv/src/win/handle-inl.h \
        libuv/src/win/internal.h \
        libuv/src/win/loop-watcher.c \
        libuv/src/win/pipe.c \
        libuv/src/win/poll.c \
        libuv/src/win/process-stdio.c \
        libuv/src/win/process.c \
        libuv/src/win/req.c \
        libuv/src/win/req-inl.h \
        libuv/src/win/signal.c \
        libuv/src/win/stream.c \
        libuv/src/win/stream-inl.h \
        libuv/src/win/tcp.c \
        libuv/src/win/thread.c \
        libuv/src/win/timer.c \
        libuv/src/win/tty.c \
        libuv/src/win/udp.c \
        libuv/src/win/util.c \
        libuv/src/win/winapi.c \
        libuv/src/win/winapi.h \
        libuv/src/win/winsock.c \
        libuv/src/win/winsock.h

    LIBS += -lws2_32 -lpsapi -liphlpapi -lshell32 -luser32 -luserenv -lAdvapi32
}
