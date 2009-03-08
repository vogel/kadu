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
#include "accounts/account_manager.h"
#include "chat/chat_manager.h"
#include "contacts/model/contact-list-model.h"
#include "contacts/contact-manager.h"

#include "debug.h"
#include "kadu.h"
#include "misc.h"
#include "xml_config_file.h"

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
	
	ContactsWidget = new ContactsListWidget(kadu);
	//ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(openChat(Contact)));
	MainLayout->addWidget(ContactsWidget);
	
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
	ContactList matchingContacts;
	if (!text.isEmpty())
		matchingContacts = OpenChatWithRunnerManager::instance()->matchingContacts(text);
	ContactsWidget->setModel(new ContactListModel(matchingContacts, this));
	kdebugf2();
}

void OpenChatWith::openChat(Contact contact)
{
	ContactsListWidget *widget = dynamic_cast<ContactsListWidget *>(sender());
	if (!widget)
		return;

	ContactList contacts = widget->selectedContacts();

	if (!contacts.isEmpty())
	{
		Contact contact = contacts[0];

		if (contacts[0] != kadu->myself()) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(contact, contacts);
		else if (contact.mobile().isEmpty() && !contact.email().isEmpty())
			openMailClient(contact.email());
		close();
	}
}

void OpenChatWith::inputAccepted()
{
	kdebugf();
	ContactList contacts = ContactsWidget->selectedContacts();
	if (!contacts.isEmpty())
	{
		chat_manager->openPendingMsgs(contacts, true);
		close();
	}
	kdebugf2();
}
