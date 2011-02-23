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

#ifndef PEP_RETRACT_TASK_H
#define PEP_RETRACT_TASK_H

#include <QtXml/QDomElement>

#include "iris/xmpp_task.h"

class PEPRetractTask : public XMPP::Task
{
	Q_OBJECT

	QDomElement iq_;
	QString node_;
	QString itemId_;

public:
	PEPRetractTask(Task *parent, const QString &node, const QString &itemId);
	virtual ~PEPRetractTask();

	bool take(const QDomElement &x);
	void onGo();

	const QString & node() const;

};

#endif // PEP_RETRACT_TASK_H
