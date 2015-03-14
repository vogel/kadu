/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

#include "chat/type/chat-type-contact.h"
#include "configuration/config-file-variant-wrapper.h"
#include "contacts/contact-manager.h"
#include "gui/actions/base-action-context.h"
#include "gui/menu/menu-inventory.h"
#include "model/roles.h"
#include "os/generic/window-geometry-manager.h"
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
	ListView->setColumnCount(7);
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
			<< tr("Description")
			<< tr("State")
			<< tr("Last time seen on");
	ListView->setHeaderLabels(labels);

	foreach (const Contact &contact, ContactManager::instance()->items())
	{
		if (contact.isAnonymous())
			continue;

		QString desc;
		if (!contact.currentStatus().description().isEmpty())
			desc = contact.currentStatus().description();
		desc.replace('\n', ' ');

		QStringList labels;
		labels << contact.display(true)
				<< contact.contactAccount().protocolName()
				<< contact.id()
				<< contact.ownerBuddy().nickName()
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

	new WindowGeometryManager(new ConfigFileVariantWrapper("LastSeen", "LastSeenWidgetGeometry"), QRect(0, 0, 800, 300), this);

	kdebugf2();
}

InfosDialog::~InfosDialog()
{
	kdebugf();

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

	BaseActionContext actionContext{this};
	actionContext.setBuddies(BuddySet(contact.ownerBuddy()));
	actionContext.setChat(ChatTypeContact::findChat(contact, ActionCreateAndAdd));
	actionContext.setContacts(ContactSet(contact));
	actionContext.setRoles(RoleSet() << ContactRole);

	QScopedPointer<QMenu> menu(new QMenu());
	MenuInventory::instance()->menu("buddy-list")->attachToMenu(menu.data());
	MenuInventory::instance()->menu("buddy-list")->applyTo(menu.data(), &actionContext);
	MenuInventory::instance()->menu("buddy-list")->update();
	menu->exec(QCursor::pos());
}

#include "moc_infos_dialog.cpp"
