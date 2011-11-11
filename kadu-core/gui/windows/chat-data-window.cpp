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

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "chat/chat-manager.h"
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

	connect(ChatManager::instance(), SIGNAL(chatRemoved(Buddy&)),
			this, SLOT(chatRemoved(Buddy&)));
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
	layout->addWidget(nameWidget);

	QHBoxLayout *nameLayout = new QHBoxLayout(nameWidget);

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ':', nameWidget);
	nameLayout->addWidget(numberLabel);

	DisplayEdit = new QLineEdit(nameWidget);
	DisplayEdit->setText(MyChat.display());
	nameLayout->addWidget(DisplayEdit);

	QLabel *groupsLabel = new QLabel(tr("Add this chat to the groups below by checking the box next to the appropriate groups."), this);
	groupsLabel->setWordWrap(true);

	layout->addWidget(groupsLabel);

	QScrollArea *groups = new QScrollArea(this);
	layout->addWidget(groups);

	groups->setFrameShape(QFrame::NoFrame);
	groups->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	groups->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QVBoxLayout *groupsLayout = new QVBoxLayout(groups);

	foreach (const Group &group, GroupManager::instance()->items())
	{
		QCheckBox *groupCheckBox = new QCheckBox(group.name(), groups);
		groupCheckBox->setChecked(MyChat.isInGroup(group));
		groupsLayout->addWidget(groupCheckBox);
		GroupCheckBoxList.append(groupCheckBox);
	}

	layout->addStretch(1);

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
	foreach (const Group &group, MyChat.groups())
		MyChat.removeFromGroup(group);

	foreach (QCheckBox *groupBox, GroupCheckBoxList)
		if (groupBox->isChecked())
			MyChat.addToGroup(GroupManager::instance()->byName(groupBox->text()));

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
