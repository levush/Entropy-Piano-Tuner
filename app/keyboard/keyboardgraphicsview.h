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

#ifndef KEYBOARDGRAPHICSVIEW_H
#define KEYBOARDGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>

#include "prerequisites.h"

#include "core/piano/keyboard.h"
#include "core/messages/messagelistener.h"

class AutoScaledToKeyboardGraphicsView;

///////////////////////////////////////////////////////////////////////////////
/// \brief Viewer for the keyboard.
///
/// This class will draw the keyboard into the QGraphicsView.
///
/// The class can be set to a different KeyboardMode depending on how it should
/// react on a mouse click.
/// A MODE_NORMAL just selects the clicked key. A MODE_CLICK_RAISES_FULLSCREEN
/// will open a FullScreenKeyboardDialog to display a keyboard in
/// MODE_FULLSCREEN. If MODE_SCROLLBAR is set, the keyboard will display a
/// scrollbar.
/// The window will have a fixed width-height ratio defined by
/// heightForWidth(int).
///////////////////////////////////////////////////////////////////////////////
class KeyboardGraphicsView : public QGraphicsView, public MessageListener
{
    Q_OBJECT
public:
    // drawing constants
    static const qreal GLOBAL_SCALING;                          ///< A global scaling factor
    static const qreal PEN_THIN_LINE;                           ///< Thin pen line width
    static const qreal PEN_MEDIUM_LINE;                         ///< Medium pen line width
    static const qreal KEY_WIDTH;                               ///< Width of a single key
    static const qreal KEY_HEIGHT;                              ///< Height of a single key
    static const qreal KEY_SPACING_TO_MARKERS;                  ///< Space between keys and markers
    static const qreal KEY_MARKERS_MARGIN;                      ///< Margin of a single marker
    static const qreal TOTAL_HEIGHT;                            ///< Total height of the view
    static const qreal KEY_BLACK_TO_WHITE_RATIO;                ///< Size ratio of black and white keys
    static const qreal KEY_WHITE_KEY_SIZE;                      ///< Size of a white key (a bit bigger than KEY_WIDTH)

    // color constants
    static const QColor KEY_PRELIMINARY_COLOR;                  ///< Color of the preliminary key
    static const QColor KEY_SELECTED_RECORDED_COLOR;            ///< Color of the selected key if it is recorded
    static const QColor KEY_SELECTED_UNRECORDED_COLOR;          ///< Color of the selected key if it is not recorded
    static const qreal KEY_FORCED_FACTOR;                       ///< Color scaling if the key is forced
    static const QColor KEY_PRELIMINARY_AND_SELECTED_COLOR;     ///< Color if the preliminary key equates the selected key

