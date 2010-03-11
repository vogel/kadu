/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_PROGRESS_WINDOW_H
#define SMS_PROGRESS_WINDOW_H

#include <QtGui/QWidget>

class QLabel;
class QMovie;
class QPushButton;

class SmsSender;

class SmsProgressWindow : public QWidget
{
	Q_OBJECT

	QLabel *IconLabel;
	QMovie *WaitMovie;
	QLabel *MessageLabel;
	QPushButton *CloseButton;

	SmsSender *Sender;

	void createGui();

public:
	explicit SmsProgressWindow(SmsSender *sender, QWidget *parent = 0);
	virtual ~SmsProgressWindow();

};

#endif // SMS_PROGRESS_WINDOW_H
