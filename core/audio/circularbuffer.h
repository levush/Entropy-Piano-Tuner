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

//=============================================================================
//                             Circular buffer
//=============================================================================

#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <vector>
#include <assert.h>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
/// \brief Template class for a circular buffer
///
/// A circular buffer is a container with a fixed maximum size. When more data
/// is written the oldest data is overwritten in order to keep the maximum size constant.
/// The current content of the buffer can be retrieved as a time-ordered vector
/// by calling the function getOrderedData().
///////////////////////////////////////////////////////////////////////////////

template <class data_type>
class CircularBuffer
{
public:
    CircularBuffer(std::size_t maximum_size = 0);   ///< Construct an empty buffer

    void clear();                                   ///< Clear the buffer
    void push_back(const data_type &data);          ///< Add a new data element to the buffer
    void resize(std::size_t maximum_size);          ///< Resize the buffer, shrink data if necessary
    std::vector<data_type> getOrderedData() const;  ///< Retrieve time-ordered data
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
//                      Circular buffer implementation
//=============================================================================

//-------------------------------- Constructor --------------------------------

/// Default constructor, creating an empty buffer with maximal size zero.

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


//----------------------------- clear the buffer ------------------------------

/// Calling this function clears the buffer but it does not change its maximum size.

template <class data_type>
void CircularBuffer<data_type>::clear()
{
    mCurrentWritePosition = 0;
    mCurrentReadPosition = 0;
    mCurrentSize = 0;
}


//------------------------------- add a new entry -----------------------------

/// This will overwrite the oldest data if the current size reaches the maximum size.
/// \param data : new data element to be added

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


//---------------------------- get ordered data -------------------------------

/// Retrieve all data contained in the cyclic buffer in a temporally ordered form as a vector.
/// The higher the index, the newer the data.
/// \return vector of the template class type containing the data.

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


//---------------------------- resize the buffer ------------------------------

/// If the new maximal size is smaller than the actual data size
/// only the newest data will be kept.
/// \param maximum_size : The new maximal size of the circular buffer.

template <class data_type>
void CircularBuffer<data_type>::resize(std::size_t maximum_size) {
    // we have to temporarily store the old data and rewrite at at position 0
    std::vector<data_type> old_data(std::move(getOrderedData()));
    mMaximumSize = maximum_size;
    // new current size is size of old data, or smaller)
    mCurrentSize = std::min(old_data.size(), maximum_size);
    // resize or internal data structure
    mData.resize(mMaximumSize);

    // dont copy data if new buffer is empty
    if (mMaximumSize == 0) {
        return;
    }
    // reset read and write pointers
    mCurrentReadPosition = 0;
    mCurrentWritePosition = mCurrentSize % mMaximumSize;
    // copy the old data
    std::memcpy(mData.data(), old_data.data() + (old_data.size() - mCurrentSize), mCurrentSize * sizeof(data_type));
}





#endif // CIRCULAR_BUFFER

