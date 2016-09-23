#ifndef MIDIEXPORT_H
#define MIDIEXPORT_H

// export defines for dynamic dlls on windows
#if defined(_WIN32)
# ifdef MIDI_BUILD
#   define MIDI_EXTERN __declspec(dllexport)
#   define MIDI_EXTERN_TEMPLATE template
# else
#   define MIDI_EXTERN __declspec(dllimport)
#   define MIDI_EXTERN_TEMPLATE extern template
# endif
#else
# define MIDI_EXTERN // nothing
# define MIDI_EXTERN_TEMPLATE template
#endif

#if defined(_MSC_VER)
#pragma warning (disable : 4251)
#endif

#endif // MIDIEXPORT_H
