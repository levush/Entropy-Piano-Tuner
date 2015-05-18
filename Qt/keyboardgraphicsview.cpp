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

#include "keyboardgraphicsview.h"
#include <QFontDatabase>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QShortcut>
#include <QLinearGradient>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include "../core/system/eptexception.h"
#include "../core/messages/messagekeyselectionchanged.h"
#include "../core/messages/messagehandler.h"
#include "../core/messages/messagepreliminarykey.h"
#include "../core/messages/messagekeydatachanged.h"
#include "../core/messages/messageprojectfile.h"
#include "../core/piano/piano.h"
#include "autoscaledtokeyboardgraphicsview.h"
#include "fullscreenkeyboarddialog.h"

const qreal KeyboardGraphicsView::GLOBAL_SCALING = 1;
const qreal KeyboardGraphicsView::PEN_THIN_LINE = 1 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::PEN_MEDIUM_LINE = 2 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::KEY_WIDTH = 10 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::KEY_HEIGHT = 80 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::KEY_SPACING_TO_MARKERS = KeyboardGraphicsView::KEY_WIDTH + 2 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::KEY_MARKERS_MARGIN = 1 * KeyboardGraphicsView::GLOBAL_SCALING;
const qreal KeyboardGraphicsView::TOTAL_HEIGHT = KeyboardGraphicsView::KEY_HEIGHT + KeyboardGraphicsView::KEY_SPACING_TO_MARKERS;
const qreal KeyboardGraphicsView::KEY_BLACK_TO_WHITE_RATIO = 0.6;
const qreal KeyboardGraphicsView::KEY_WHITE_KEY_SIZE = KeyboardGraphicsView::KEY_WIDTH * 12.0 / 7.0;


const QColor KeyboardGraphicsView::KEY_PRELIMINARY_COLOR = Qt::gray;
const QColor KeyboardGraphicsView::KEY_SELECTED_RECORDED_COLOR(0, 184, 82);
const QColor KeyboardGraphicsView::KEY_SELECTED_UNRECORDED_COLOR(188, 3, 3);
const qreal KeyboardGraphicsView::KEY_FORCED_FACTOR = 0.5;
const QColor KeyboardGraphicsView::KEY_PRELIMINARY_AND_SELECTED_COLOR(255, 150, 0);

KeyboardGraphicsView::KeyboardGraphicsView(QWidget *parent, KeyboardMode mode)
    : QGraphicsView(parent),
      mMode(mode),
      mKeyboard(nullptr),
      mKeysGraphicsItems(0),
      mCenterOnKey(-1),
      mSelectedKey(-1),
      mSelectedKeyState(KeyState::STATE_NORMAL),
      mPreliminaryKey(-1),
      mKeyNumberOfA(-1) {
    setScene(&mScene);

    // antialiasing for nice lines
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);


    // create shortcuts for selecting next and previous key
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(selectPrevious()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(selectNext()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(deselectKey()));

    QSizePolicy p(sizePolicy());
    p.setHeightForWidth(true);
    setSizePolicy(p);

    if (mMode & MODE_SCROLLBAR) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
}

KeyboardGraphicsView::~KeyboardGraphicsView()
{

}

void KeyboardGraphicsView::setKeyboard(const Keyboard *kb) {
    mKeyboard = kb;
    if (mKeyboard) {
        changeTotalNumberOfKeys(mKeyboard->getNumberOfKeys(), mKeyboard->getKeyNumberOfA4());
    }
}

void KeyboardGraphicsView::setSelection(int key, KeyState state, int preliminary) {
    selectKey(key, state == KeyState::STATE_FORCED, false);
    selectPreliminaryKey(preliminary);
}

void KeyboardGraphicsView::centerOnKey(int8_t key) {
    mCenterOnKey = key;
    if (key >= 0 && key < static_cast<int>(mKeysGraphicsItems.size())) {
        centerOn(mKeysGraphicsItems[key]);
    }
}

void KeyboardGraphicsView::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
}

void KeyboardGraphicsView::resizeEvent(QResizeEvent *event) {
    fitAllInView();
    centerOnKey(mCenterOnKey);
    QGraphicsView::resizeEvent(event);
}

int KeyboardGraphicsView::heightForWidth( int w ) const {
    if (mScene.sceneRect().width() == 0) {
        return w * TOTAL_HEIGHT / (88 * KEY_WIDTH);
    }
    return w * mScene.sceneRect().height() / mScene.sceneRect().width();
}

void KeyboardGraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);

    int newSelectedKey = getKeyAtPosition(event->pos());

    if (mMode & MODE_NORMAL) {
        if (newSelectedKey == mSelectedKey) {
            if (mSelectedKeyState == KeyState::STATE_FORCED) {
                deselectKey();
            } else {
                selectKey(newSelectedKey, true);
            }
        } else {
            selectKey(newSelectedKey, false);
        }
    } else if (mMode & MODE_CLICK_RAISES_FULLSCREEN) {
        FullScreenKeyboardDialog dia(this);
        dia.getKeyboardView()->centerOnKey(newSelectedKey);
        dia.exec();
        this->setFocus();
        event->setAccepted(false);
    }
}

void KeyboardGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);

    if (event->buttons() & Qt::LeftButton) {
        int newSelectedKey = getKeyAtPosition(event->pos());
        if (newSelectedKey != mSelectedKey) {
            selectKey(newSelectedKey, false);
        }
    }
}

void KeyboardGraphicsView::keyPressEvent(QKeyEvent *event) {
    QGraphicsView::keyPressEvent(event);

    if (event->key() == Qt::Key_Return) {
        selectKey(mSelectedKey, true);
    }
}

void KeyboardGraphicsView::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_KEY_SELECTION_CHANGED: {
        auto mksc(std::static_pointer_cast<MessageKeySelectionChanged>(m));
        selectKey(mksc->getKeyNumber(), mksc->isForced(), false);
        for (AutoScaledToKeyboardGraphicsView *scaledViews : mAutoScaledGraphicsViews) {
            scaledViews->highlightKey(mksc->getKeyNumber());
        }
        break;}
    case Message::MSG_PRELIMINARY_KEY: {
        auto mpk(std::static_pointer_cast<MessagePreliminaryKey>(m));
        selectPreliminaryKey(mpk->getKeyNumber());
        break;
    }
    case Message::MSG_PROJECT_FILE: {
        auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
        mKeyboard = &mpf->getPiano().getKeyboard();
        changeTotalNumberOfKeys(mpf->getPiano().getKeyboard().getNumberOfKeys(), mpf->getPiano().getKeyboard().getKeyNumberOfA4());
        break;
    }
    case Message::MSG_CLEAR_RECORDING:
        // deselect all
        selectPreliminaryKey(-1);
        selectKey(-1, false);

        // redraw all markers
        redrawAllMarkers();
        break;
    case Message::MSG_KEY_DATA_CHANGED: {
        auto mkdc(std::static_pointer_cast<MessageKeyDataChanged>(m));
        updateColorMarker(mkdc->getIndex());
        setKeyColor(mkdc->getIndex());
        // update key
        break;
    }
    default:
        break;
    }
}


void KeyboardGraphicsView::resetKey(int8_t key0) {
    EptAssert(mKeyboard, "The key data structure has to exist");

    if (mKeyboard->getKeyColor(key0) == Keyboard::Black) {
        // black
        mKeysGraphicsItems[key0]->setBrush(QBrush(Qt::black));
    } else  {
        // white
        mKeysGraphicsItems[key0]->setBrush(QBrush(Qt::white));
    }

    updateColorMarker(key0);
}

void KeyboardGraphicsView::setKeyColor(int8_t key) {
    EptAssert(key >= 0, "Key has to exist");
    EptAssert(mKeyboard, "Keyboard has to be set");
    QColor selectedKeyColor = getKeyColor(mKeyboard->getKeyColor(key) == Keyboard::White,
                                          mSelectedKey == key,
                                          mSelectedKeyState == KeyState::STATE_FORCED,
                                          mKeyboard->at(key).isRecorded());
    QColor preliminaryColor = KEY_PRELIMINARY_COLOR;

    if (mPreliminaryKey == key && mSelectedKey == key) {
        /*QLinearGradient g(0, 0, 5, 5);
        g.setSpread(QGradient::RepeatSpread);
        g.setColorAt(0, selectedKeyColor);
        g.setColorAt(1, preliminaryColor);
        mKeysGraphicsItems[mSelectedKey]->setBrush(QBrush(g));*/
        /*QColor c;
        c.setRed(preliminaryColor.red() * selectedKeyColor.red() / 255);
        c.setGreen(preliminaryColor.green() * selectedKeyColor.green() / 255);
        c.setBlue(preliminaryColor.blue() * selectedKeyColor.blue() / 255);
        mKeysGraphicsItems[mSelectedKey]->setBrush(QBrush(c));*/
        mKeysGraphicsItems[mSelectedKey]->setBrush(QBrush(KEY_PRELIMINARY_AND_SELECTED_COLOR));
    } else if (mPreliminaryKey == key) {
        mKeysGraphicsItems[mPreliminaryKey]->setBrush(QBrush(preliminaryColor));
    } else if (mSelectedKey == key) {
        mKeysGraphicsItems[mSelectedKey]->setBrush(QBrush(selectedKeyColor));
    } else {
        if (mKeyboard->getKeyColor(key) == Keyboard::Black) {
            // black
            mKeysGraphicsItems[key]->setBrush(QBrush(Qt::black));
        } else  {
            // white
            mKeysGraphicsItems[key]->setBrush(QBrush(Qt::white));
        }
    }
}

