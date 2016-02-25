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

//============================================================================
//                 Adapter for viewing graphical elements
//============================================================================

#include "graphicsviewadapter.h"
#include "../system/eptexception.h"

//-----------------------------------------------------------------------------
//                          Clear graphics panel
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Clear the graphics panel
///
/// This function deletes all graphics items helt in the private list.
///////////////////////////////////////////////////////////////////////////////

void GraphicsViewAdapter::clear()
{
    // Delete all elements in the list of GraphicsItems
    // Note that the destructor of the graphics item calls this function
    while (mGraphicItems.size() > 0) delete mGraphicItems.front();
}


//-----------------------------------------------------------------------------
//                     Get the list of the graphic items.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the list of the graphic items.
///
/// Getter function retrieving the complete list of the drawn graphic items.
/// \returns mGraphicItems : List of graphics item.
///////////////////////////////////////////////////////////////////////////////

GraphicItemsList &GraphicsViewAdapter::getGraphicItems()
{ return mGraphicItems; }


//-----------------------------------------------------------------------------
//                 Get a constant list of the graphic items.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a constant list of the graphic items
///
/// Getter function retreiving the complete list of the drawn graphic items.
/// \returns mGraphicItems : Constant list of graphics item.
///////////////////////////////////////////////////////////////////////////////

const GraphicItemsList &GraphicsViewAdapter::getGraphicItems() const
{ return mGraphicItems; }


//-----------------------------------------------------------------------------
//              Get a pointer to a single graphics element
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a single graphics element specified by its index and its role.
///
/// This function returns the pointer to a single graphics element which
/// is specified by its index and its role.
/// Note that the function will return nullptr if no item in the list matches
/// with the request. The function will fail if more than one item matches.
/// \see getGraphicItems(RoleType)
/// \param keyIndex : Index of the graphics element
/// \param role : User-defined role of the graphics element
/// \return Pointer to the graphics item or nullptr
///////////////////////////////////////////////////////////////////////////////

GraphicsItem *GraphicsViewAdapter::getGraphicItem (int keyIndex, RoleType role)
{
    GraphicItemsList list(getGraphicItems (keyIndex, role));
    EptAssert(list.size() <= 1, "Either the item is unique or does not exist at all");
    if (list.size() > 0) return list.front();
    else return nullptr;
}


//-----------------------------------------------------------------------------
//      Get the first of all graphics elements with a given role
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the first of all graphics elements with a given role
///
/// Note that the function will return nullptr if no item matches with the
/// requested role. The function will fail if more than one item matches.
/// \see getGraphicItems(int, RoleType)
/// \param role : The role of the searched graphics element
/// \return Pointer to the first graphics element in the list or nullptr
///////////////////////////////////////////////////////////////////////////////

GraphicsItem *GraphicsViewAdapter::getGraphicItemByRole (RoleType role)
{
    GraphicItemsList list(getGraphicItemsByRole(role));
    EptAssert(list.size() <= 1, "Either the item is unique or does not exist at all");
    if (list.size() > 0) return list.front();
    else return nullptr;
}


//-----------------------------------------------------------------------------
//      Get a list of graphic items that match with the keyIndex.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a list of graphic items that match with the keyIndex.
/// \param keyIndex : Index of the key used as a filter
/// \return List of graphics items that may be empty
///////////////////////////////////////////////////////////////////////////////

GraphicItemsList GraphicsViewAdapter::getGraphicItems(int keyIndex)
{
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems)
        if (item->getKeyIndex() == keyIndex) list.push_back(item);
    return list;
}


//-----------------------------------------------------------------------------
//      Get the list of all graphics elements that match with a given role.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a list of graphic items that match with the given role.
/// \param role : Role of the graphics item used as a filter
/// \return List of graphics items that may be empty
///////////////////////////////////////////////////////////////////////////////

GraphicItemsList GraphicsViewAdapter::getGraphicItemsByRole(RoleType role)
{
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems)
        if ((role & item->getItemRole()) == role) list.push_back(item);
    return list;
}


//-----------------------------------------------------------------------------
//      Get the list of all graphics elements of a given key index
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the list of all graphics elements of a given key and role
/// \param keyIndex : Index of the key used as a filter
/// \param role : Role of the graphics item used as a filter
/// \return List of graphics items
///////////////////////////////////////////////////////////////////////////////

GraphicItemsList GraphicsViewAdapter::getGraphicItems(int keyIndex, RoleType role)
{
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems)
        if (item->getKeyIndex() == keyIndex and (role & item->getItemRole()) == role)
            list.push_back(item);
    return list;
}
