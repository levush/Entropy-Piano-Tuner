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
//                             Circular buffer
//=============================================================================

#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <vector>
#include <assert.h>
#include <cstring>
#include <algorithm>

#include "prerequisites.h"
#include "core/system/eptexception.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Template class for a circular buffer.
///
/// A circular buffer is a cyclic container with a fixed maximum size.
/// When more data is written the oldest data is cyclicylly overwritten
/// in order to keep the maximum size constant.
///
/// The current content of the buffer can be retrieved as a time-ordered vector
/// by calling the function getOrderedData().
///
/// The circular buffer is used by the AudioRecorder and the SignalAnalyzer.
///
/// This class contains of a header file only. There is no corresponding
/// implementation (cpp) file.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
class CircularBuffer
{
public:
    CircularBuffer(std::size_t maximum_size = 0);   ///< Construct an empty buffer of maximal size zero

    void clear();                                   ///< Clear the buffer, keeping its maximal size
    void push_back(const data_type &data);          ///< Append a new data element to the buffer
    void resize(std::size_t maximum_size);          ///< Resize the buffer, shrink oldest data if necessary
    std::vector<data_type> getOrderedData() const;  ///< Copy entire data in a time-ordered form
    std::vector<data_type> readData(size_t n);      ///< Retrieve time-ordeded data with maximum size of n and remove if from the buffer
    std::size_t size() const {return mCurrentSize;} ///< Return current buffer size
    std::size_t maximum_size() const
        {return mMaximumSize;}                      ///< Return actual maximal size

private:
    std::size_t mCurrentWritePosition;              ///< Current read position
    std::size_t mCurrentReadPosition;               ///< Current write position
    std::size_t mMaximumSize;                       ///< Maximal size of the buffer
    std::size_t mCurrentSize;                       ///< Current size of the buffer
    std::vector<data_type> mData;                   ///< Internal cyclic data buffer
};

//=============================================================================
//    Circular buffer implementation (contained in header because of template)
//=============================================================================

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, creating an empty buffer with maximal size zero.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
CircularBuffer<data_type>::CircularBuffer(std::size_t maximum_size)
    : mCurrentWritePosition(0),
      mCurrentReadPosition(0),
      mMaximumSize(maximum_size),
      mCurrentSize(0),
      mData()
{
    mData.resize(mMaximumSize);
}


//-----------------------------------------------------------------------------
//                  Clear the buffer, keeping its maximal size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Calling this function clears the buffer, setting the actual size to zero,
/// but it does not change its maximum size.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
void CircularBuffer<data_type>::clear()
{
    mCurrentWritePosition = 0;
    mCurrentReadPosition = 0;
    mCurrentSize = 0;
}


//-----------------------------------------------------------------------------
//                        Append a new data element
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function appends a new element. If the current size reaches the
/// maximum size the oldest data is overwritten.
/// \param data : New data element to be added.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
void CircularBuffer<data_type>::push_back(const data_type &data)
{
    assert(mMaximumSize > 0);
    assert(mCurrentWritePosition < mMaximumSize);
    assert(mCurrentReadPosition < mMaximumSize);
    // write at the desired position
    mData[mCurrentWritePosition] = data;
    // update write pointer
    mCurrentWritePosition = (mCurrentWritePosition+1) % mMaximumSize;
    // update current size
    mCurrentSize = std::min(mCurrentSize + 1, mMaximumSize);
    // update read pointer (add mMaximumSize to keep it positive)
    mCurrentReadPosition = (mCurrentWritePosition + mMaximumSize - mCurrentSize) % mMaximumSize;
}


//-----------------------------------------------------------------------------
//                      Copy data in an ordered form
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Since the buffer is written cyclically, the newest data entry is somewhere
/// but not necessarily at the phyical end of the buffer. This function
/// therefore retrieves all data contained in the cyclic buffer in a
/// temporally ordered form as a vector, i.e., the higher the index,
/// the newer the data. The data will NOT be removed from the buffer
///
/// \return vector of the template class type containing the data.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
std::vector<data_type> CircularBuffer<data_type>::getOrderedData() const
{
    std::vector<data_type> data_out(mCurrentSize);
    // copy end
    std::size_t part1Size = std::min(mCurrentReadPosition + mCurrentSize, mMaximumSize) - mCurrentReadPosition;
    std::memcpy(data_out.data(), mData.data() + mCurrentReadPosition, part1Size * sizeof(data_type));

    // copy start
    std::size_t part2Size = mCurrentSize - part1Size;
    std::memcpy(data_out.data() + part1Size, mData.data(), part2Size * sizeof(data_type));

    return data_out;
}



//-----------------------------------------------------------------------------
//  Retrieve time-ordeded data with max size of n and remove if from the buffer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function retrieves the existing data up to a given maximal size n
/// in a time-ordered form and removes this data from the buffer.
/// \return Vector of the template class type containing the data.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
std::vector<data_type> CircularBuffer<data_type>::readData(size_t n)
{
    std::vector<data_type> data_out(std::move(getOrderedData()));
    if (data_out.size() > n) data_out.resize(n);

    EptAssert(mCurrentSize >= data_out.size(), "Do not read more data than existent.");

    // reset read position
    mCurrentReadPosition = (mCurrentReadPosition + data_out.size()) % mMaximumSize;
    mCurrentSize = mCurrentSize - data_out.size();

    return data_out;
}


//-----------------------------------------------------------------------------
//                            Resize the buffer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Set a new maximal buffer size.
/// If the new maximal size is smaller than the actual data size
/// only the newest data will be kept.
/// \param maximum_size : The new maximal size of the circular buffer.
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
void CircularBuffer<data_type>::resize(std::size_t maximum_size)
{
    // we have to temporarily store the old data and rewrite at at position 0
    std::vector<data_type> old_data(std::move(getOrderedData()));
    mMaximumSize = maximum_size;
    // new current size is size of old data, or smaller)
    mCurrentSize = std::min(old_data.size(), maximum_size);
    // resize or internal data structure
    mData.resize(mMaximumSize);

    // dont copy data if new buffer is empty
    if (mMaximumSize == 0) return;

    // reset read and write pointers
    mCurrentReadPosition = 0;
    mCurrentWritePosition = mCurrentSize % mMaximumSize;
    // copy the old data
    std::memcpy(mData.data(),
                old_data.data() + (old_data.size() - mCurrentSize),
                mCurrentSize * sizeof(data_type));
}

//-----------------------------------------------------------------------------
//                           Explicit instance
//-----------------------------------------------------------------------------

template class EPT_EXTERN CircularBuffer<double>;
template class EPT_EXTERN CircularBuffer<float>;

#endif // CIRCULAR_BUFFER
