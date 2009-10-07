/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
*/
#include <QtGui/QLineEdit>

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"

#include "contact-groups-configuration-widget.h"

ContactGroupsConfigurationWidget::ContactGroupsConfigurationWidget(ConfigurationWindowDataManager *dataManager, QWidget *parent)
	: ConfigurationWidget(dataManager, parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactGroupsConfigurationWidget::~ContactGroupsConfigurationWidget()
{
}

void ContactGroupsConfigurationWidget::createGui()
{
	appendUiFile(dataPath("kadu/configuration/contact-account-data.ui"));

	QLineEdit *addrLineEdit = dynamic_cast<QLineEdit *>(widgetById("Addr"));
	addrLineEdit->setReadOnly(true);
	addrLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *versionLineEdit = dynamic_cast<QLineEdit *>(widgetById("Version"));
	versionLineEdit->setReadOnly(true);
	versionLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *dnsLineEdit = dynamic_cast<QLineEdit *>(widgetById("DnsName"));
	dnsLineEdit->setReadOnly(true);
	dnsLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *statusLineEdit = dynamic_cast<QLineEdit *>(widgetById("Status"));
	statusLineEdit->setReadOnly(true);
	statusLineEdit->setBackgroundRole(QPalette::Button);
}

/*
void ContactDataWindow::setupTab2()
{
	kdebugf();

	// TODO: 0.6.6 move to ContactGroupsConfigurationWidget

	scrollArea = new QScrollArea(tw_main);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget *groupsTab = new QWidget(tw_main);
	QVBoxLayout *groupsTabLayout = new QVBoxLayout(groupsTab);

	tw_main->addTab(scrollArea, tr("Groups"));

	scrollArea->setWidget(groupsTab);
	scrollArea->setWidgetResizable(true);

	groupsWidget = new QWidget(groupsTab);
	groupsLayout = new QVBoxLayout(groupsWidget);
	groupsLayout->setSpacing(3);

	foreach(Group* group , GroupManager::instance()->groups())
	{
		QCheckBox *checkBox = new QCheckBox(group->name());
		checkBox->setChecked(CurrentContact.isInGroup(group));

		groupsLayout->addWidget(checkBox);

		groups.append(checkBox);
	}

	newGroup = new QLineEdit(groupsTab);
	QPushButton *addNewGroup = new QPushButton(tr("Add new group"), groupsTab);

	groupsTabLayout->addWidget(groupsWidget);
	groupsTabLayout->addWidget(newGroup);
	groupsTabLayout->addWidget(addNewGroup);

	connect(addNewGroup, SIGNAL(clicked()), this, SLOT(newGroupClicked()));
	connect(newGroup, SIGNAL(returnPressed()), this, SLOT(newGroupClicked()));

	kdebugf2();
}

void ContactDataWindow::newGroupClicked()
{
	kdebugf();
	QString groupName = newGroup->text();
	if (!GroupManager::instance()->acceptableGroupName(groupName))
	{
		kdebugf2();
		return;
	}

	QCheckBox *checkBox = new QCheckBox(groupName);

	checkBox->setChecked(true);

	groupsLayout->addWidget(checkBox);

	checkBox->show();

	groups.append(checkBox);

	QTimer::singleShot(0, this, SLOT(scrollToBottom()));

	//create new group
	GroupManager::instance()->byName(groupName);

	kdebugf2();
}*/
