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
	explicit HistoryTalkableComboBox(QWidget *parent = 0);
	virtual ~HistoryTalkableComboBox();

	void setAllLabel(const QString &allLabel);
	void setTalkables(const QVector<Talkable> &talkables);
	void setFutureTalkables(const QFuture<QVector<Talkable> > &talkables);

};

#endif // HISTORY_TALKABLE_COMBO_BOX_H
