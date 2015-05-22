#ifndef PIANODEFINES_H
#define PIANODEFINES_H

namespace piano {

    /// Enumeration of piano types
    enum PianoType
    {
        PT_GRAND = 0,                          ///< The piano is a grand piano
        PT_UPGRIGHT,                           ///< The piano is a upgright piano

        PT_COUNT
    };

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The KeyState enum
    ///
    ///////////////////////////////////////////////////////////////////////////////
    enum KeyState {
        STATE_NORMAL = 0,            ///< Normal state
        STATE_FORCED,                ///< Key is forced

        STATE_COUNT
    };

    //////////////////////////////////////////////////////////////////////////////
    /// \brief The KeyColor enum
    ///
    //////////////////////////////////////////////////////////////////////////////
    enum KeyColor {
        KC_BLACK = 0,             ///< Black key
        KC_WHITE,                 ///< White key

        KC_COUNT
    };

}  // namespace piano

#endif // PIANODEFINES_H

