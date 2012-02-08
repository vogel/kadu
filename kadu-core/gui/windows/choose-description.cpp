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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

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
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

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

	QString windowTitle = StatusContainers.count() > 1
		? tr("Account status")
		: tr("Account status: %1").arg(FirstStatusContainer->statusContainerName());
	setWindowTitle(windowTitle);
	setAttribute(Qt::WA_DeleteOnClose);

	QFormLayout *layout = new QFormLayout(this);

	statusCombo = new QComboBox(this);
	layout->addRow(new QLabel(tr("Status") + ':'), statusCombo);

	QList<StatusType> statusTypes = FirstStatusContainer->supportedStatusTypes();
	int selectedIndex, i = 0;

	foreach (StatusType statusType, statusTypes)
	{
		if (StatusTypeNone == statusType)
			continue;

		const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(statusType);

		KaduIcon icon = FirstStatusContainer->statusIcon(typeData.type());
		statusCombo->addItem(icon.icon(), typeData.displayName(), QVariant::fromValue(typeData.type()));

		if (typeData.type() == FirstStatusContainer->status().type())
			selectedIndex = i;
		i++;
	}
	statusCombo->setCurrentIndex(selectedIndex);

	DescriptionEdit = new QTextEdit(this);
	DescriptionEdit->setPlainText(StatusSetter::instance()->manuallySetStatus(FirstStatusContainer).description());
	layout->addRow(new QLabel(tr("Description") + ':'), DescriptionEdit);

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	int maxDescriptionLength = FirstStatusContainer->maxDescriptionLength();
	if (maxDescriptionLength > 0)
	{
		AvailableChars = new QLabel(this);
		currentDescriptionChanged();
		connect(DescriptionEdit, SIGNAL(textChanged()), this, SLOT(currentDescriptionChanged()));
		layout->addRow(spacer, AvailableChars);
	}

	QPushButton *chooseButton = new QPushButton(tr("Choose description..."), this);
	layout->addRow(spacer, chooseButton);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	OkButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&OK"), this);
	OkButton->setDefault(true);
	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	
	layout->addWidget(buttons);

	setMinimumSize(QDialog::sizeHint().expandedTo(QSize(250, 80)));

	connect(this, SIGNAL(accepted()), this, SLOT(setDescription()));

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
	QString description = DescriptionEdit->toPlainText();
	DescriptionManager::instance()->addDescription(description);

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, Talkable(Core::instance()->myself()), false);

	foreach (StatusContainer *container, StatusContainers)
	{
		Status status = StatusSetter::instance()->manuallySetStatus(container);
		status.setDescription(description);

		StatusType statusType = statusCombo->itemData(statusCombo->currentIndex()).value<StatusType>();
		status.setType(statusType);

		StatusSetter::instance()->setStatus(container, status);
		container->storeStatus(status);
	}
}

void ChooseDescription::activated(int index)
{
// 	// TODO: fix this workaround
// 	QString text = Description->model()->data(Description->model()->index(index, 0), Qt::DisplayRole).toString();
// 	Description->setEditText(text);
}

void ChooseDescription::currentDescriptionChanged()
{
	int length = DescriptionEdit->toPlainText().length();
	int charactersLeft = FirstStatusContainer->maxDescriptionLength() - length;
	if (OkButton)
		OkButton->setEnabled(charactersLeft >= 0);
	AvailableChars->setText(' ' + QString::number(charactersLeft));
}
