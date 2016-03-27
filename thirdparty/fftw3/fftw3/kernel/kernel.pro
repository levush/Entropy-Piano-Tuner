include(../../fftw3_func.pri)

$$fftw3SubPart()

SOURCES += \
    align.c \
    alloc.c \
    assert.c \
    awake.c \
    buffered.c \
    cpy1d.c \
    cpy2d.c \
    cpy2d-pair.c \
    ct.c \
    extract-reim.c \
    hash.c \
    iabs.c \
    kalloc.c \
    md5.c \
    md5-1.c \
    minmax.c \
    ops.c \
    pickdim.c \
    plan.c \
    planner.c \
    primes.c \
    print.c \
    problem.c \
    rader.c \
    scan.c \
    solver.c \
    solvtab.c \
    stride.c \
    tensor.c \
    tensor1.c \
    tensor2.c \
    tensor3.c \
    tensor4.c \
    tensor5.c \
    tensor7.c \
    tensor8.c \
    tensor9.c \
    tile2d.c \
    timer.c \
    transpose.c \
    trig.c \
    twiddle.c \

# empty files, that dont need to be compiled
# including them causes a linker errur (... has no symbols) under iOS
# debug.c
