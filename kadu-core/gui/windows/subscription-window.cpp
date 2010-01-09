/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QVBoxLayout>

#include "buddies/model/groups-model.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "icons-manager.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "subscription-window.h"

void SubscriptionWindow::getSubscription(Contact contact, QObject *receiver, const char *slot)
{
	SubscriptionWindow *window = new SubscriptionWindow(contact);
	connect(window, SIGNAL(requestAccepted(Contact)), receiver, slot);

	window->exec();
}

SubscriptionWindow::SubscriptionWindow(Contact contact, QWidget *parent) :
		QDialog(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("New Contact Request"));
	
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(2, 4);

	QLabel *messageLabel = new QLabel(tr("%1 wants to be able to chat with you.").arg(contact.id()), this);
	QLabel *visibleNameLabel = new QLabel(tr("Visible Name") + ":", this);
	QLabel *enterNameLabel = new QLabel(tr("Enter a name for this contact."), this);
	VisibleName = new QLineEdit(this);
	QLabel *groupLabel = new QLabel(tr("Add in Group") + ":", this);

	GroupCombo = new QComboBox(this);
	GroupCombo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	connect(GroupCombo, SIGNAL(activated(int)), this, SLOT(groupChanged(int)));
	GroupsModel *groupComboModel = new GroupsModel(GroupCombo);

	QSortFilterProxyModel *sortModel = new QSortFilterProxyModel(GroupCombo);
	sortModel->setSourceModel(groupComboModel);
	sortModel->setDynamicSortFilter(true);
	sortModel->sort(1);
	sortModel->sort(0);

	ActionsProxyModel::ModelActionList groupsModelBeforeActions;
	groupsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select group - "), ""));
	ActionsProxyModel::ModelActionList groupsModelAfterActions;
	groupsModelAfterActions.append(qMakePair<QString, QString>(tr("Create a new group..."), "createNewGroup"));

	ActionsProxyModel *groupsProxyModel = new ActionsProxyModel(groupsModelBeforeActions,
			groupsModelAfterActions, GroupCombo);
	groupsProxyModel->setSourceModel(sortModel);

	GroupCombo->setModel(groupsProxyModel);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Allow"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Ignore"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(messageLabel, 0, 0, 1, 3);
	layout->addWidget(visibleNameLabel, 1, 0, 1, 1);
	layout->addWidget(VisibleName, 1, 1, 1, 1);
	layout->addWidget(enterNameLabel, 2, 1, 1, 1);
	layout->addWidget(groupLabel, 3, 0, 1, 1);
	layout->addWidget(GroupCombo, 3, 1, 1, 1);
	layout->addWidget(buttons, 4, 1, 1, 3);
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::groupChanged(int index)
{
	QModelIndex modelIndex = GroupCombo->model()->index(index, 0, QModelIndex());
	QString action = modelIndex.data(ActionRole).toString();

	if (action.isEmpty())
		return;
	bool ok;

	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
			tr("Please enter the name for the new group:"), QLineEdit::Normal,
			QString::null, &ok);

	if (!ok || newGroupName.isEmpty() || !GroupManager::instance()->acceptableGroupName(newGroupName))
	{
		GroupCombo->setCurrentIndex(0);
		return;
	}

	GroupManager::instance()->byName(newGroupName);
	GroupCombo->setCurrentIndex(GroupCombo->findText(newGroupName));
}


void SubscriptionWindow::accepted()
{
	Buddy buddy = CurrentContact.ownerBuddy();
	if (buddy.isNull())
		buddy = BuddyManager::instance()->byId(CurrentContact.contactAccount(), VisibleName->text().isEmpty() ? CurrentContact.id() : VisibleName->text(), true);
	buddy.setAnonymous(false);
	if (!VisibleName->text().isEmpty())
		buddy.setDisplay(VisibleName->text());
	
	QModelIndex modelIndex = GroupCombo->model()->index(GroupCombo->currentIndex(), 0, QModelIndex());
	QString groupName = modelIndex.data(Qt::DisplayRole).toString();

	if (!groupName.isEmpty())
		buddy.addToGroup(GroupManager::instance()->byName(groupName));
	
	emit requestAccepted(CurrentContact);
	close();
}
