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
#include <QtGui/QGridLayout>
#include <QtGui/QListView>
#include <QtGui/QPushButton>

#include "activate.h"
#include "gui/windows/status-window.h"
#include "status/description-manager.h"
#include "status/description-model.h"

#include "previous-descriptions-window.h"

PreviousDescriptionsWindow::PreviousDescriptionsWindow(StatusWindow *parent) :
		QDialog(parent)
{
	setWindowTitle(tr("Previous descriptions"));

	QGridLayout *grid = new QGridLayout(this);

	DescriptionsList = new QListView(this);
	DescriptionsList->setModel(DescriptionManager::instance()->model());
	DescriptionsList->setWordWrap(true);
	DescriptionsList->setAlternatingRowColors(true);
	DescriptionsList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	grid->addWidget(DescriptionsList, 0, 0, 1, 1);
	connect(DescriptionsList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(listItemDoubleClicked(const QModelIndex &)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Select"), this);
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(chooseButtonClicked()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	grid->addWidget(buttons, 1, 0, 1, 1);

	setMinimumSize(QDialog::sizeHint().expandedTo(QSize(400, 200)));
	setLayout(grid);
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
	// TODO: fix this workaround
	QString text = DescriptionsList->model()->data(index, Qt::DisplayRole).toString();

	emit descriptionSelected(text);
	accept();
}

