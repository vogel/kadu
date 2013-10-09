/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ACTION_CONTEXT_PROVIDER_H
#define ACTION_CONTEXT_PROVIDER_H

class ActionContext;

/**
 * @addtogroup Actions
 * @{
 */

/**
 * @class ActionContextProvider
 * @author Rafał 'Vogel' Malinowski
 * @short Interface used by classes that have access to ActionContext instances.
 *
 * This inteface is implemented by all classes/widgets that have access to ActionContext instances.
 * For example, all main windows must implement this interface.
 */
class ActionContextProvider
{
public:
	virtual ~ActionContextProvider() {}

	virtual ActionContext * actionContext() = 0;

};

/**
 * @}
 */

#endif // ACTION_CONTEXT_PROVIDER_H
