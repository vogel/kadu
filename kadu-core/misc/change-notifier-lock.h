/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHANGE_NOTIFIER_LOCK_H
#define CHANGE_NOTIFIER_LOCK_H

#include "exports.h"

class ChangeNotifier;

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @class ChangeNotifierLock
 * @author Rafał 'Vogel' Malinowski
 * @short Lock ChangeNotifier until end of scope.
 */
class KADUAPI ChangeNotifierLock
{

public:
	enum Mode
	{
		ModeNotify,
		ModeForget
	};

	explicit ChangeNotifierLock(ChangeNotifier &notifier, Mode mode = ModeNotify);
	~ChangeNotifierLock();

	void setMode(Mode mode);

private:
	ChangeNotifier &m_notifier;
	Mode m_notifierMode;

};

/**
 * @}
 */

#endif // CHANGE_NOTIFIER_LOCK_H
