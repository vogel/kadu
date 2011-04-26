/*
 * Copyright (C) 2006  Remko Troncon
 *
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

#ifndef PEP_GET_TASK_H
#define PEP_GET_TASK_H

#include <QtXml/QDomElement>

#include "iris/xmpp_task.h"

namespace XMPP
{
	class PubSubItem;
}

class PEPGetTask : public XMPP::Task
{
	Q_OBJECT

	QDomElement iq_;
	QString jid_;
	QString node_;
	QList<XMPP::PubSubItem> items_;

public:
	PEPGetTask(Task *parent, const QString &jid, const QString &node, const QString &itemID);
	virtual ~PEPGetTask();

	void onGo();
	bool take(const QDomElement &x);

	const QList<XMPP::PubSubItem> & items() const;
	const QString & jid() const;
	const QString & node() const;

};

#endif // PEP_GET_TASK_H
