/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "model/roles.h"
#include "status/description-manager.h"
#include "status/description-model.h"
#include "activate.h"

#include "previous-descriptions-window.h"

PreviousDescriptionsWindow::PreviousDescriptionsWindow(QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Previous descriptions"));

	QVBoxLayout *layout = new QVBoxLayout(this);

	DescriptionsList = new QListView(this);
	DescriptionsList->setModel(DescriptionManager::instance()->model());
	DescriptionsList->setWordWrap(true);
	DescriptionsList->setAlternatingRowColors(true);
	DescriptionsList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(DescriptionsList, SIGNAL(doubleClicked(const QModelIndex &)),
	        this, SLOT(listItemDoubleClicked(const QModelIndex &)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Select"), this);
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(chooseButtonClicked()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	layout->addWidget(DescriptionsList);
	layout->addSpacing(16);
	layout->addWidget(buttons);

	setMinimumSize(QDialog::sizeHint().expandedTo(QSize(400, 200)));
}

PreviousDescriptionsWindow::~PreviousDescriptionsWindow()
{
}

void PreviousDescriptionsWindow::chooseButtonClicked()
{
	listItemDoubleClicked(DescriptionsList->currentIndex());
}

void PreviousDescriptionsWindow::listItemDoubleClicked(const QModelIndex &index)
{
	emit descriptionSelected(index.data(DescriptionRole).toString());
	accept();
}
