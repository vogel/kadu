/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "buddies/group.h"
#include "gui/windows/image-dialog.h"

#include "icons-manager.h"
#include "misc/misc.h"

#include "group-properties-window.h"

GroupPropertiesWindow::GroupPropertiesWindow(Group editedGroup, QWidget *parent)
	: QWidget(parent, Qt::Window), group(editedGroup)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Properties of group %1").arg(group.name()));

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QGroupBox *behaviourGroupBox = new QGroupBox;
	QVBoxLayout *behaviour_layout = new QVBoxLayout(behaviourGroupBox);
	behaviour_layout->setSpacing(5);
	behaviourGroupBox->setTitle(tr("Behaviour"));

	notifyCheckBox = new QCheckBox(tr("Notify about status changes"), behaviourGroupBox);
	notifyCheckBox->setChecked(group.notifyAboutStatusChanges());

	offlineCheckBox = new QCheckBox(tr("Offline for this group"), behaviourGroupBox);
	offlineCheckBox->setChecked(group.offlineToGroup());
	offlineCheckBox->setToolTip(tr("Supported for Gadu-Gadu network"));

	info = new QLabel;
	info->setText("<font size=\"-1\"><i>" + tr("Work only when network supports it") + "</i></font>");
	info->setToolTip(tr("Supported for Gadu-Gadu network"));
	info->setIndent(20);
	info->setVisible(offlineCheckBox->isChecked());
	connect(offlineCheckBox, SIGNAL(toggled(bool)), info, SLOT(setVisible(bool)));

	allGroupCheckBox = new QCheckBox(tr("Show in group \"All\""), behaviourGroupBox);
	allGroupCheckBox->setChecked(group.showInAllGroup());

	behaviour_layout->addWidget(notifyCheckBox);
	behaviour_layout->addWidget(offlineCheckBox);
	behaviour_layout->addWidget(info);
	behaviour_layout->addWidget(allGroupCheckBox);

	QGroupBox *lookGroupBox = new QGroupBox;
	QVBoxLayout *look_layout = new QVBoxLayout(lookGroupBox);
	lookGroupBox->setTitle(tr("Look"));

	QWidget *iconWidget = new QWidget(this);
	QHBoxLayout *icon_layout = new QHBoxLayout(iconWidget);
	icon_layout->setContentsMargins(0, 0, 0, 0);

	iconCheckBox = new QCheckBox(tr("Use custom icon"), lookGroupBox);
	iconCheckBox->setChecked(group.showIcon());

	iconPath = group.icon();
	icon = new QPushButton(iconWidget);

	if (iconPath.isEmpty())
	{
		icon->setIcon(IconsManager::instance()->iconByPath("16x16/document-open.png"));
		icon->setText(tr(" Set Icon"));
	}
	else
		icon->setIcon(QIcon(iconPath));

	icon->setEnabled(iconCheckBox->isChecked());
	connect(iconCheckBox, SIGNAL(toggled(bool)), icon, SLOT(setEnabled(bool)));
	connect(icon, SIGNAL(clicked()), this, SLOT(selectIcon()));

	icon_layout->addWidget(iconCheckBox);
	icon_layout->setStretchFactor(iconCheckBox, 100);

	icon_layout->addWidget(icon);

	nameCheckBox = new QCheckBox(tr("Show group name"), lookGroupBox);
	nameCheckBox->setChecked(group.showName());

	look_layout->addWidget(iconWidget);
	look_layout->addWidget(nameCheckBox);

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-ok.png"), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-apply.png"), tr("Apply"), this);
	buttons_layout->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-cancel.png"), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->addWidget(behaviourGroupBox);
	main_layout->addWidget(lookGroupBox);
	main_layout->addSpacing(10);//TODO
	main_layout->addWidget(buttons_layout);
}

void GroupPropertiesWindow::selectIcon()
{
	ImageDialog* iDialog = new ImageDialog(this);
	iDialog->setDirectory(config_file.readEntry("GroupIcon", "recentPath", "~/"));
	iDialog->setWindowTitle(tr("Choose an icon"));
	iDialog->setFilter(tr("Icons (*.png *.xpm *.jpg)"));
	if (iDialog->exec() == QDialog::Accepted && 1 == iDialog->selectedFiles().count())
	{
		iconPath = iDialog->selectedFiles()[0];
		config_file.writeEntry("GroupIcon", "recentPath", iDialog->directory().absolutePath());
		icon->setText("");
		icon->setIcon(QIcon(iconPath));
	}
	delete iDialog;
}


void GroupPropertiesWindow::applyClicked()
{
	group.blockUpdatedSignal();

	group.setShowName(nameCheckBox->isChecked());
	group.setShowIcon(iconCheckBox->isChecked());
	group.setIcon(iconPath);
	group.setNotifyAboutStatusChanges(notifyCheckBox->isChecked());
	group.setOfflineToGroup(offlineCheckBox->isChecked());
	group.setShowInAllGroup(allGroupCheckBox->isChecked());

	group.unblockUpdatedSignal();
}

void GroupPropertiesWindow::okClicked()
{
	applyClicked();
	close();
}

void GroupPropertiesWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
