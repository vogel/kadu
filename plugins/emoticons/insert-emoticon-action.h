/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef INSERT_EMOTICON_ACTION_H
#define INSERT_EMOTICON_ACTION_H

#include "gui/actions/action-description.h"

#include "emoticon.h"

class InsertEmoticonAction : public ActionDescription
{
	Q_OBJECT

	QVector<Emoticon> Emoticons;

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void configurationUpdated();

public:
	explicit InsertEmoticonAction(QObject *parent = 0);
	virtual ~InsertEmoticonAction();

	virtual void actionTriggered(QAction *sender, bool toggled);

	void setEmoticons(const QVector<Emoticon> &emoticons);

};


#endif // INSERT_EMOTICON_ACTION_H
