/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QChar>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QKeyEvent>
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
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/line-edit-with-clear-button.h"
#include "misc/misc.h"
#include "os/generic/url-opener.h"

#include "activate.h"
#include "debug.h"

#include "open-chat-with-contact-list-runner.h"
#include "open-chat-with-runner-manager.h"

#include "open-chat-with.h"

OpenChatWith *OpenChatWith::Instance = 0;

OpenChatWith * OpenChatWith::instance()
{
	if (!Instance)
		Instance = new OpenChatWith();

	return Instance;
}

OpenChatWith::OpenChatWith() :
	QWidget(0, Qt::Window), DesktopAwareObject(this), IsTyping(false), ListModel(0)
{
	kdebugf();

	setWindowRole("kadu-open-chat-with");

	setWindowTitle(tr("Open chat with..."));
	setAttribute(Qt::WA_DeleteOnClose);
	
	int width = QDesktopWidget().availableGeometry().width()*0.25;
	int height = QDesktopWidget().availableGeometry().height()*0.3;
	QRect rect(QDesktopWidget().availableGeometry().center().x()-width/2, QDesktopWidget().availableGeometry().center().y()-height/2, width, height);
	setWindowGeometry(this, rect);

	MainLayout = new QVBoxLayout(this);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	ContactID = new LineEditWithClearButton(this);
	ContactID->installEventFilter(this);
	connect(ContactID, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged(const QString &)));
	MainLayout->addWidget(ContactID);

	BuddiesWidget = new BuddiesListView(this);
	connect(BuddiesWidget, SIGNAL(chatActivated(Chat)), this, SLOT(openChat()));
	MainLayout->addWidget(BuddiesWidget);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Ok"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(inputAccepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	MainLayout->addWidget(buttons);

	OpenChatRunner = new OpenChatWithContactListRunner();
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
	kdebugf2();
}

OpenChatWith::~OpenChatWith()
{
	//saveWindowGeometry(this, "General", "OpenChatWith");
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	Instance = 0;

	delete OpenChatRunner;
	OpenChatRunner = 0;

	delete ListModel;
	ListModel = 0;
}

bool OpenChatWith::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == ContactID && e->type() == QEvent::KeyPress)
	{
		if (static_cast<QKeyEvent *>(e)->modifiers() == Qt::NoModifier)
		{
			int key = static_cast<QKeyEvent *>(e)->key();
			if (key == Qt::Key_Down ||
					key == Qt::Key_Up ||
					key == Qt::Key_PageDown ||
					key == Qt::Key_PageUp ||
					(key == Qt::Key_Left && ContactID->cursorPosition() == 0) ||
					(key == Qt::Key_Right && ContactID->cursorPosition() == ContactID->text().length()))
			{
				qApp->sendEvent(BuddiesWidget, e);
				return true;
			}
		}
	}

	return false;
}

void OpenChatWith::keyPressEvent(QKeyEvent *e)
{
	kdebugf();

	switch (e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			e->accept();
			inputAccepted();
			return;
			break;
		case Qt::Key_Escape:
			e->accept();
			close();
			return;
			break;
	}

	if (BuddiesListView::shouldEventGoToFilter(e))
	{
		ContactID->setText(e->text());
		ContactID->setFocus(Qt::OtherFocusReason);
		e->accept();
		return;
	}

	QWidget::keyPressEvent(e);

	kdebugf2();
}

void OpenChatWith::inputChanged(const QString &text)
{
	kdebugf();

	BuddyList matchingContacts;
	if (!text.isEmpty())
		matchingContacts = OpenChatWithRunnerManager::instance()->matchingContacts(text);

	delete ListModel;
	ListModel = new BuddyListModel(matchingContacts, this);
	BuddiesWidget->setModel(ListModel);

	if (!text.isEmpty())
	{
		if (!IsTyping || BuddiesWidget->selectionModel()->selectedIndexes().count() == 0)
		{
			BuddiesWidget->setCurrentIndex(BuddiesWidget->model()->index(0, 0));
			BuddiesWidget->selectionModel()->select(BuddiesWidget->model()->index(0, 0), QItemSelectionModel::SelectCurrent);
		}
		IsTyping = true;
	}
	else
		IsTyping = false;

	kdebugf2();
}

void OpenChatWith::openChat()
{
	ContactSet contacts = BuddiesWidget->selectedContacts();

	if (contacts.isEmpty())
	{
		close();
		return;
	}

	// In case a contact was added to the manager after BuddiesWidget was created,
	// ensure that we don't add actually duplicate contacts to the manager.
	ContactSet knownContacts;
	for (ContactSet::iterator it = contacts.begin(), end = contacts.end(); it != end; )
	{
		Contact knownContact = ContactManager::instance()->byId(it->contactAccount(), it->id(), ActionReturnNull);
		if (knownContact)
		{
			it = contacts.erase(it);
			knownContacts.insert(knownContact);
		}
		else
		{
			ContactManager::instance()->addItem(knownContact);
			++it;
		}
	}

	contacts.unite(knownContacts);

	BuddySet buddies = contacts.toBuddySet();

	Chat chat = ChatManager::instance()->findChat(contacts);
	if (chat)
	{
		ChatWidgetManager::instance()->openPendingMessages(chat, true);
		close();
		return;
	}

	Buddy buddy = *buddies.constBegin();
	if (buddy.mobile().isEmpty() && !buddy.email().isEmpty())
		UrlOpener::openEmail(buddy.email().toUtf8());

	close();
}

void OpenChatWith::inputAccepted()
{
	openChat();
}

void OpenChatWith::show()
{
  	if (!isVisible())
		QWidget::show();
	else
		_activateWindow(this);
}
