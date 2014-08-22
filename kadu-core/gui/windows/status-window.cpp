/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/kadu-text-edit.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/status-window-description-proxy-model.h"
#include "icons/kadu-icon.h"
#include "parser/parser.h"
#include "status/description-manager.h"
#include "status/description-model.h"
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
		QDialog(parent), DesktopAwareObject(this), Container(statusContainer), IgnoreNextTextChange(false)
{
	Q_ASSERT(Container);

	kdebugf();

	setWindowRole("kadu-status-window");

	QString windowTitle = Container->subStatusContainers().count() > 1
		? tr("Change status")
		: tr("Change account status: %1").arg(Container->statusContainerName());
	setWindowTitle(windowTitle);
	setAttribute(Qt::WA_DeleteOnClose);

	createLayout();

	DescriptionCounter->setVisible(Container->maxDescriptionLength() > 0);

	setupStatusSelect();

	QString description = StatusSetter::instance()->manuallySetStatus(Container->subStatusContainers().first()).description();

	setupDescriptionSelect(description);

	IgnoreNextTextChange = true;
	DescriptionEdit->setPlainText(description);
	descriptionEditTextChanged(); // not connected yet
	IgnoreNextTextChange = false;

	QTextCursor cursor = DescriptionEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	DescriptionEdit->setTextCursor(cursor);

	DescriptionEdit->setFocus();

	connect(DescriptionSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(descriptionSelected(int)));
	connect(ClearDescriptionsHistoryButton, SIGNAL(clicked(bool)), this, SLOT(clearDescriptionsHistory()));
	connect(DescriptionEdit, SIGNAL(textChanged()), this, SLOT(descriptionEditTextChanged()));
	connect(EraseButton, SIGNAL(clicked(bool)), this, SLOT(eraseDescription()));
	connect(SetStatusButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
	connect(this, SIGNAL(accepted()), this, SLOT(applyStatus()));

	setFixedSize(sizeHint().expandedTo(QSize(460, 1)));

	kdebugf2();
}

StatusWindow::~StatusWindow()
{
	Dialogs.remove(Container);
}

void StatusWindow::createLayout()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QFormLayout *formLayout = new QFormLayout();
	mainLayout->addLayout(formLayout);
	formLayout->setMargin(0);
	formLayout->setVerticalSpacing(0);

	// status combo box

	StatusSelect = new QComboBox(this);
	formLayout->addRow(new QLabel(tr("Status") + ':'), StatusSelect);

	// spacing

	formLayout->addItem(new QSpacerItem(0, 4));

	// description combo box

	QHBoxLayout *descriptionSelectLayout = new QHBoxLayout();
	descriptionSelectLayout->setMargin(0);
	descriptionSelectLayout->setSpacing(0);

	DescriptionSelect = new QComboBox(this);
	DescriptionSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	DescriptionSelect->setToolTip(tr("Select Previously Used Description"));
	descriptionSelectLayout->addWidget(DescriptionSelect);

	ClearDescriptionsHistoryButton = new QPushButton(KaduIcon("edit-clear").icon(), "", this);
	ClearDescriptionsHistoryButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	ClearDescriptionsHistoryButton->setToolTip(tr("Clear Descriptions History"));
	descriptionSelectLayout->addWidget(ClearDescriptionsHistoryButton);

	formLayout->addRow(new QLabel(tr("Description") + ':'), descriptionSelectLayout);

	// description edit field

	QWidget *descriptionCounterLayoutWidget = new QWidget(this);
	QVBoxLayout *descriptionCounterLayout = new QVBoxLayout(descriptionCounterLayoutWidget);
	descriptionCounterLayout->setMargin(0);
	descriptionCounterLayout->setSpacing(5);

	descriptionCounterLayout->addStretch();

	DescriptionCounter = new QLabel(this);
	DescriptionCounter->setAlignment(formLayout->labelAlignment());
	descriptionCounterLayout->addWidget(DescriptionCounter);

	descriptionCounterLayout->addSpacing(2); // 2 px bottom margin

	QWidget *descriptionEditLayoutWidget = new QWidget(this);
	QHBoxLayout *descriptionEditLayout = new QHBoxLayout(descriptionEditLayoutWidget);
	descriptionEditLayout->setMargin(0);
	descriptionEditLayout->setSpacing(0);

	DescriptionEdit = new KaduTextEdit(this);
	DescriptionEdit->installEventFilter(this);
	DescriptionEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	DescriptionEdit->setTabChangesFocus(true);
	descriptionEditLayout->addWidget(DescriptionEdit);

	QVBoxLayout *descriptionEraseLayout = new QVBoxLayout();
	descriptionEraseLayout->setMargin(0);
	descriptionEraseLayout->setSpacing(0);
	descriptionEraseLayout->addStretch();
	EraseButton = new QPushButton(KaduIcon("edit-clear-locationbar-rtl").icon(), "", this);
	EraseButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	EraseButton->setToolTip(tr("Erase Description"));
	descriptionEraseLayout->addWidget(EraseButton);
	descriptionEditLayout->addLayout(descriptionEraseLayout);

	formLayout->addRow(descriptionCounterLayoutWidget, descriptionEditLayoutWidget);

	mainLayout->addSpacing(16);

	// dialog buttons

	QDialogButtonBox *buttonsBox = new QDialogButtonBox(Qt::Horizontal, this);

	SetStatusButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Set status"), this);
	SetStatusButton->setDefault(true);
	buttonsBox->addButton(SetStatusButton, QDialogButtonBox::AcceptRole);

	CancelButton = new QPushButton(tr("&Cancel"), this);
	CancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	buttonsBox->addButton(CancelButton, QDialogButtonBox::RejectRole);

	mainLayout->addWidget(buttonsBox);
}

