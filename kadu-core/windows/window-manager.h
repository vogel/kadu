/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>

class QPoint;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class WindowManager
 * @short Helper class to manage windows.
 *
 * This helper singleton is designed to help manager windows sizes and positions.
 */
class WindowManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE WindowManager(QObject *parent = nullptr);
    virtual ~WindowManager();

    /**
     * @short Move window to workspace center.
     * @param window window to move
     */
    void moveToWorkspaceCenter(QWidget *window);

    /**
     * @short Move window to specific position.
     * @param window window to move
     * @param position desctination position for given window's top-left corner
     *
     * This method uses ensureWholeVisible() to move window inside workspace area.
     */
    void moveToPosition(QWidget *window, const QPoint &position);

    /**
     * @short Ensure that whole window is visible on screen.
     * @param window window
     *
     * This method moves given window inside workspace area if it is not visible.
     */
    void ensureWholeVisible(QWidget *window);
};

/**
 * @}
 */
