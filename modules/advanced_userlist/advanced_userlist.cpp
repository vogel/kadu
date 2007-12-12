/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qspinbox.h>

#include "advanced_userlist.h"
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"
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

	// zliczamy u¿ytkowników bez priorytetu
	int cnt = 0;
	CONST_FOREACH(user, *userlist)
		if ((*user).data("Priority").isNull())
			++cnt;

	// i ustawiamy im priorytet
	int i = 1;
	FOREACH(user, *userlist)
		if ((*user).data("Priority").isNull())
			(*user).setData("Priority", int(0), true, i++ == cnt);

	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this,       SLOT(userAdded(UserListElement, bool, bool)));

	const QValueList<UserBox *> &userboxes = UserBox::userboxes();
	CONST_FOREACH(userbox, userboxes)
		userboxCreated(*userbox);
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
	const QValueList<UserBox *> &userboxes = UserBox::userboxes();
	CONST_FOREACH(userbox, userboxes)
	{
		(*userbox)->removeCompareFunction("Priority");
		(*userbox)->removeCompareFunction("Pending");
	}
}

void AdvancedUserList::userAdded(UserListElement elem, bool massively, bool last)
{
	elem.setData("Priority", int(0), massively, last);
}

void AdvancedUserList::userboxCreated(QObject *new_object)
{
	UserBox *box = static_cast<UserBox *>(new_object);
	box->addCompareFunction("Pending", tr("Number of pending messages)"), compareByPending);
	box->addCompareFunction("Priority", tr("Priorities"), compareByPriority);

	int idx = 0;
	// TODO: lipne sortowanie b±belkowe, pó¼niej siê to napisze porz±dnie
	CONST_FOREACH(funId, order)
	{
		while (box->compareFunctions()[idx].id != *funId)
			if (!box->moveUpCompareFunction(*funId))
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
	QWidget *space = static_cast<QWidget*>(info->child("space_for_advanced_userlist"));
	if (!space)
		space = info;
	new QLabel(tr("Priority"), space);
	(new QSpinBox(-10, 10, 1, space, "priority_spinbox"))->setValue(info->user().data("Priority").toInt());
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
	unsigned int index = sortingListBox->currentItem();
	if (index == 0)
		return;

	QString removed = newOrder[index];
	newOrder[index] = newOrder[index - 1];
	newOrder[index - 1] = removed;

	QString text = sortingListBox->text(index);
	sortingListBox->removeItem(index);
	sortingListBox->insertItem(text, --index);
	sortingListBox->setSelected(sortingListBox->findItem(text), true);
}

void AdvancedUserList::onDownButton()
{
	unsigned int index = sortingListBox->currentItem();
	if (index == sortingListBox->count() - 1)
		return;

	QString removed = newOrder[index];
	newOrder[index] = newOrder[index + 1];
	newOrder[index + 1] = removed;

	QString text = sortingListBox->text(index);
	sortingListBox->removeItem(index);
	sortingListBox->insertItem(text, ++index);
	sortingListBox->setSelected(sortingListBox->findItem(text), true);
}

void AdvancedUserList::displayFunctionList()
{
	kdebugf();
	QValueList <UserBox::CmpFuncDesc> cmpFuns = kadu->userbox()->compareFunctions();
	QString selected = sortingListBox->currentText();

	sortingListBox->clear();
	CONST_FOREACH(id, order)
		CONST_FOREACH(fun, cmpFuns)
			if ((*id) == (*fun).id)
			{
				sortingListBox->insertItem((*fun).description);
				break;
			}

	if (selected.isEmpty())
		sortingListBox->setSelected(0, true);
	else
		sortingListBox->setSelected(sortingListBox->findItem(selected, Qt::ExactMatch), true);

	kdebugf2();
}

void AdvancedUserList::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	ConfigGroupBox *sortingGroupBox = mainConfigurationWindow->configGroupBox("Look", "Userbox", "Sorting");
	QHBox *sortingHBox = new QHBox(sortingGroupBox->widget());
	sortingHBox->setSpacing(5);

	sortingListBox = new QListBox(sortingHBox);

	QWidget *buttons = new QWidget(sortingHBox);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *up = new QPushButton(tr("Up"), buttons);
	QPushButton *down = new QPushButton(tr("Down"), buttons);

	buttonsLayout->addWidget(up);
	buttonsLayout->addWidget(down);
	buttonsLayout->addStretch(100);

	connect(up, SIGNAL(clicked()), this, SLOT(onUpButton()));
	connect(down, SIGNAL(clicked()), this, SLOT(onDownButton()));

	sortingGroupBox->addWidgets(new QLabel(tr("Sorting functions") + ":", sortingGroupBox->widget()), sortingHBox);

	newOrder = order;
	displayFunctionList();
}

void AdvancedUserList::configurationWindowApplied()
{
	kdebugf();

	order = newOrder;
	config_file.writeEntry("AdvUserList", "Order", order.join(","));

	const QValueList<UserBox *> &userboxes = UserBox::userboxes();
	CONST_FOREACH(userbox, userboxes)
		userboxCreated(*userbox);

	kdebugf2();
}