void KeyboardGraphicsView::selectKey(int8_t key0, bool doubleClick, bool notifyMessageListeners) {
    if (mSelectedKey == key0 && !doubleClick) {
        return;
    }

    if (mSelectedKey >= 0) {
        // reset key and marker!
        resetKey(mSelectedKey);

        // if old key was the preliminary key, mark it as gray
        if (mSelectedKey == mPreliminaryKey) {
            mKeysGraphicsItems[mPreliminaryKey]->setBrush(QBrush(KEY_PRELIMINARY_COLOR));
        }
    }

    mSelectedKey = key0;

    if (notifyMessageListeners) {
        MessageHandler::send<MessageKeySelectionChanged>(key0, mKeyboard->getKeyPtr(key0), doubleClick);
        emit selectionChanged(key0);
    }

    if (mSelectedKey < 0) {
        return;
    }

    mSelectedKeyState = (doubleClick ? KeyState::STATE_FORCED : KeyState::STATE_NORMAL);
    setKeyColor(mSelectedKey);

    updateColorMarker(key0);
}

void KeyboardGraphicsView::selectPreliminaryKey(int8_t key0) {
    if (mPreliminaryKey == key0) {
        return;
    }

    int oldKey = mPreliminaryKey;
    mPreliminaryKey = key0;

    if (oldKey >= 0) {
        setKeyColor(oldKey);
    }

    if (mPreliminaryKey < 0) {
        return;
    }

    setKeyColor(mPreliminaryKey);
}

void KeyboardGraphicsView::redrawAllMarkers() {
    EptAssert(mKeyboard, "The key data structure has to exist");

    for (int8_t i = 0; i < (int8_t)mKeysGraphicsItems.size(); i++) {
        updateColorMarker(i);
    }
}

void KeyboardGraphicsView::updateColorMarker(int8_t key0) {
    EptAssert(mKeyboard, "The key data structure has to exist");

    if (key0 < 0) {
        return;  // an error occurred
    }

    bool recorded(mKeyboard->at(key0).isRecorded());
    mMarkerPixmapItems[key0]->setPixmap(getPixmapForKeyRecordingState(recorded));
}

QPixmap KeyboardGraphicsView::getPixmapForKeyRecordingState(bool recorded) {
    if (!recorded) {
        return QPixmap(":/media/images/status_unchecked.png");
    } else {
        return QPixmap(":/media/images/status_checked.png");
    }
}

QColor KeyboardGraphicsView::getKeyColor(bool isWhite, bool selected, bool doubleClick, bool recorded) {
    if (selected) {
        if (recorded) {
            if (doubleClick) {
                return QColor(KEY_SELECTED_RECORDED_COLOR).darker();
            }
            return QColor(KEY_SELECTED_RECORDED_COLOR);
        } else {
            if (doubleClick) {
                return QColor(KEY_SELECTED_UNRECORDED_COLOR).darker();
            }
            return QColor(KEY_SELECTED_UNRECORDED_COLOR);
        }
    }
    if (isWhite) {
        return QColor(Qt::white);
    }
    return QColor(Qt::black);
}

