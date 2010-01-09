/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"

#include "open-chat-with.h"
#include "open-chat-with-contact-list-runner.h"
#include "open-chat-with-runner-manager.h"


OpenChatWith::OpenChatWith(QWidget *parent)
	: QWidget(parent, Qt::Window)
{
	kdebugf();
	setWindowTitle(tr("Open chat with..."));
	setAttribute(Qt::WA_DeleteOnClose);
	
	MainLayout = new QVBoxLayout(this);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);
	
	ContactID = new QLineEdit(this);
	connect(ContactID, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged(const QString &)));
	MainLayout->addWidget(ContactID);
	
	BuddiesWidget = new BuddiesListView(0); // TODO: 0.6.6 fix that one
	//ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	connect(BuddiesWidget, SIGNAL(chatActivated(Chat)), this, SLOT(openChat()));
	MainLayout->addWidget(BuddiesWidget);
	
	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);

	QPushButton *b_cancel = new QPushButton(tr("&Cancel"));
	connect(b_cancel, SIGNAL(clicked()), this, SLOT(close()));
	QPushButton *b_ok = new QPushButton(tr("&OK"));
	connect(b_ok, SIGNAL(clicked()), this, SLOT(inputAccepted()));

	buttons_layout->setContentsMargins(0, 5, 0, 0);
	buttons_layout->setAlignment(Qt::AlignRight);
	buttons_layout->addWidget(b_ok);
	buttons_layout->addWidget(b_cancel);

	MainLayout->addWidget(buttons);
	
	OpenChatRunner = new OpenChatWithContactListRunner();
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
	kdebugf2();
}

OpenChatWith::~OpenChatWith()
{
	//saveWindowGeometry(this, "General", "OpenChatWith");
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
}

void OpenChatWith::keyPressEvent(QKeyEvent *e)
{
	kdebugf();
	switch (e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return: inputAccepted(); break;
		case Qt::Key_Escape: close(); break;
	}
	kdebugf2();
}

void OpenChatWith::inputChanged(const QString &text)
{
	kdebugf();
	BuddyList matchingContacts;
	if (!text.isEmpty())
		matchingContacts = OpenChatWithRunnerManager::instance()->matchingContacts(text);
	BuddiesWidget->setModel(new BuddyListModel(matchingContacts, this));
	kdebugf2();
}

void OpenChatWith::openChat()
{
	BuddiesListView *widget = dynamic_cast<BuddiesListView *>(sender());
	if (!widget)
		return;

	ContactSet contacts = widget->selectedContacts();
	foreach (Contact contact, contacts)
		ContactManager::instance()->addItem(contact);

	BuddySet buddies = contacts.toBuddySet();

	if (!buddies.isEmpty() && !buddies.contains(Core::instance()->myself()))
	{
		Chat chat = ChatManager::instance()->findChat(contacts);
		if (chat)
		{
			ChatWidgetManager::instance()->sendMessage(chat);
			close();
			return;
		}
	}

	Buddy buddy = *buddies.begin();
	if (buddy.mobile().isEmpty() && !buddy.email().isEmpty())
		openMailClient(buddy.email());

	close();
}

void OpenChatWith::inputAccepted()
{
	openChat();
}