    static const int NO_KEY = -1;                               ///< No key selection constant


private:
    using KeyState = piano::KeyState;
    using KeyColor = piano::KeyColor;

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param parent : The parent widget
    /// \param mode : The KeyboadMode
    ///
    ///////////////////////////////////////////////////////////////////////////////
    KeyboardGraphicsView(QWidget *parent);
    ~KeyboardGraphicsView();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mScene.
    /// \return mScene
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const QGraphicsScene &getScene() const {return mScene;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter for mKeyboard.
    /// \param kb : The Keyboard
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setKeyboard(const Keyboard *kb);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mKeyboard.
    /// \return mKeyboard.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const Keyboard *getKeyboard() const {return mKeyboard;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mSelectedKey.
    /// \return mSelectedKey
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int8_t getSelectedKeyIndex() const {return mSelectedKey;}

    const Key *getSelectedKey() const {if (mSelectedKey < 0) {return nullptr;} return &mKeyboard->at(mSelectedKey);}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mSelectedKeyState.
    /// \return mSelectedKeyState.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    KeyState getSelectedKeyState() const {return mSelectedKeyState;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for mPreliminaryKey.
    /// \return mPreliminaryKey
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int8_t getPreliminaryKey() const {return mPreliminaryKey;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to set the current selection.
    /// \param key : The selected key or -1
    /// \param state : The selected KeyState
    /// \param preliminary : The preliminary key or -1
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSelection(int key, KeyState state, int preliminary);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief This function will center the view on the given key.
    /// \param key : The key to center or -1
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void centerOnKey(int8_t key);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to add a AutoScaledToKeyboardGraphicsView.
    /// \param view The view to add
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addAutoScaledGraphicsView(AutoScaledToKeyboardGraphicsView *view) {mAutoScaledGraphicsViews.push_back(view);}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to select or deselect a key.
    /// \param key0 The key index to select or -1.
    /// \param keyState The state of the key.
    /// \param notifyMessageListeners Notiy the message listeners by sending a
    ///                               message
    ///
    /// The current selected key is stored in mSelectedKey and its state in
    /// mSelectedKeyState.
    ///////////////////////////////////////////////////////////////////////////////
    void selectKey(int8_t key0, KeyState keyState, bool notifyMessageListeners = true);

    QRectF getVisibleContentsRect() const;

protected:
    virtual QSize sizeHint() const override final {return QSize(0, 200);}
    virtual QSize minimumSizeHint() const override final {return QSize(0, 100);}
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented to keep the complete scene rect in the view.
    /// \param event : The show event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void showEvent(QShowEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented to keep the complete scene rect in the view.
    /// \param event : The resize event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void resizeEvent(QResizeEvent *event) override;

    virtual void scrollContentsBy(int dx, int dy) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The mouse press will select the klicked key.
    /// \param event : The mouse event
    ///
    /// In in MODE_CLICK_RAISES_FULLSCREEN, this will open a
    /// FullScreenKeyboardDialog
    ///////////////////////////////////////////////////////////////////////////////
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Handler for key presses.
    /// \param event : The key event
    ///
    /// Pressing return will set the current selected key to forced mode.
    ///////////////////////////////////////////////////////////////////////////////
    void keyPressEvent(QKeyEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Message handling.
    /// \param m : The message
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void handleMessage(MessagePtr m) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Get the height for a given view width.
    /// \param w : The width
    /// \return Height with a fixed ratio.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int heightForWidth( int w ) const override;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to reset the key and marker color to the unselected state.
    /// \param key : The key index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void resetKey(int8_t key);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to set the key color of key.
    /// \param : key The key index
    ///
    /// Depending on mSelectedKey, mPreliminaryKey and mSelectedKeyState this
    /// function will chose the correct color and apply it to the key.
    ///////////////////////////////////////////////////////////////////////////////
    void setKeyColor(int8_t key);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to change the prelimary key stored in mPreliminaryKey.
    /// \param key0 : The new preliminary key
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void selectPreliminaryKey(int8_t key0);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Force all markers to update its state.
    ///
    /// E.g. this function is called when a new piano was loaded.
    ///////////////////////////////////////////////////////////////////////////////
    void redrawAllMarkers();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to update a single color marker.
    /// \param key0 : The index of the marker
    ///
    /// This function will read the current state of the key in the mKeyboard
    /// and adjust the QPixmap of the marker
    ///////////////////////////////////////////////////////////////////////////////
    void updateColorMarker(int8_t key0);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to get the QPixmap for the given recording state.
    /// \param recorded : The recording state
    /// \return The QPixmap that is connected to the recording state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QPixmap getPixmapForKeyRecordingState(bool recorded);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to get the key color.
    /// \param color The color of the key.
    /// \param selected Is the key selected?
    /// \param keyState The state of the key.
    /// \param recorded Is the key recorded?
    /// \return A QColor for the key
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QColor getKeyColor(piano::KeyColor color, bool selected, KeyState keyState, bool recorded);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Returns a QRectF as shape of the given key.
    /// \param key0 : The key index
    /// \return The key shape
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QRectF keyShape(int8_t key0);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to change the total number of keys and the key number of A.
    /// \param keys : Number of keys
    /// \param keyA : Index of A
    ///
    /// This function will also recreate all elements.
    ///////////////////////////////////////////////////////////////////////////////
    void changeTotalNumberOfKeys(int keys, int keyA);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Fit the complete scene rect in the given view.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void fitAllInView();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to get the key index at the given position.
    /// \param pos : The position to search for a key
    /// \return The index of the key or -1 if none was found
    ///
    /// This function is usually called by a mousePressEvent(QMouseEvent*) to
    /// select the clicked key.
    ///////////////////////////////////////////////////////////////////////////////
    int getKeyAtPosition(const QPoint &pos);

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Signal beeing emitted if the selected key changed.
    /// \param key The newly selected key index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void selectionChanged(int8_t key);

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot that will select the next key.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void selectNext();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot that will select the previous key.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void selectPrevious();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to deselect a key.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void deselectKey(bool notifyListeners = true);

private:
    int mResizeBlocked = 0;
    /// Pointer to the keyboard that is displayed.
    const Keyboard *mKeyboard;

    /// The QGraphicsScene in which the elements will be drawed.
    QGraphicsScene mScene;

    /// List of all marker items.
    std::vector<QGraphicsPixmapItem *> mMarkerPixmapItems;

    /// List of all key items.
    std::vector<QGraphicsRectItem *> mKeysGraphicsItems;


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Index of the key to center on.
    ///
    /// This variable is needed in the fullscreen keyboard to center on the key
    /// that was pressed in the small size keyboard view.
    ///////////////////////////////////////////////////////////////////////////////
    int8_t mCenterOnKey;

    /// The selected key.
    int8_t mSelectedKey;

    /// The key state of the selected key.
    KeyState mSelectedKeyState;

    /// The preliminary key.
    int8_t mPreliminaryKey;

    /// The keynumber of A of the given mKeyboard.
    int mKeyNumberOfA;

    /// List of other graphic views that are scaled along this keyboard view.
    QList<AutoScaledToKeyboardGraphicsView*> mAutoScaledGraphicsViews;
};

#endif // KEYBOARDGRAPHICSVIEW_H
