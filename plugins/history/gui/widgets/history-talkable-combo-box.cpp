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

#include <QtCore/QFutureWatcher>
#include <QtGui/QAction>

#include "buddies/model/buddy-list-model.h"
#include "chat/model/chat-list-model.h"
#include "model/action-list-model.h"
#include "model/merged-proxy-model-factory.h"
#include "chats-buddies-splitter.h"

#include "history-talkable-combo-box.h"

HistoryTalkableComboBox::HistoryTalkableComboBox(QWidget *parent) :
		SelectTalkableComboBox(parent), TalkablesFutureWatcher(0)
{
	setShowAnonymous(true);

	ActionListModel *actionModel = new ActionListModel(this);
	AllAction = new QAction(this);
	actionModel->appendAction(AllAction);

	ChatsModel = new ChatListModel(this);
	BuddiesModel = new BuddyListModel(this);

	QList<KaduAbstractModel *> models;
	models.append(actionModel);
	models.append(ChatsModel);
	models.append(BuddiesModel);

	setBaseModel(MergedProxyModelFactory::createKaduModelInstance(models, this));
}

HistoryTalkableComboBox::~HistoryTalkableComboBox()
{
}

void HistoryTalkableComboBox::setAllLabel(const QString &allLabel)
{
	AllAction->setText(allLabel);
}

void HistoryTalkableComboBox::setTalkables(const QVector<Talkable> &talkables)
{
	futureTalkablesCanceled();

	ChatsBuddiesSplitter chatsBuddies(talkables);

	ChatsModel->setChats(chatsBuddies.chats().toList().toVector());
	BuddiesModel->setBuddyList(chatsBuddies.buddies().toList());
}

void HistoryTalkableComboBox::setFutureTalkables(const QFuture<QVector<Talkable> > &talkables)
{
	if (TalkablesFutureWatcher)
		delete TalkablesFutureWatcher;

	ChatsModel->setChats(QVector<Chat>());
	BuddiesModel->setBuddyList(BuddyList());

	TalkablesFutureWatcher = new QFutureWatcher<QVector<Talkable> >(this);
	connect(TalkablesFutureWatcher, SIGNAL(finished()), this, SLOT(futureTalkablesAvailable()));
	connect(TalkablesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureTalkablesCanceled()));
	TalkablesFutureWatcher->setFuture(talkables);
}

void HistoryTalkableComboBox::futureTalkablesAvailable()
{
	if (TalkablesFutureWatcher)
		setTalkables(TalkablesFutureWatcher->result());
}

void HistoryTalkableComboBox::futureTalkablesCanceled()
{
	if (!TalkablesFutureWatcher)
		return;

	TalkablesFutureWatcher->deleteLater();
	TalkablesFutureWatcher = 0;
}
