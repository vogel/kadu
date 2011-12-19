/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "buddies/group.h"
#include "chat/chat-manager.h"
#include "gui/widgets/group-list.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "activate.h"

#include "chat-data-window.h"

QMap<Chat, ChatDataWindow *> ChatDataWindow::Instances;

ChatDataWindow * ChatDataWindow::instance(const Chat &chat, QWidget *parent)
{
	if (Instances.contains(chat))
		// TODO: it might be useful someday to reparent in case the new parent is different than the old
		return Instances.value(chat);
	else
		return new ChatDataWindow(chat, parent);
}

ChatDataWindow::ChatDataWindow(const Chat &chat, QWidget *parent) :
		QWidget(parent, Qt::Dialog), MyChat(chat)
{
	Instances.insert(MyChat, this);

	setWindowRole("kadu-chat-data");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Chat Properties - %1").arg(MyChat.display()));

	createGui();
	updateButtons();

	loadWindowGeometry(this, "General", "ChatDataWindowGeometry", 0, 50, 425, 500);

	connect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)));
}

ChatDataWindow::~ChatDataWindow()
{
	Instances.remove(MyChat);

	saveWindowGeometry(this, "General", "ChatDataWindowGeometry");
}

void ChatDataWindow::show()
{
	QWidget::show();

	_activateWindow(this);
}

void ChatDataWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *nameWidget = new QWidget(this);

	QHBoxLayout *nameLayout = new QHBoxLayout(nameWidget);

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ':', nameWidget);

	DisplayEdit = new QLineEdit(nameWidget);
	DisplayEdit->setText(MyChat.display());

	nameLayout->addWidget(numberLabel);
	nameLayout->addWidget(DisplayEdit);

	QLabel *groupsLabel = new QLabel(tr("Add this chat to the groups below by checking the box next to the appropriate groups."), this);
	groupsLabel->setWordWrap(true);

	ChatGroupList = new GroupList(this);
	ChatGroupList->setCheckedGroups(MyChat.groups());

	layout->addWidget(nameWidget);
	layout->addWidget(groupsLabel);
	layout->addWidget(ChatGroupList);

	createButtons(layout);

	connect(DisplayEdit, SIGNAL(textChanged(QString)), this, SLOT(updateButtons()));
}

void ChatDataWindow::createButtons(QLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	OkButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("OK"), this);
	buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
	ApplyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	buttons->addButton(ApplyButton, QDialogButtonBox::ApplyRole);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(updateChatAndClose()));
	connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(updateChat()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);
}

void ChatDataWindow::updateChat()
{
	MyChat.blockUpdatedSignal();

	MyChat.setDisplay(DisplayEdit->text());
	MyChat.setGroups(ChatGroupList->checkedGroups());

	MyChat.unblockUpdatedSignal();
}

void ChatDataWindow::updateChatAndClose()
{
	updateChat();
	close();
}

void ChatDataWindow::chatRemoved(const Chat &chat)
{
	if (chat == MyChat)
		close();
}

void ChatDataWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}

bool ChatDataWindow::isValid()
{
	const Chat &chat = ChatManager::instance()->byDisplay(DisplayEdit->text());
	if (!chat)
		return true;

	return chat == MyChat;
}

void ChatDataWindow::updateButtons()
{
	bool valid = isValid();
	OkButton->setEnabled(valid);
	ApplyButton->setEnabled(valid);
}
