/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * %kadu copyright end%
 *
 * Copyright 2011 Adam "Vertex" Makświej (vertexbz@gmail.com)
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

#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
#else
typedef int NSInteger;
#endif

struct MacDockingHelperStruct;
class MacDockingHelper : public QObject
{
	Q_OBJECT

private:
	static MacDockingHelper *Instance;
	NSInteger currentAttentionRequest;
	bool isBouncing;
	MacDockingHelperStruct *d;
	MacDockingHelper(QObject *parent = 0);
	~MacDockingHelper();
	void showMinimizedChats();

public:
	static MacDockingHelper *instance()
	{
		if (!Instance)
			Instance = new MacDockingHelper();
		return Instance;
	};
	static void destroyInstance()
	{
		if (Instance)
		{
			delete Instance;
			Instance = 0;
		}
	};

	void startBounce();
	void stopBounce();
	void removeOverlay();
	void overlay(const NSInteger count);
};

#endif // MAC_DOCKING_HELPER_H
