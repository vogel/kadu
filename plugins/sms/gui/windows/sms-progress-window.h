/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include "gui/windows/progress-window.h"

class SmsSender;

class SmsProgressWindow : public ProgressWindow
{
	Q_OBJECT

	SmsSender *Sender;

	void createGui();

private slots:
	void sendingFailed(const QString &errorMessage);
	void sendingSucceed(const QString &message);

public:
	explicit SmsProgressWindow(SmsSender *sender, QWidget *parent = 0);
	virtual ~SmsProgressWindow();

};

#endif // SMS_PROGRESS_WINDOW_H