void StatusWindow::setupStatusSelect()
{
	StatusType commonStatusType = findCommonStatusType(Container->subStatusContainers());
	if (commonStatusType == StatusTypeNone)
		StatusSelect->addItem(tr("do not change"), QVariant(StatusTypeNone));

	foreach (StatusType statusType, Container->supportedStatusTypes())
	{
		if (StatusTypeNone == statusType)
			continue;
		const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(statusType);
		StatusSelect->addItem(Container->statusIcon(typeData.type()).icon(), typeData.displayName(), QVariant(typeData.type()));
	}

	StatusSelect->setCurrentIndex(StatusSelect->findData(QVariant(commonStatusType)));
}

void StatusWindow::setupDescriptionSelect(const QString &description)
{
	StatusWindowDescriptionProxyModel *proxyModel = new StatusWindowDescriptionProxyModel(this);
	proxyModel->setSourceModel(DescriptionManager::instance()->model());

	DescriptionSelect->setModel(proxyModel);
	DescriptionSelect->setEnabled(false);
	ClearDescriptionsHistoryButton->setEnabled(false);

	Q_ASSERT(Container->subStatusContainers().count() > 0);

	if (DescriptionManager::instance()->model()->rowCount() > 0)
	{
		DescriptionSelect->setEnabled(true);
		ClearDescriptionsHistoryButton->setEnabled(true);

		QModelIndexList matching = DescriptionManager::instance()->model()->match(
			DescriptionManager::instance()->model()->index(0, 0),
			DescriptionRole, QVariant(description),
			1,
			Qt::MatchFixedString | Qt::MatchCaseSensitive
		);
		if (matching.count() > 0)
			DescriptionSelect->setCurrentIndex(matching.first().row());
		else
			DescriptionSelect->setCurrentIndex(-1);
	}
}

QSize StatusWindow::sizeHint() const
{
	return QDialog::sizeHint().expandedTo(QSize(400, 80));
}

StatusType StatusWindow::findCommonStatusType(const QList<StatusContainer *> &containers)
{
	StatusType commonStatusType = StatusTypeNone;
	foreach (StatusContainer *container, containers)
	{
		StatusType statusType = container->status().type();
		if (commonStatusType == StatusTypeNone)
			commonStatusType = statusType;
		else if (commonStatusType != statusType)
		{
			commonStatusType = StatusTypeNone;
			break;
		}
	}
	return commonStatusType;
}

void StatusWindow::applyStatus()
{
	disconnect(DescriptionSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(descriptionSelected(int)));

	QString description = DescriptionEdit->toPlainText();
	DescriptionManager::instance()->addDescription(description);

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, Talkable(Core::instance()->myself()), ParserEscape::NoEscape);

	foreach (StatusContainer *container, Container->subStatusContainers())
	{
		Status status = StatusSetter::instance()->manuallySetStatus(container);
		status.setDescription(description);

		StatusType statusType = static_cast<StatusType>(StatusSelect->itemData(StatusSelect->currentIndex()).toInt());
		if (statusType != StatusTypeNone)
			status.setType(statusType);

		StatusSetter::instance()->setStatus(container, status);
		container->storeStatus(status);
	}
}

void StatusWindow::descriptionSelected(int index)
{
	if (index < 0)
		return;

	QString description = DescriptionManager::instance()->model()->data(DescriptionManager::instance()->model()->index(index, 0), DescriptionRole).toString();

	IgnoreNextTextChange = true;
	DescriptionEdit->setPlainText(description);
	IgnoreNextTextChange = false;

	QTextCursor cursor = DescriptionEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	DescriptionEdit->setTextCursor(cursor);

	DescriptionEdit->setFocus();
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

void StatusWindow::checkDescriptionLengthLimit()
{
	int length = DescriptionEdit->toPlainText().length();
	int charactersLeft = Container->maxDescriptionLength() - length;
	bool limitExceeded = charactersLeft < 0;

	SetStatusButton->setEnabled(!limitExceeded);

	QString counterText = QString("%1").arg(charactersLeft);
	QColor color;
	if (charactersLeft >= 0)
	{
		color = palette().windowText().color();
		color.setAlpha(128);
	}
	else
		color = Qt::red;
	QString counterStyle = QString("color:rgba(%1,%2,%3,%4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
	DescriptionCounter->setText(QString("<span style='%1'>%2</span>").arg(counterStyle, counterText));
}

void StatusWindow::descriptionEditTextChanged()
{
	if (!IgnoreNextTextChange)
		DescriptionSelect->setCurrentIndex(-1);

	EraseButton->setEnabled(!DescriptionEdit->toPlainText().isEmpty());

	if (Container->maxDescriptionLength() > 0)
		checkDescriptionLengthLimit();
}

void StatusWindow::eraseDescription()
{
	DescriptionEdit->clear();
	DescriptionEdit->setFocus();
}

void StatusWindow::clearDescriptionsHistory()
{
	MessageDialog *dialog = MessageDialog::create(
		KaduIcon("dialog-warning"),
		tr("Clear Descriptions History"),
		tr("Do you really want to clear the descriptions history?"),
		this);
	dialog->addButton(QMessageBox::Yes, tr("Clear history"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (!dialog->ask())
		return;

	DescriptionManager::instance()->clearDescriptions();
	DescriptionSelect->setModel(DescriptionManager::instance()->model());
	DescriptionSelect->setCurrentIndex(-1);
	DescriptionSelect->setEnabled(false);
	ClearDescriptionsHistoryButton->setEnabled(false);
}

#include "moc_status-window.cpp"
