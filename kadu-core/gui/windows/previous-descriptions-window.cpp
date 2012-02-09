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
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>

#include "gui/windows/choose-description.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/previous-descriptions-window.h"
#include "icons/kadu-icon.h"
#include "parser/parser.h"
#include "status/description-manager.h"
#include "status/description-model.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#include "icons/icons-manager.h"
#include "activate.h"
#include "debug.h"

#include "previous-descriptions-window.h"

QMap<QWidget *, PreviousDescriptionsWindow *> PreviousDescriptionsWindow::Dialogs;

PreviousDescriptionsWindow * PreviousDescriptionsWindow::showDialog(ChooseDescription *parent)
{
	PreviousDescriptionsWindow *dialog;
	if (Dialogs.contains(parent))
		dialog = Dialogs[parent];
	else
	{
		dialog = new PreviousDescriptionsWindow(parent);
		Dialogs[parent] = dialog;
	}

	dialog->show();
	_activateWindow(dialog);

	return dialog;
}

PreviousDescriptionsWindow::PreviousDescriptionsWindow(ChooseDescription *parent) :
		QDialog(parent)
{
	setWindowTitle(tr("Previous descriptions"));

	QGridLayout *grid = new QGridLayout(this);

	list = new QListView(this);
	list->setModel(DescriptionManager::instance()->model());
	list->setWordWrap(true);
	list->setAlternatingRowColors(true);
	list->setEditTriggers(QAbstractItemView::NoEditTriggers);

	grid->addWidget(list, 0, 0, 1, 1);
	connect(list, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(selectListItem(const QModelIndex &)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Select"), this);
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(chooseDescription()));

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
	Dialogs.remove(parentWidget());
}

void PreviousDescriptionsWindow::chooseDescription()
{
	selectListItem(list->currentIndex());
}

void PreviousDescriptionsWindow::selectListItem(const QModelIndex &index)
{
	// TODO: fix this workaround
	QString text = list->model()->data(index, Qt::DisplayRole).toString();

	emit descriptionSelected(text);
	accept();
}

