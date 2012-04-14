/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QStringList>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHostAddress>

#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "gui/actions/base-action-context.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "debug.h"

#include "infos.h"

#include "infos_dialog.h"

InfosDialog::InfosDialog(const LastSeen &lastSeen, QWidget *parent) :
	QDialog(parent)
{
	kdebugf();
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Buddies Information"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	ListView = new QTreeWidget(this);
	ListView->setAllColumnsShowFocus(true);
	ListView->setColumnCount(9);
	ListView->setContextMenuPolicy(Qt::CustomContextMenu);
	ListView->setRootIsDecorated(false);
	ListView->setSelectionMode(QAbstractItemView::SingleSelection);
	ListView->setSortingEnabled(true);

	connect(ListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));

	QStringList labels;
	labels << tr("Buddy")
			<< tr("Protocol")
			<< tr("Username")
			<< tr("Nick")
			<< tr("IP")
			<< tr("Domain name")
			<< tr("Description")
			<< tr("State")
			<< tr("Last time seen on");
	ListView->setHeaderLabels(labels);

	foreach (const Contact &contact, ContactManager::instance()->items())
	{
		if (contact.isAnonymous())
			continue;

		QString desc, ip;
		if (!contact.currentStatus().description().isEmpty())
			desc = contact.currentStatus().description();
		desc.replace('\n', ' ');
		if (!contact.address().isNull())
			ip = contact.address().toString();

		QStringList labels;
		labels << contact.display(true)
				<< contact.contactAccount().protocolName()
				<< contact.id()
				<< contact.ownerBuddy().nickName()
				<< ip
				<< contact.dnsName()
				<< desc
				<< StatusTypeManager::instance()->statusTypeData(contact.currentStatus().type()).name()
				<< lastSeen[qMakePair(contact.contactAccount().protocolName(), contact.id())];

		QTreeWidgetItem *item = new QTreeWidgetItem(labels);
		item->setData(0, ContactRole, contact);
		ListView->addTopLevelItem(item);
	}
	ListView->sortItems(0, Qt::AscendingOrder);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("&Close"), this);

	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);

	layout->addWidget(ListView);
	layout->addSpacing(16);
	layout->addWidget(buttons);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	loadWindowGeometry(this, "LastSeen", "LastSeenWidgetGeometry", 0, 0, 800, 300);

	kdebugf2();
}

InfosDialog::~InfosDialog()
{
	kdebugf();

	saveWindowGeometry(this, "LastSeen", "LastSeenWidgetGeometry");

	kdebugf2();
}

void InfosDialog::customContextMenuRequested(const QPoint &point)
{
	Q_UNUSED(point);

	QList<QTreeWidgetItem *> selectedItems = ListView->selectedItems();
	if (1 != selectedItems.count())
		return;

	QTreeWidgetItem *selectedItem = selectedItems.at(0);
	if (!selectedItem)
		return;

	Contact contact = selectedItem->data(0, ContactRole).value<Contact>();
	if (!contact)
		return;

	BaseActionContext actionContext;
	actionContext.setBuddies(BuddySet(contact.ownerBuddy()));
	actionContext.setChat(ChatTypeContact::findChat(contact, ActionCreateAndAdd));
	actionContext.setContacts(ContactSet(contact));
	actionContext.setRoles(RoleSet() << ContactRole);

	QScopedPointer<QMenu> menu(TalkableMenuManager::instance()->menu(this, &actionContext));
	menu->exec(QCursor::pos());
}
