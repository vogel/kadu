/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "group-edit-window.h"

#include "buddies/group-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/kadu-icon.h"

#include <QtCore/QFileInfo>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

GroupEditWindow::GroupEditWindow(GroupManager *groupManager, DeprecatedConfigurationApi *configuration, Group group, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		QDialog(parent),
		m_groupManager(groupManager),
		m_configuration(configuration),
		m_group(group),
		m_add(group.isNull())
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setMinimumWidth(400);
	setWindowRole("kadu-group-edit");
	setWindowTitle(m_add
			? tr("Add group")
			: tr("Edit group %1").arg(group.name()));

	createGui();
	if (m_add)
		m_group = Group::create();
	loadValues();
	dataChanged();

	connect(m_groupManager, SIGNAL(groupAdded(Group)), this, SLOT(dataChanged()));
	connect(m_groupManager, SIGNAL(groupRemoved(Group)), this, SLOT(dataChanged()));
}

GroupEditWindow::~GroupEditWindow()
{
}

void GroupEditWindow::createGui()
{
	auto mainWidget = new QWidget{this};

	m_nameLineEdit = new QLineEdit{mainWidget};
	m_nameErrorLabel = new QLabel(mainWidget);
	m_nameCheckBox = new QCheckBox{tr("Show group name"), mainWidget};
	m_iconCheckBox = new QCheckBox{tr("Use custom icon"), mainWidget};
	m_selectIconButton = new QToolButton{mainWidget};
	m_selectIconButton->setAutoRaise(true);
	m_selectIconButton->setIconSize(QSize{16, 16});
	m_notifyCheckBox = new QCheckBox{tr("Notify about status changes"), mainWidget};
	m_offlineCheckBox = new QCheckBox{tr("Offline for this group"), mainWidget};
	m_offlineCheckBox->setToolTip(tr("Supported for Gadu-Gadu network"));
	m_allGroupCheckBox = new QCheckBox{tr("Show in group \"All\""), mainWidget};

	connect(m_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	connect(m_nameCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	connect(m_iconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	connect(m_selectIconButton, SIGNAL(clicked(bool)), this, SLOT(selectIcon()));
	connect(m_notifyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	connect(m_offlineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	connect(m_offlineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	connect(m_allGroupCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));

	auto iconWidget = new QWidget{mainWidget};
	auto iconLayout = new QHBoxLayout{iconWidget};
	iconLayout->setMargin(0);
	iconLayout->addWidget(m_iconCheckBox);
	iconLayout->addWidget(m_selectIconButton);
	iconLayout->addStretch(100);

	auto layout = new QFormLayout{mainWidget};
	layout->addRow(tr("Name:"), m_nameLineEdit);
	layout->addWidget(m_nameErrorLabel);
	layout->addWidget(m_nameCheckBox);
	layout->addWidget(iconWidget);
	layout->addWidget(m_notifyCheckBox);
	layout->addWidget(m_offlineCheckBox);
	layout->addWidget(m_allGroupCheckBox);

	auto saveTitle = m_add
			? tr("Add group")
			: tr("Save group");
	m_saveButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), saveTitle, this);
	m_saveButton->setDefault(true);
	connect(m_saveButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	if (!m_add)
	{
		m_applyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
		connect(m_applyButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	}
	else
		m_applyButton = nullptr;

	auto cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	auto dialogButtonBox = new QDialogButtonBox{this};
	dialogButtonBox->addButton(m_saveButton, QDialogButtonBox::AcceptRole);
	if (m_applyButton)
		dialogButtonBox->addButton(m_applyButton, QDialogButtonBox::ApplyRole);
	dialogButtonBox->addButton(cancel, QDialogButtonBox::DestructiveRole);

	auto mainLayout = new QVBoxLayout{this};
	mainLayout->addWidget(mainWidget);
	mainLayout->addSpacing(16);
	mainLayout->addStretch(100);
	mainLayout->addWidget(dialogButtonBox);

	m_nameLineEdit->setFocus();
}

void GroupEditWindow::loadValues()
{
	m_nameLineEdit->setText(m_group.name());
	m_nameCheckBox->setChecked(m_group.showName());
	m_iconCheckBox->setChecked(m_group.showIcon());
	m_notifyCheckBox->setChecked(m_group.notifyAboutStatusChanges());
	m_offlineCheckBox->setChecked(m_group.offlineToGroup());
	m_allGroupCheckBox->setChecked(m_group.showInAllGroup());

	m_selectedIcon = m_group.icon();
	auto iconFile = QFileInfo{m_selectedIcon};
	if (iconFile.exists())
		m_selectIconButton->setIcon(QIcon{iconFile.absoluteFilePath()});
	else
		m_selectIconButton->setIcon(KaduIcon{"document-open", "16"}.icon());
}

void GroupEditWindow::storeValues()
{
	m_group.setName(m_nameLineEdit->text());
	m_group.setShowName(m_nameCheckBox->isChecked());
	m_group.setShowIcon(m_iconCheckBox->isChecked());
	m_group.setIcon(m_selectedIcon);
	m_group.setNotifyAboutStatusChanges(m_notifyCheckBox->isChecked());
	m_group.setOfflineToGroup(m_offlineCheckBox->isChecked());
	m_group.setShowInAllGroup(m_allGroupCheckBox->isChecked());
}

void GroupEditWindow::selectIcon()
{
	auto recentPath = m_configuration->readEntry("GroupIcon", "recentPath", "~/");
	auto file = QFileDialog::getOpenFileName(this, tr("Choose an icon"), recentPath, tr("Images (*.png *.xpm *.jpg);;All Files (*)"));
	auto iconFile = QFileInfo{file};
	if (iconFile.exists() && iconFile.isReadable())
	{
		m_configuration->writeEntry("GroupIcon", "recentPath", iconFile.absolutePath());
		m_selectIconButton->setIcon(QIcon{iconFile.absoluteFilePath()});
		m_selectedIcon = file;
	}
}

void GroupEditWindow::dataChanged()
{
	m_saveButton->setEnabled(false);
	if (m_applyButton)
		m_applyButton->setEnabled(false);

	m_selectIconButton->setEnabled(m_iconCheckBox->isChecked());

	auto invalidNameMessage = m_groupManager->validateGroupName(m_group, m_nameLineEdit->text());
	if (!invalidNameMessage.isEmpty())
	{
		m_nameErrorLabel->setText(QString{"<small><i>%1</i></small>"}.arg(invalidNameMessage));
		return;
	}
	else
		m_nameErrorLabel->setText(QString{"<small><i>&nbsp;</i></small>"}.arg(invalidNameMessage));

	if (m_nameLineEdit->text() != m_group.name() ||
	    m_nameCheckBox->isChecked() != m_group.showName() ||
	    m_iconCheckBox->isChecked() != m_group.showIcon() ||
	    m_selectedIcon != m_group.icon() ||
	    m_notifyCheckBox->isChecked() != m_group.notifyAboutStatusChanges() ||
	    m_offlineCheckBox->isChecked() != m_group.offlineToGroup() ||
	    m_allGroupCheckBox->isChecked() != m_group.showInAllGroup())
	{
		m_saveButton->setEnabled(true);
		if (m_applyButton)
			m_applyButton->setEnabled(true);
	}
}

void GroupEditWindow::accept()
{
	storeValues();
	if (m_add)
		m_groupManager->addItem(m_group);

	QDialog::accept();
}

void GroupEditWindow::apply()
{
	storeValues();
	loadValues();
}

#include "moc_group-edit-window.cpp"
