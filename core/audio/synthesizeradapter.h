/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#ifndef SYNTHESIZERADAPTER_H
#define SYNTHESIZERADAPTER_H

#include "rawdatawriter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Adapter class for a simple synthesizer based on Fourier modes.
///
/// This is a simple software synthesizer that can reproduce any
/// frequency spectrum.
///
/// A sound is produced in three steps. At first the sound has to be created
/// by calling the function CreateSound. Secondly one has to add one or several
/// Fourier modes with a given frequency and amplitude. This allows one to
/// create any sound texture. Finally, by calling the function PlaySound,
/// the waveform is calculated and sent to the actual audio implementation.
///
/// Each sound is identified by an ID, usually the number of the piano key.
///
/// The synthesizer supports basic ADSR-envelopes (attack-decay-sustain-release)
/// which are known from traditional synthesizers. The ADSR-envelope of each sound
/// (note) can be chosen individually. The release phase is triggered by calling
/// the function ReleaseSound.
///
/// This adapter has no implementation file.
///////////////////////////////////////////////////////////////////////////////

class SynthesizerAdapter : public RawDataWriter
{
public:
    // Create a new sound (note)
    virtual void createSound (int id,               // Id of the sound
                              double volume=1,      // overall volume
                              double stereo=0.5,    // stereo position (0..1)
                              double attack=10,     // ADSR attack rate
                              double decayrate=0.5, // ADSR decay rate
                              double sustain=0.0,   // ADSR sustain level
                              double release=10)=0; // ADSR release rate

    // Add a Fourier component
    virtual void addFourierComponent (int id, double f, double amplitude) = 0;

    // Start playing
    virtual void playSound (int id) = 0;

    // Stop playing
    virtual void releaseSound (int id) = 0;

    // Check whether sound is still playing
    virtual bool isPlaying (int id) = 0;

    // Modify the sustain level of a constantly playing sound
    virtual void ModifySustainLevel (int id, double level) = 0;
};

#endif // SYNTHESIZERADAPTER_H
