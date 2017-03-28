/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QLabel>
#include <injeqt/injeqt.h>

class IconsManager;

class QLabel;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class WaitOverlay
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that shows wait animation over any other widget.
 *
 * This widget is a gray overlay with wait animation over any other widget.
 * To show overlay just create new WaitOverlay with target widget as parent.
 * To remove overlay delete this new widget.
 *
 * This widget is hidden by default. It shows itself after 0.5 seconds of existence
 * so it does not show for very short operations.
 */
class KADUAPI WaitOverlay : public QLabel
{
    Q_OBJECT

public:
    /**
     * @author Rafał 'Vogel' Malinowski
     * @short Creates new WaitOverlay widget.
     * @param parent parent widget
     *
     * This contructor creates WaitOverlay widget and attaches to given parent.
     * Overlay will follow target widget and resize with it.
     */
    explicit WaitOverlay(QWidget *parent = nullptr);
    virtual ~WaitOverlay();

protected:
    virtual bool eventFilter(QObject *object, QEvent *event);

private:
    QPointer<IconsManager> m_iconsManager;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void timeoutPassed();
};

/**
 * @}
 */
