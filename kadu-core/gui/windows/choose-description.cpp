/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QComboBox>
#include <QtGui/QCompleter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "icons/kadu-icon.h"
#include "parser/parser.h"
#include "status/description-manager.h"
#include "status/description-model.h"
#include "status/status-container.h"
#include "status/status-setter.h"

#include "icons/icons-manager.h"
#include "activate.h"
#include "debug.h"

#include "choose-description.h"

QMap<QWidget *, ChooseDescription *> ChooseDescription::Dialogs;

/**
 * Special value for position parameter of ChooseDescription::showDialog method. Causes the dialog to be positioned in the center of the desktop.
 */
QPoint ChooseDescription::ShowCentered = QPoint(-512000, -512000);

ChooseDescription * ChooseDescription::showDialog(const QList<StatusContainer *> &statusContainerList, const QPoint &position, QWidget *parent)
{
	if (statusContainerList.isEmpty())
		return 0;

	ChooseDescription *dialog;
	if (Dialogs.contains(parent))
		dialog = Dialogs[parent];
	else
	{
		dialog = new ChooseDescription(statusContainerList, parent);
		Dialogs[parent] = dialog;
	}

	if (position != ChooseDescription::ShowCentered)
		dialog->setPosition(position);
	else
		dialog->setPosition(QPoint((qApp->desktop()->screenGeometry().width() - dialog->sizeHint().width()) / 2,
				(qApp->desktop()->screenGeometry().height() - dialog->sizeHint().height()) / 2));

	dialog->show();
	_activateWindow(dialog);

	return dialog;
}

ChooseDescription::ChooseDescription(const QList<StatusContainer *> &statusContainerList, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), StatusContainers(statusContainerList)
{
	Q_ASSERT(!StatusContainers.isEmpty());

	FirstStatusContainer = StatusContainers.at(0);

	kdebugf();

	setWindowRole("kadu-choose-description");

	setWindowTitle(tr("Select description"));
	setAttribute(Qt::WA_DeleteOnClose);

	Description = new QComboBox(this);
	Description->setMaxVisibleItems(10);
	Description->setModel(DescriptionManager::instance()->model());
	Description->setEditable(true);
	Description->setInsertPolicy(QComboBox::NoInsert);
	Description->completer()->setCaseSensitivity(Qt::CaseSensitive);
	Description->setEditText(StatusSetter::instance()->manuallySetStatus(FirstStatusContainer).description());
	connect(Description, SIGNAL(activated(int)), this, SLOT(activated(int)));

	OkButton = new QPushButton(tr("&OK"), this);
	OkButton->setIcon(FirstStatusContainer->statusIcon().icon());
	OkButton->setDefault(true);
	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this);
	grid->addWidget(Description, 0, 0, 1, -1);

	int maxDescriptionLength = FirstStatusContainer->maxDescriptionLength();
	if (maxDescriptionLength > 0)
	{
		AvailableChars = new QLabel(this);
		Description->lineEdit()->setMaxLength(maxDescriptionLength);
		currentDescriptionChanged(Description->currentText());
		connect(Description, SIGNAL(editTextChanged(const QString &)), this, SLOT(currentDescriptionChanged(const QString &)));
		grid->addWidget(AvailableChars, 1, 0);
	}

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	grid->addWidget(spacer, 1, 1);

	grid->addWidget(OkButton, 1, 2, Qt::AlignRight);
	grid->addWidget(cancelButton, 1, 3, Qt::AlignRight);

	setMinimumSize(QDialog::sizeHint().expandedTo(QSize(250, 80)));

	connect(this, SIGNAL(accepted()), this, SLOT(setDescription()));
	connect(FirstStatusContainer, SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));

	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
	Dialogs.remove(parentWidget());
}

QSize ChooseDescription::sizeHint() const
{
	return QDialog::sizeHint().expandedTo(QSize(350, 80));
}

void ChooseDescription::setPosition(const QPoint &position)
{
	QSize sh = sizeHint();
	int width = sh.width();
	int height = sh.height();

	QDesktopWidget *d = QApplication::desktop();

	QPoint p = position;
	if (p.x() + width + 20 >= d->width())
		p.setX(d->width() - width - 20);
	if (p.y() + height + 20 >= d->height())
		p.setY(d->height() - height - 20);
	move(p);
}

void ChooseDescription::setDescription()
{
	QString description = Description->currentText();
	DescriptionManager::instance()->addDescription(description);

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, Talkable(Core::instance()->myself()), false);

	foreach (StatusContainer *container, StatusContainers)
	{
		Status status = StatusSetter::instance()->manuallySetStatus(container);
		status.setDescription(description);

		StatusSetter::instance()->setStatus(container, status);
		container->storeStatus(status);
	}
}

void ChooseDescription::activated(int index)
{
	// TODO: fix this workaround
	QString text = Description->model()->data(Description->model()->index(index, 0), Qt::DisplayRole).toString();
	Description->setEditText(text);
}

void ChooseDescription::currentDescriptionChanged(const QString &text)
{
	int length = text.length();
	AvailableChars->setText(' ' + QString::number(FirstStatusContainer->maxDescriptionLength() - length));
}

void ChooseDescription::statusUpdated()
{
	OkButton->setIcon(FirstStatusContainer->statusIcon().icon());
}
