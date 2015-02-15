/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SHOW_HISTORY_ACTION_DESCRIPTION_H
#define SHOW_HISTORY_ACTION_DESCRIPTION_H

#include <QtWidgets/QAction>

#include "history_exports.h"

#include "gui/actions/action-description.h"

class HISTORYAPI ShowHistoryActionDescription : public ActionDescription
{
	Q_OBJECT

	int ChatHistoryQuotationTime;

	void showDaysMessages(QAction *action, int days);

private slots:
	void showPruneMessages();
	void showOneDayMessages();
	void show7DaysMessages();
	void show30DaysMessages();
	void showAllMessages();

protected:
	virtual void configurationUpdated();

	virtual void actionInstanceCreated(Action *action);
	virtual void actionTriggered(QAction *sender, bool toggled);

public:
	explicit ShowHistoryActionDescription(QObject *parent);
	virtual ~ShowHistoryActionDescription();

};

#endif // SHOW_HISTORY_ACTION_DESCRIPTION_H
