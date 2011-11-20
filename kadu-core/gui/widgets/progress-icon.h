/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROGRESS_ICON_H
#define PROGRESS_ICON_H

#include <QtGui/QLabel>

class QMovie;

class ProgressIcon : public QLabel
{
	Q_OBJECT

public:
	enum ProgressState {
		StateInProgress,
		StateFinished,
		StateFailed
	};

private:
	QMovie *ProgressMovie;
	ProgressState State;

public:
	explicit ProgressIcon(QWidget *parent = 0);
	virtual ~ProgressIcon();

	void setState(ProgressState state);

};

#endif // PROGRESS_ICON_H
