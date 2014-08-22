/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "buddies/group.h"
#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"
#include "misc/change-notifier.h"
#include "misc/change-notifier-lock.h"
#include "misc/misc.h"

#include "group-properties-window.h"

GroupPropertiesWindow::GroupPropertiesWindow(Group editedGroup, QWidget *parent)
	: QWidget(parent, Qt::Window), DesktopAwareObject(this), group(editedGroup)
{
	setWindowRole("kadu-group-properties");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Properties of group %1").arg(group.name()));

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QGroupBox *behaviorGroupBox = new QGroupBox;
	QVBoxLayout *behavior_layout = new QVBoxLayout(behaviorGroupBox);
	behavior_layout->setSpacing(5);
	behaviorGroupBox->setTitle(tr("Behavior"));

	notifyCheckBox = new QCheckBox(tr("Notify about status changes"), behaviorGroupBox);
	notifyCheckBox->setChecked(group.notifyAboutStatusChanges());

	offlineCheckBox = new QCheckBox(tr("Offline for this group"), behaviorGroupBox);
	offlineCheckBox->setChecked(group.offlineToGroup());
	offlineCheckBox->setToolTip(tr("Supported for Gadu-Gadu network"));

	info = new QLabel;
	info->setText("<font size=\"-1\"><i>" + tr("Work only when network supports it") + "</i></font>");
	info->setToolTip(tr("Supported for Gadu-Gadu network"));
	info->setIndent(20);
	info->setVisible(offlineCheckBox->isChecked());
	connect(offlineCheckBox, SIGNAL(toggled(bool)), info, SLOT(setVisible(bool)));

	allGroupCheckBox = new QCheckBox(tr("Show in group \"All\""), behaviorGroupBox);
	allGroupCheckBox->setChecked(group.showInAllGroup());

	behavior_layout->addWidget(notifyCheckBox);
	behavior_layout->addWidget(offlineCheckBox);
	behavior_layout->addWidget(info);
	behavior_layout->addWidget(allGroupCheckBox);

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
		icon->setIcon(KaduIcon("document-open").icon());
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

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	buttons_layout->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->addWidget(behaviorGroupBox);
	main_layout->addWidget(lookGroupBox);
	main_layout->addSpacing(10);//TODO
	main_layout->addWidget(buttons_layout);
}

void GroupPropertiesWindow::selectIcon()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Choose an icon"), config_file.readEntry("GroupIcon", "recentPath", "~/"),
					tr("Images (*.png *.xpm *.jpg);;All Files (*)"));
	if (!file.isEmpty())
	{
		QFileInfo fileInfo(file);
		config_file.writeEntry("GroupIcon", "recentPath", fileInfo.absolutePath());
		icon->setText(QString());
		icon->setIcon(QIcon(file));
		iconPath = file;
	}
}


void GroupPropertiesWindow::applyClicked()
{
	ChangeNotifierLock lock(group.changeNotifier());

	group.setShowName(nameCheckBox->isChecked());
	group.setShowIcon(iconCheckBox->isChecked());
	group.setIcon(iconPath);
	group.setNotifyAboutStatusChanges(notifyCheckBox->isChecked());
	group.setOfflineToGroup(offlineCheckBox->isChecked());
	group.setShowInAllGroup(allGroupCheckBox->isChecked());
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

#include "moc_group-properties-window.cpp"
