/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DESKTOP_AWARE_OBJECT_HELPER
#define DESKTOP_AWARE_OBJECT_HELPER

#include <QtCore/QObject>
#include <QtCore/QTimer>

class DesktopAwareObjectHelper : public QObject
{
	Q_OBJECT

	QTimer Timer;

protected slots:
	void workAreaResized();

public:
	DesktopAwareObjectHelper();
	~DesktopAwareObjectHelper();

};

#endif // DESKTOP_AWARE_OBJECT_HELPER
