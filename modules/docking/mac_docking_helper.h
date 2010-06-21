/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef MAC_DOCKING_HELPER_H
#define MAC_DOCKING_HELPER_H

#include <QtCore/QObject>

#include "Carbon/Carbon.h"

class MacDockingHelper : public QObject
{
	Q_OBJECT

private:
	bool isBouncing;
	bool isOverlayed;
	NMRec bounceRec;
	static MacDockingHelper *Instance;

	MacDockingHelper(QObject *parent = 0);
	~MacDockingHelper();

public:
	static MacDockingHelper *instance()
		{
			if (!Instance)
				Instance = new MacDockingHelper();
			return Instance; 
		};
	void startBounce();
	void stopBounce();
	void removeOverlay();
	void overlay(const QString& text);
};

#endif // MAC_DOCKING_HELPER_H
