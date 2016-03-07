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
//                            Calculation adapter
//=============================================================================

#ifndef CALCULATIONADAPTER_H
#define CALCULATIONADAPTER_H

#include "prerequisites.h"

class Core;

///////////////////////////////////////////////////////////////////////////////
/// \brief Adapter for starting and stopping the calculation process
///
/// This adapter is part of the interface between the core and the GUI.
/// It allows the GUI to start and to interrupt (stop) the calculation process.
/// The adapter is reimplemented in Qt as CalculationProgressGroup.
///
/// \see CalculationProgressGroup
///
/// The adapter calls several functions of the Singleton-Calculationmanager.
///
/// \see CalculationManager
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN CalculationAdapter
{
public:
    CalculationAdapter(Core *core);         // Constructor
    virtual ~CalculationAdapter() {}        ///< Empty virtual destructor

    virtual void startCalculation();
    virtual void cancelCalculation();

    std::vector<std::string> getAvailableAlgorithms() const;

private:
    Core *mCore;                            ///< Private pointer to the core
};

#endif // CALCULATIONADAPTER_H
