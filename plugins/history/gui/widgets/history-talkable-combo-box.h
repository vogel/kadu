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

#ifndef HISTORY_TALKABLE_COMBO_BOX_H
#define HISTORY_TALKABLE_COMBO_BOX_H

#include <QtCore/QFuture>

#include "gui/widgets/select-talkable-combo-box.h"

class QAction;

class BuddyListModel;
class ChatsListModel;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryTalkableComboBox
 * @author Rafał 'Vogel' Malinowski
 * @short Version of talkable combo box with support for "All talkables" and future item list.
 *
 * This widgets can be used to select one of talkable with list or "All talkables" item.
 * Talkables can be provided as QFuture value that will be resolved and updated asynchronously.
 *
 * List of Talkables will be split into chats and buddies (buddies will be extracted from one-buddy chats).
 */
class HistoryTalkableComboBox : public SelectTalkableComboBox
{
	Q_OBJECT

	QFutureWatcher<QVector<Talkable> > *TalkablesFutureWatcher;

	QAction *AllAction;
	ChatsListModel *ChatsModel;
	BuddyListModel *BuddiesModel;

private slots:
	void futureTalkablesAvailable();
	void futureTalkablesCanceled();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new HistoryTalkableComboBox.
	 * @param parent parent widget
	 */
	explicit HistoryTalkableComboBox(QWidget *parent = 0);
	virtual ~HistoryTalkableComboBox();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set label for "All talkables" item.
	 * @param allLabel new label for "All talkables" item
	 *
	 * Default value of this label is empty.
	 */
	void setAllLabel(const QString &allLabel);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set list of talkables to display below "All talkables" item.
	 * @param talkables list of talkables to display below "All talkables" item
	 *
	 * This list will be split into chats and buddies before displaying. Buddies will be extracted
	 * from single-buddy chats.
	 */
	void setTalkables(const QVector<Talkable> &talkables);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set list of future talkables to display below "All talkables" item.
	 * @param talkables list of future talkables to display below "All talkables" item
	 *
	 * Widget will be populated when future value will become available. Thenhis list will be split
	 * into chats and buddies before displaying. Buddies will be extracted from single-buddy chats.
	 */
	void setFutureTalkables(const QFuture<QVector<Talkable> > &talkables);

};

/**
 * @}
 */

#endif // HISTORY_TALKABLE_COMBO_BOX_H
