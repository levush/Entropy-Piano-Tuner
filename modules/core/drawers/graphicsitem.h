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
//                    Abstract class of a Graphics Item
//============================================================================

#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <list>
#include <cstdint>
#include "prerequisites.h"

class GraphicsViewAdapter;

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a single item in a graphics view
///
/// Whenever a new element is drawn in the GraphicsViewAdapter a new GraphicItem
/// has to be created. This item will be usful whenever a single item has to be
/// changed. For example, the tuning marks have to be moved during the
/// calculation process.
///
/// You can set a key index to identify an item with a key and you can set a
/// user-defined role that will be treated as bitwise flag. All of this is
/// useful if you want to find a single item in the list of all items in a
/// GraphicsViewAdapter.
///
/// A key index of -1 is allowed to indicate no link with a key. This is
/// normally a global item.
///
/// The GUI implementation should define its own GraphicsItem and reimplement
/// setPosition(float, float)
///
/// Whenever the GraphicsItem is changed the GraphicsViewAdapter implementation
/// should redraw the corresponding element, but not the complete panel.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN GraphicsItem
{
public:

    ///////////////////////////////////////////////////////////////////////////
    /// Type of a user role that can be set in a GraphicsItem (bitwise flag).
    ///////////////////////////////////////////////////////////////////////////
    using RoleType = std::uint32_t;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    ///
    /// It will automatically add this as new element to the GraphicsViewAdapter.
    /// \param graphicsView : The parent GraphicsViewAdapter
    ///////////////////////////////////////////////////////////////////////////
    GraphicsItem(GraphicsViewAdapter *graphicsView);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual Destructor.
    ///
    /// It will automatically remove this as element from the GraphicsViewAdapter.
    ///////////////////////////////////////////////////////////////////////////
    virtual ~GraphicsItem();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Short function for setting the key index and the role.
    ///
    /// The function will all setItemRole(RoleType) and setKeyIndex(int).
    /// \param index : The key index of the item
    /// \param role : The user defined item role
    ///////////////////////////////////////////////////////////////////////////
    void setKeyIndexAndItemRole(int index, RoleType role);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Setter for mKeyIndex.
    /// \param index : The key index of the item
    ///////////////////////////////////////////////////////////////////////////
    void setKeyIndex(int index) {mKeyIndex = index;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mKeyIndex.
    /// \return mKeyIndex
    ///////////////////////////////////////////////////////////////////////////
    int getKeyIndex() const {return mKeyIndex;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mRole.
    /// \param role : The user defined role of the item
    ///////////////////////////////////////////////////////////////////////////
    void setItemRole(RoleType role) {mRole = role;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bitwise add of role and mRole.
    /// \param role : The role to add
    ///////////////////////////////////////////////////////////////////////////
    void addItemRole(RoleType role) {mRole |= role;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mRole.
    /// \return mRole
    ///////////////////////////////////////////////////////////////////////////
    RoleType getItemRole() const {return mRole;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mGraphicsView.
    /// \return mGraphicsView.
    ///////////////////////////////////////////////////////////////////////////
    GraphicsViewAdapter *getGraphicsView() {return mGraphicsView;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Function to set the position of the element.
    ///
    /// This function should be reimplemented by an actual implementation of the
    /// element.
    /// \param x : The relative x coord
    /// \param y : The relative y coord
    ///////////////////////////////////////////////////////////////////////////
    virtual void setPosition(double x, double y) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the z order of the element (here implemented without function)
    /// \param z : The z order
    ///////////////////////////////////////////////////////////////////////////
    virtual void setZOrder(double z) { (void)z; }

protected:

    GraphicsViewAdapter *mGraphicsView; ///< Pointer to the parent GraphicsViewAdapter.
    int mKeyIndex;                      /// The index of this item to identify it with a key.
    RoleType mRole;                     /// A user-defined role that can be set by a drawer.
};


using GraphicItemsList = std::list<GraphicsItem*>; ///< A list of GraphicItem (global)

#endif // GRAPHICSITEM_H

