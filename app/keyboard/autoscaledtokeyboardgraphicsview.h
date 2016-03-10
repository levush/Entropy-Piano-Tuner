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

#ifndef AUTOSCALEDTOKEYBOARDGRAPHICSVIEW_H
#define AUTOSCALEDTOKEYBOARDGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

#include "prerequisites.h"

#include "implementations/graphicsviewadapterforqt.h"

#include "keyboardgraphicsview.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief An GraphicsViewAdaptorForQt keeping the same size as a
///        KeyboardGraphicsView.
///
/// The tuning cureve and signal spectrum have to stay at the same size as
/// the keyboard. This class will automatically adjust the visible view if it
/// resizes or is shown.
///////////////////////////////////////////////////////////////////////////////
class AutoScaledToKeyboardGraphicsView : public GraphicsViewAdapterForQt
{
public:
    enum ItemRoles {
        ROLE_HIGHLIGHT = 1024,
    };

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param parent : The parent widget
    /// \param drawer : The drawer of this view
    ///////////////////////////////////////////////////////////////////////////////
    AutoScaledToKeyboardGraphicsView(QWidget *parent, DrawerBase *drawer);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~AutoScaledToKeyboardGraphicsView();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for the keyboard
    /// \param keyboardgraphicsview : The keyboard
    ///////////////////////////////////////////////////////////////////////////////
    void setKeyboard(KeyboardGraphicsView *keyboardgraphicsview);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function that will highlight the given key
    /// \param key : The key to highlight
    ///
    /// There may only be one highlighted key.
    ///////////////////////////////////////////////////////////////////////////////
    void highlightKey(int key);

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented to keep the visible view with the KeyboardGraphicsView
    /// \param event : The event
    ///////////////////////////////////////////////////////////////////////////////
    void showEvent(QShowEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented to keep the visible view with the KeyboardGraphicsView
    /// \param event : The event
    ///////////////////////////////////////////////////////////////////////////////
    void resizeEvent(QResizeEvent *event) override;

protected:
    /// Pointer to the KeyboardGraphicsView.
    KeyboardGraphicsView *mKeyboardGraphicsView;
};

#endif // AUTOSCALEDTOKEYBOARDGRAPHICSVIEW_H