QRectF KeyboardGraphicsView::keyShape(int8_t key0) {
    double frac = 12.0 / 7.0;  // distribute 7 keys of a total size of 12

    switch ((key0 + 9 + (120 - mKeyboard->getKeyNumberOfA4())) % 12) {
    // white keys
    case 0: return QRectF((key0) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 2: return QRectF((key0 - 2 + 1 * frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 4: return QRectF((key0 - 4 + 2 * frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 5: return QRectF((key0 - 5 + 3 *frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 7: return QRectF((key0 - 7 + 4 * frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 9: return QRectF((key0 - 9 + 5 * frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);
    case 11: return QRectF((key0 - 11 + 6 * frac) * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH * frac, KEY_HEIGHT);

    // black keys
    default:
        return QRectF(key0 * KEY_WIDTH, KEY_SPACING_TO_MARKERS, KEY_WIDTH, KEY_HEIGHT * KEY_BLACK_TO_WHITE_RATIO);
    }
}

void KeyboardGraphicsView::changeTotalNumberOfKeys(int keys, int keyA) {
    if (keys == static_cast<int>(mMarkerPixmapItems.size()) && mKeyNumberOfA == keyA) {
        // no changes, but redraw markers
        redrawAllMarkers();
        return;
    }

    mKeyNumberOfA = keyA;

    mScene.clear();  // clear all elements

    mKeysGraphicsItems.resize(keys);
    mMarkerPixmapItems.resize(keys);

    QFont textFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    textFont.setPixelSize(12);
    textFont.setStyleStrategy(QFont::PreferAntialias);

    for (int8_t i = 0; i < (int8_t)(mMarkerPixmapItems.size()); i++) {
        QPen borderpen(QBrush(Qt::black), PEN_THIN_LINE);
        borderpen.setCosmetic(true);

        // marker pixmaps
        QGraphicsPixmapItem *pixmapItem = mScene.addPixmap(getPixmapForKeyRecordingState(mKeyboard->at(i).isRecorded()));
        pixmapItem->setPos(i * KEY_WIDTH + KEY_MARKERS_MARGIN, 0);
        // 64 is the size of the picture
        pixmapItem->setScale(KEY_WIDTH / 64.0);
        // smooth transformation, nicer, non pixled borders
        pixmapItem->setTransformationMode(Qt::SmoothTransformation);
        mMarkerPixmapItems[i] = pixmapItem;

        // key rects
        QGraphicsRectItem *item = mScene.addRect(keyShape(i));


        item->setPen(borderpen);
        mKeysGraphicsItems[i] = item;



        QGraphicsSimpleTextItem *textItem = mScene.addSimpleText(QString::fromStdString(mKeyboard->getNoteName(i)), textFont);
        textItem->setPos(item->rect().left() + item->rect().width() / 2 - textFont.pixelSize() * 0.55, item->rect().bottom() - 5);
        textItem->setRotation(-90);
        textItem->setZValue(10);
        textItem->setAcceptedMouseButtons(0);

        // colors
        if (mKeyboard->getKeyColor(i) == Keyboard::Black) {
            // black
            item->setBrush(QBrush(Qt::black));
            textItem->setBrush(QBrush(Qt::darkGray));
            item->setZValue(1);  // on top of white keys
        } else {
            // white key
            item->setBrush(QBrush(Qt::white));

            // only A's are black, all others are gray
            if ((i + 1200 - mKeyNumberOfA) % 12 == 0) {
                textItem->setBrush(QBrush(Qt::black));
            } else {
                textItem->setBrush(QBrush(Qt::lightGray));
            }
        }
    }

    // redrawAllMarkers();

    for (auto *view : mAutoScaledGraphicsViews) {
        view->setSceneRect(QRectF(0, 0, KeyboardGraphicsView::KEY_WIDTH * keys, KeyboardGraphicsView::TOTAL_HEIGHT));
    }

    fitAllInView();
    updateGeometry();
}

void KeyboardGraphicsView::fitAllInView() {
    if (mMode & MODE_SCROLLBAR) {
        fitInView(mScene.sceneRect(), Qt::KeepAspectRatioByExpanding);
    } else {
        fitInView(mScene.sceneRect(), Qt::IgnoreAspectRatio);
        for (AutoScaledToKeyboardGraphicsView *view : mAutoScaledGraphicsViews) {
            view->fitInView(mScene.sceneRect(), Qt::IgnoreAspectRatio);
        }
    }
}

int KeyboardGraphicsView::getKeyAtPosition(const QPoint &pos) {
    QGraphicsRectItem *selectedItem(nullptr);

    QList<QGraphicsItem *> itemList(items(pos));
    for (QGraphicsItem *item : itemList) {
        if (item->type() == QGraphicsRectItem::Type) {
            if (!selectedItem || item->zValue() > selectedItem->zValue()) {
                // select top most rect
                selectedItem = dynamic_cast<QGraphicsRectItem*>(item);
            }
        }
    }
    if (!selectedItem) {return -1;}

    for (int8_t i = 0; i < (int8_t)(mKeysGraphicsItems.size()); i++) {
        if (mKeysGraphicsItems[i] == selectedItem) {
            return i;
        }
    }

    return -1;
}

void KeyboardGraphicsView::selectNext() {
    setFocus();
    selectKey(std::min<int>(mSelectedKey + 1, mKeysGraphicsItems.size() - 1), false);
}

void KeyboardGraphicsView::selectPrevious() {
    setFocus();
    selectKey(std::max(0, mSelectedKey - 1), false);
}

void KeyboardGraphicsView::deselectKey() {
    setFocus();
    selectKey(-1, false);
}
