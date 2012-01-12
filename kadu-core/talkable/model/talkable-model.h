/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TALKABLE_MODEL_H
#define TALKABLE_MODEL_H

#include "model/kadu-merged-proxy-model.h"

class BuddiesModel;
class ChatsModel;

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class TalkableModel
 * @author Rafał 'Vogel' Malinowski
 * @short Proxy model than merges BuddiesModel and ChatsModel.
 *
 * This proxy model merges BuddiesModel and ChatsModel and displays registered instances of Buddy and Chat classes.
 * Chat instances are displayed before Buddy ones.
 */
class TalkableModel : public KaduMergedProxyModel
{
	Q_OBJECT

	ChatsModel *Chats;
	BuddiesModel *Buddies;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of TalkableModel with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit TalkableModel(QObject *parent = 0);
	virtual ~TalkableModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Change value of includeMyself property.
	 * @param includeMyself new value of includeMyself property
	 *
	 * Change value of includeMyself property. If includeMyself is true, then Myself buddy will be
	 * available in this model.
	 */
	void setIncludeMyself(bool includeMyself);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return value of includeMyself property.
	 * @return value of includeMyself property
	 *
	 * Returns value of includeMyself property. If includeMyself is true, then Myself buddy will be
	 * available in this model.
	 */
	bool includeMyself() const;

};

/**
 * @}
 */

#endif // TALKABLE_MODEL_H
