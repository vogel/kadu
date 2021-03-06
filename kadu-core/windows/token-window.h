/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TOKEN_WINDOW_H
#define TOKEN_WINDOW_H

#include <QtWidgets/QDialog>

#include "exports.h"

class QLineEdit;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class TokenWindow
 * @short Window with picture and text field to enter text from picture.
 *
 * This window displays a picture and text field to enter text from picture. When user enters texts or closes window
 * signal @link tokenValueEntered @endlink is emited.
 */
class KADUAPI TokenWindow : public QDialog
{
    Q_OBJECT

    QLineEdit *TokenValue;

    void createGui(const QPixmap &tokenPixmap);
    void result(const QString &value);

public:
    /**
     * @short Create new window with given picture.
     * @param tokenPixmap pixmap to present to user
     */
    explicit TokenWindow(const QPixmap &tokenPixmap, QWidget *parent = nullptr);
    virtual ~TokenWindow();

public slots:
    virtual void accept();
    virtual void reject();

signals:
    /**
     * @short Signal emited when user enters text from picture or closes window.
     * @param tokenValue value entered by user
     *
     * If user closed window without entering value or with Escape key then tokenValue parameter is empty.
     */
    void tokenValueEntered(const QString &tokenValue);
};

/**
 * @}
 */

#endif   // TOKEN_WINDOW_H
