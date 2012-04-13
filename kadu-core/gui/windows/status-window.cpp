/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/kadu-text-edit.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/previous-descriptions-window.h"
#include "icons/kadu-icon.h"
#include "parser/parser.h"
#include "status/description-manager.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#include "activate.h"
#include "debug.h"

#include "status-window.h"

QMap<StatusContainer *, StatusWindow *> StatusWindow::Dialogs;

StatusWindow * StatusWindow::showDialog(StatusContainer *statusContainer, QWidget *parent)
{
	if (!statusContainer)
		return 0;

	StatusWindow *dialog;
	if (Dialogs.contains(statusContainer))
		dialog = Dialogs.value(statusContainer);
	else
	{
		dialog = new StatusWindow(statusContainer, parent);
		Dialogs.insert(statusContainer, dialog);
	}

	dialog->show();
	_activateWindow(dialog);

	return dialog;
}

StatusWindow::StatusWindow(StatusContainer *statusContainer, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), Container(statusContainer)
{
	Q_ASSERT(Container);

	kdebugf();

	setWindowRole("kadu-status-window");

	QString windowTitle = Container->subStatusContainers().count() > 1
		? tr("Change status")
		: tr("Change account status: %1").arg(Container->statusContainerName());
	setWindowTitle(windowTitle);
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	QFormLayout *layout = new QFormLayout(formWidget);
	layout->setMargin(0);

	StatusList = new QComboBox(formWidget);
	layout->addRow(new QLabel(tr("Status") + ':'), StatusList);

	QList<StatusType> statusTypes = Container->supportedStatusTypes();
	int selectedIndex = -1, i = 0;

	foreach (StatusType statusType, statusTypes)
	{
		if (StatusTypeNone == statusType)
			continue;

		const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(statusType);

		KaduIcon icon = Container->statusIcon(typeData.type());
		StatusList->addItem(icon.icon(), typeData.displayName(), QVariant::fromValue(typeData.type()));

		if (typeData.type() == Container->status().type())
			selectedIndex = i;
		i++;
	}
	Q_ASSERT(selectedIndex != -1);
	StatusList->setCurrentIndex(selectedIndex);

	DescriptionEdit = new KaduTextEdit(formWidget);
	DescriptionEdit->installEventFilter(this);
	DescriptionEdit->setPlainText(StatusSetter::instance()->manuallySetStatus(Container).description());
	DescriptionEdit->setFocus();
	DescriptionEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	DescriptionEdit->setTabChangesFocus(true);

	QWidget *descriptionLabelWidget = new QWidget(formWidget);
	QVBoxLayout *descriptionLabelLayout = new QVBoxLayout(descriptionLabelWidget);
	descriptionLabelLayout->setMargin(0);
	descriptionLabelLayout->setSpacing(5);

	QLabel *descriptionLabel = new QLabel(tr("Description") + ':', descriptionLabelWidget);
	descriptionLabel->setAlignment(layout->labelAlignment());

	DescriptionLimitCounter = new QLabel(formWidget);
	DescriptionLimitCounter->setVisible(false);
	DescriptionLimitCounter->setAlignment(layout->labelAlignment());

	descriptionLabelLayout->addWidget(descriptionLabel);
	descriptionLabelLayout->addWidget(DescriptionLimitCounter);
	descriptionLabelLayout->addStretch(1);

	layout->addRow(descriptionLabelWidget, DescriptionEdit);

	QPushButton *chooseButton = new QPushButton(tr("Choose description..."), formWidget);
	connect(chooseButton, SIGNAL(clicked(bool)), this, SLOT(openDescriptionsList()));
	layout->addRow(0, chooseButton);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, formWidget);

	OkButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Set status"), buttons);
	OkButton->setDefault(true);
	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), buttons);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QPushButton *clearButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"), buttons);
	connect(clearButton, SIGNAL(clicked(bool)), DescriptionEdit, SLOT(clear()));

	buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	buttons->addButton(clearButton, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(formWidget);
	mainLayout->addSpacing(16);
	mainLayout->addWidget(buttons);

	setFixedSize(sizeHint().expandedTo(QSize(250, 80)));

	connect(this, SIGNAL(accepted()), this, SLOT(applyStatus()));

	int maxDescriptionLength = Container->maxDescriptionLength();
	if (maxDescriptionLength > 0)
	{
		DescriptionLimitCounter->setVisible(true);
		connect(DescriptionEdit, SIGNAL(textChanged()), this, SLOT(checkDescriptionLengthLimit()));
		checkDescriptionLengthLimit();
	}

	kdebugf2();
}

StatusWindow::~StatusWindow()
{
	Dialogs.remove(Container);
}

QSize StatusWindow::sizeHint() const
{
	return QDialog::sizeHint().expandedTo(QSize(400, 80));
}

void StatusWindow::applyStatus()
{
	QString description = DescriptionEdit->toPlainText();
	DescriptionManager::instance()->addDescription(description);

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, Talkable(Core::instance()->myself()), false);

	foreach (StatusContainer *container, Container->subStatusContainers())
	{
		Status status = StatusSetter::instance()->manuallySetStatus(container);
		status.setDescription(description);

		StatusType statusType = StatusList->itemData(StatusList->currentIndex()).value<StatusType>();
		status.setType(statusType);

		StatusSetter::instance()->setStatus(container, status);
		container->storeStatus(status);
	}
}

void StatusWindow::descriptionSelected(const QString &description)
{
	DescriptionEdit->setPlainText(description);
}

bool StatusWindow::eventFilter(QObject *source, QEvent *event)
{
	if (source != DescriptionEdit)
		return false;

	if (event->type() != QEvent::KeyPress)
		return false;

	QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	if (!keyEvent)
		return false;

	if (Qt::ControlModifier == keyEvent->modifiers() &&
	    (Qt::Key_Enter == keyEvent->key() || Qt::Key_Return == keyEvent->key()))
	{
		accept();
		return true;
	}

	return false;
}

void StatusWindow::openDescriptionsList()
{
	PreviousDescriptionsWindow *chooseDescDialog = new PreviousDescriptionsWindow(this);
	connect(chooseDescDialog, SIGNAL(descriptionSelected(const QString &)), this, SLOT(descriptionSelected(const QString &)));
	chooseDescDialog->exec();
}

void StatusWindow::checkDescriptionLengthLimit()
{
	int length = DescriptionEdit->toPlainText().length();
	int charactersLeft = Container->maxDescriptionLength() - length;
	bool limitExceeded = charactersLeft < 0;

	OkButton->setEnabled(!limitExceeded);
	DescriptionLimitCounter->setText(QString("(%1)").arg(charactersLeft));
}
