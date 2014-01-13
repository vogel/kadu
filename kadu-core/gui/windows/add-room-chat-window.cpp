/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "accounts/filter/protocol-filter.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-room.h"
#include "configuration/config-file-variant-wrapper.h"
#include "core/core.h"
#include "gui/widgets/accounts-combo-box.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "icons/kadu-icon.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol.h"

#include "add-room-chat-window.h"

AddRoomChatWindow::AddRoomChatWindow(QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowRole("kadu-add-room-chat");
	setWindowTitle(tr("Join Room"));

	createGui();

	validateData();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "AddRoomChatWindowGeometry"), QRect(0, 50, 430, 250), this);
}

AddRoomChatWindow::~AddRoomChatWindow()
{
}

void AddRoomChatWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *mainWidget = new QWidget(this);
	mainLayout->addWidget(mainWidget);

	QFormLayout *layout = new QFormLayout(mainWidget);

	AccountCombo = new AccountsComboBox(true, AccountsComboBox::NotVisibleWithOneRowSourceModel, this);
	AccountCombo->setIncludeIdInDisplay(true);

	// only xmpp rooms for now
	// we need to add something like Protocol::supporterChatTypes()
	ProtocolFilter *protocolFilter = new ProtocolFilter(AccountCombo);
	protocolFilter->setProtocolName("jabber");
	AccountCombo->addFilter(protocolFilter);
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(validateData()));

	layout->addRow(tr("Account:"), AccountCombo);

	DisplayNameEdit = new QLineEdit(this);
	connect(DisplayNameEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Visible name:"), DisplayNameEdit);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this conference if you want to have it on roster"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 2);
	hintLabel->setFont(hintLabelFont);
	layout->addRow(0, hintLabel);

	ErrorLabel = new QLabel(this);
	QFont labelFont = ErrorLabel->font();
	labelFont.setBold(true);
	ErrorLabel->setFont(labelFont);
	mainLayout->addWidget(ErrorLabel);

	RoomEdit = new QLineEdit(this);
	connect(RoomEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Room:"), RoomEdit);

	NickEdit = new QLineEdit(this);
	connect(NickEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Nick:"), NickEdit);

	PasswordEdit = new QLineEdit(this);
	PasswordEdit->setEchoMode(QLineEdit::Password);
	connect(PasswordEdit, SIGNAL(textChanged(QString)), this, SLOT(validateData()));

	layout->addRow(tr("Password:"), PasswordEdit);

	QDialogButtonBox *buttons = new QDialogButtonBox(mainWidget);

	AddButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Add Room Chat"), this);
	AddButton->setDefault(true);
	connect(AddButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	StartButton = new QPushButton(KaduIcon("internet-group-chat").icon(), tr("Start Room Chat"), this);
	connect(StartButton, SIGNAL(clicked(bool)), this, SLOT(start()));

	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(StartButton, QDialogButtonBox::ActionRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

	mainLayout->addSpacing(32);
	mainLayout->addWidget(buttons);

	if (AccountCombo->currentAccount())
		DisplayNameEdit->setFocus();
	else
		AccountCombo->setFocus();
}

void AddRoomChatWindow::displayErrorMessage(const QString &message)
{
	ErrorLabel->setText(message);
}

void AddRoomChatWindow::validateData()
{
	AddButton->setEnabled(false);
	StartButton->setEnabled(false);

	Account account = AccountCombo->currentAccount();
	if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->protocolFactory())
	{
		displayErrorMessage(tr("Account is not selected"));
		return;
	}

	if (RoomEdit->text().isEmpty())
	{
		displayErrorMessage(tr("Enter room name"));
		return;
	}

	if (NickEdit->text().isEmpty())
	{
		displayErrorMessage(tr("Enter nick"));
		return;
	}

	StartButton->setEnabled(true);

	const QString &display = DisplayNameEdit->text();
	if (!display.isEmpty() && ChatManager::instance()->byDisplay(display))
	{
		displayErrorMessage(tr("Visible name is already used for another chat"));
		return;
	}

	const Chat &chat = computeChat();
	Q_ASSERT(!chat.isNull());

	if (!chat.display().isEmpty())
	{
		displayErrorMessage(tr("This room chat is already available as <i>%1</i>").arg(chat.display()));
		return;
	}

	if (display.isEmpty())
		displayErrorMessage(tr("Enter visible name to add this room chat to roster"));
	else
		displayErrorMessage(QString());

	AddButton->setEnabled(!display.isEmpty());
}

Chat AddRoomChatWindow::computeChat() const
{
	Chat chat = ChatTypeRoom::findChat(AccountCombo->currentAccount(), RoomEdit->text(), ActionCreateAndAdd);
	if (!chat)
		return Chat::null;

	return chat;
}

void AddRoomChatWindow::accept()
{
	const Chat &chat = computeChat();
	Q_ASSERT(!chat.isNull());

	chat.setDisplay(DisplayNameEdit->text());

	ChatDetailsRoom *details = qobject_cast<ChatDetailsRoom *>(chat.details());
	Q_ASSERT(details);

	details->setNick(NickEdit->text());
	details->setPassword(PasswordEdit->text());

	QDialog::accept();
}

void AddRoomChatWindow::start()
{
	const Chat &chat = computeChat();
	Q_ASSERT(!chat.isNull());

	// only update display chat name when not empty
	// this method can be called even when uer did not enter display name
	if (!DisplayNameEdit->text().isEmpty())
		chat.setDisplay(DisplayNameEdit->text());

	Core::instance()->chatWidgetManager()->openChat(computeChat(), OpenChatActivation::Activate);

	ChatDetailsRoom *details = qobject_cast<ChatDetailsRoom *>(chat.details());
	Q_ASSERT(details);

	details->setNick(NickEdit->text());
	details->setPassword(PasswordEdit->text());

	QDialog::accept();
}

#include "moc_add-room-chat-window.cpp"
