/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "icons-manager.h"

#include "buddy-delete-window.h"

BuddyDeleteWindow::BuddyDeleteWindow(QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setModal(false);

	createGui();
}

BuddyDeleteWindow::~BuddyDeleteWindow()
{

}

void BuddyDeleteWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *labels = new QWidget(this);
	mainLayout->addWidget(labels);

	QHBoxLayout *labelsLayout = new QHBoxLayout(labels);

	QLabel *iconLabel = new QLabel(labels);
	iconLabel->setPixmap(IconsManager::instance()->pixmapByPath("32x32/dialog-warning.png"));
	labelsLayout->addWidget(iconLabel);

	QLabel *messageLabel = new QLabel(tr("Selected users:\n%0 will be deleted. Are you sure?"), labels);
	labelsLayout->addWidget(messageLabel);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addWidget(buttons);

	QPushButton *deleteButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("Delete"));
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"));

	connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(deleteButton, QDialogButtonBox::DestructiveRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void BuddyDeleteWindow::accept()
{
    QDialog::accept();
}

void BuddyDeleteWindow::reject()
{
    QDialog::reject();
}
