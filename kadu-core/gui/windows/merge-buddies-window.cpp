/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "buddies/filter/non-buddy-filter.h"
#include "buddies/buddy-manager.h"
#include "gui/widgets/select-buddy-combo-box.h"
#include "icons-manager.h"

#include "merge-buddies-window.h"

MergeBuddiesWindow::MergeBuddiesWindow(Buddy buddy, QWidget *parent) :
		QDialog(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowRole("kadu-merge-buddies");
	createGui();
}

MergeBuddiesWindow::~MergeBuddiesWindow()
{
}

void MergeBuddiesWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("<i>Choose which contact would you like to merge with <b>%1</b></i>")
			.arg(MyBuddy.display()), this));

	QWidget *chooseWidget = new QWidget(this);
	layout->addWidget(chooseWidget);

	QHBoxLayout *chooseLayout = new QHBoxLayout(chooseWidget);

	chooseLayout->addWidget(new QLabel(tr("Contact:"), chooseWidget));
	SelectCombo = new SelectBuddyComboBox(chooseWidget);
	NonBuddyFilter *filter = new NonBuddyFilter(SelectCombo);
	filter->setBuddy(MyBuddy);
	SelectCombo->addFilter(filter);
	connect(SelectCombo, SIGNAL(buddyChanged(Buddy)), this, SLOT(selectedBuddyChanged(Buddy)));
	chooseLayout->addWidget(SelectCombo);

	layout->addStretch(100);
	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);

	MergeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Merge"), this);
	MergeButton->setDefault(true);
	MergeButton->setEnabled(false);
	connect(MergeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(MergeButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);
}

void MergeBuddiesWindow::selectedBuddyChanged(Buddy buddy)
{
	MergeButton->setEnabled(!buddy.isNull());
}

void MergeBuddiesWindow::accept()
{
	Buddy mergeWith = SelectCombo->currentBuddy();
	if (mergeWith.isNull())
		return;

	if (MyBuddy.isNull())
		return;

	BuddyManager::instance()->mergeBuddies(mergeWith, MyBuddy);

	QDialog::accept();
}
