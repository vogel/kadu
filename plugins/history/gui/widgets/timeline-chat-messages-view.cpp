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

#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/chat-messages-view.h"
#include "model/roles.h"

#include "timeline-chat-messages-view.h"

TimelineChatMessagesView::TimelineChatMessagesView(QWidget *parent) :
		QWidget(parent)
{
	setLayout(new QVBoxLayout(this));

	createGui();
}

TimelineChatMessagesView::~TimelineChatMessagesView()
{
}

void TimelineChatMessagesView::createGui()
{
	Splitter = new QSplitter(Qt::Vertical, this);

	Timeline = new QTreeView(Splitter);

	Timeline->setAlternatingRowColors(true);
	Timeline->setRootIsDecorated(false);
	Timeline->setUniformRowHeights(true);

	QFrame *frame = new QFrame(Splitter);
	frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	QVBoxLayout *frameLayout = new QVBoxLayout(frame);
	frameLayout->setMargin(0);

	MessagesView = new ChatMessagesView(Chat::null, false, frame);
	MessagesView->setFocusPolicy(Qt::StrongFocus);
	MessagesView->setForcePruneDisabled(true);

	frameLayout->addWidget(MessagesView);

	layout()->addWidget(Splitter);
}

QDate TimelineChatMessagesView::currentDate() const
{
	return Timeline->currentIndex().data(DateRole).value<QDate>();
}
