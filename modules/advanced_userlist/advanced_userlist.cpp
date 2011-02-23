/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QList>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QHBoxLayout>

#include "advanced_userlist.h"
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc/misc.h"
#include "pending_msgs.h"
#include "userbox.h"
#include "userinfo.h"
#include "userlist.h"

static int compareByPriority(const UserListElement &u1, const UserListElement &u2)
{
	return u2.data("Priority").toInt() - u1.data("Priority").toInt();
}

static int compareByPending(const UserListElement &u1, const UserListElement &u2)
{
	return int(pending.pendingMsgs(u2)) - int(pending.pendingMsgs(u1));
}

static QString getPriority(const UserListElement &elem)
{
	return elem.data("Priority").toString();
}

AdvancedUserList::AdvancedUserList()
{
	config_file.addVariable("AdvUserList", "Order", "Pending,Status,Priority,AltNick");
	order = QStringList::split(',', config_file.readEntry("AdvUserList", "Order"));

	userlist->addPerContactNonProtocolConfigEntry("priority", "Priority");

	// zliczamy u�ytkownik�w bez priorytetu
	int cnt = 0;
	foreach(const UserListElement &user, *userlist)
		if (user.data("Priority").isNull())
			++cnt;

	// i ustawiamy im priorytet
	int i = 1;

	foreach(const UserListElement &user, *userlist)
		if (user.data("Priority").isNull())
			user.setData("Priority", int(0), true, i++ == cnt);

	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));

	const QList<UserBox *> &userboxes = UserBox::userboxes();
	foreach(UserBox *userbox, userboxes)
		userboxCreated(userbox);
	connect(&UserBox::createNotifier, SIGNAL(objectCreated(QObject *)), this, SLOT(userboxCreated(QObject *)));
	connect(&UserInfo::createNotifier, SIGNAL(objectCreated(QObject *)), this, SLOT(userInfoWindowCreated(QObject *)));

	KaduParser::registerTag("priority", getPriority);
}

AdvancedUserList::~AdvancedUserList()
{
	KaduParser::unregisterTag("priority", getPriority);

	disconnect(&UserInfo::createNotifier, SIGNAL(objectCreated(QObject *)), this, SLOT(userInfoWindowCreated(QObject *)));
	disconnect(&UserBox::createNotifier, SIGNAL(objectCreated(QObject *)), this, SLOT(userboxCreated(QObject *)));

	disconnect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
				this,      SLOT(userAdded(UserListElement, bool, bool)));
	const QList<UserBox *> &userboxes = UserBox::userboxes();
	foreach(UserBox *userbox, userboxes)
	{
		userbox->removeCompareFunction("Priority");
		userbox->removeCompareFunction("Pending");
	}
}

void AdvancedUserList::userAdded(UserListElement elem, bool massively, bool last)
{
	elem.setData("Priority", int(0), massively, last);
}

void AdvancedUserList::userboxCreated(QObject *new_object)
{
	UserBox *box = static_cast<UserBox *>(new_object);
	box->addCompareFunction("Pending", tr("Number of pending messages"), compareByPending);
	box->addCompareFunction("Priority", tr("Priorities"), compareByPriority);

	int idx = 0;
	// TODO: lipne sortowanie b�belkowe, p��niej si� to napisze porz�dnie
	foreach(const QString &funId, order)
	{
		while (box->compareFunctions()[idx].id != funId)
			if (!box->moveUpCompareFunction(funId))
			{
				--idx;
				break;
			}
		++idx;
	}
}

void AdvancedUserList::userInfoWindowCreated(QObject *new_object)
{
	kdebugf();

	UserInfo *info = static_cast<UserInfo *>(new_object);
	connect(info, SIGNAL(updateClicked(UserInfo *)), this, SLOT(updateClicked(UserInfo *)));
	QLabel *space1 = static_cast<QLabel*>(info->child("space_for_advanced_userlist_label"));
	QWidget *space2 = static_cast<QWidget*>(info->child("space_for_advanced_userlist_spinbox"));

	if (!space1 || !space2)
		return;

	space1->setText(tr("Priority"));
	QSpinBox *spinBox = new QSpinBox(-10, 10, 1, space2);
	spinBox->setName("priority_spinbox");
	spinBox->setValue(info->user().data("Priority").toInt());

	kdebugf2();
}

void AdvancedUserList::updateClicked(UserInfo *info)
{
	kdebugf();

	int val = static_cast<QSpinBox *>(info->child("priority_spinbox"))->value();
	if (info->user().data("Priority").toInt() != val)
	{
		info->user().setData("Priority", val);
		UserBox::refreshAllLater();
	}

	kdebugf2();
}

void AdvancedUserList::onUpButton()
{
	int index = sortingListBox->currentRow();
	if (index < 1)
		return;
	QListWidgetItem *item = sortingListBox->takeItem(index);

	QString removed = newOrder[index];
	newOrder[index] = newOrder[index - 1];
	newOrder[index - 1] = removed;

	sortingListBox->insertItem(index - 1, item);
	sortingListBox->setCurrentItem(item);
}

void AdvancedUserList::onDownButton()
{
	int index = sortingListBox->currentRow();
	if (index == sortingListBox->count() - 1 || index == -1)
		return;

	QListWidgetItem *item = sortingListBox->takeItem(index);

	QString removed = newOrder[index];
	newOrder[index] = newOrder[index + 1];
	newOrder[index + 1] = removed;

	sortingListBox->insertItem(index + 1, item);
	sortingListBox->setCurrentItem(item);
}

void AdvancedUserList::displayFunctionList()
{
	kdebugf();

	QList <UserBox::CmpFuncDesc> cmpFuns = kadu->userbox()->compareFunctions();
	QListWidgetItem *selected = sortingListBox->currentItem();

	sortingListBox->clear();
	foreach(const QString &id, order)
		foreach(const UserBox::CmpFuncDesc &fun, cmpFuns)
			if (id == fun.id)
			{
				sortingListBox->addItem(fun.description);
				break;
			}

	if (selected)
		sortingListBox->setCurrentRow(0);
	else
		sortingListBox->setCurrentItem(selected);

	kdebugf2();
}

void AdvancedUserList::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	ConfigGroupBox *sortingGroupBox = mainConfigurationWindow->configGroupBox("Look", "Userbox", "Sorting");

	QWidget *sortingWidget = new QWidget(sortingGroupBox->widget());

	QHBoxLayout *sortingHBox = new QHBoxLayout(sortingWidget);
	sortingHBox->setSpacing(5);

	sortingListBox = new QListWidget();

	QWidget *buttons = new QWidget();

	sortingHBox->addWidget(sortingListBox);
	sortingHBox->addWidget(buttons);

	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *up = new QPushButton(tr("Up"), buttons);
	QPushButton *down = new QPushButton(tr("Down"), buttons);

	buttonsLayout->addWidget(up);
	buttonsLayout->addWidget(down);
	buttonsLayout->addStretch(100);

	connect(up, SIGNAL(clicked()), this, SLOT(onUpButton()));
	connect(down, SIGNAL(clicked()), this, SLOT(onDownButton()));

	sortingGroupBox->addWidgets(new QLabel(tr("Sorting functions") + ':', sortingGroupBox->widget()), sortingWidget);

	newOrder = order;
	displayFunctionList();
}

void AdvancedUserList::configurationWindowApplied()
{
	kdebugf();

	order = newOrder;
	config_file.writeEntry("AdvUserList", "Order", order.join(","));

	const QList<UserBox *> &userboxes = UserBox::userboxes();
	foreach(UserBox *userbox, userboxes)
		userboxCreated(userbox);

	kdebugf2();
}
