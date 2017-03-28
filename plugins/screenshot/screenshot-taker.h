/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SCREENSHOT_TAKER_H
#define SCREENSHOT_TAKER_H

#include <QtCore/QPointer>
#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QLabel;
class QPushButton;

class ChatWidget;
class IconsManager;

class ScreenshotTaker : public QWidget
{
    Q_OBJECT

    QPointer<IconsManager> m_iconsManager;

    ChatWidget *CurrentChatWidget;

    QLabel *IconLabel;
    QPushButton *CancelButton;

    bool Dragging;

    void createLayout();

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void takeShot();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit ScreenshotTaker(ChatWidget *chatWidget);
    virtual ~ScreenshotTaker();

public slots:
    void takeStandardShot();
    void takeShotWithChatWindowHidden();
    void takeWindowShot();

signals:
    void screenshotTaken(QPixmap screenshot, bool needsCrop);
    void screenshotNotTaken();
};

#endif   // SCREENSHOT_TAKER_H
