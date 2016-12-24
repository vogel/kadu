/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QResizeEvent>
#include <QtWidgets/QFileDialog>
#include <QtXml/QDomElement>

#include "actions/action.h"
#include "actions/base-action-context.h"
#include "actions/chat-widget/bold-action.h"
#include "actions/chat-widget/italic-action.h"
#include "actions/chat-widget/underline-action.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "icons/icons-manager.h"
#include "identities/identity.h"
#include "misc/change-notifier-lock.h"
#include "misc/error.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image-service.h"
#include "status/status-configuration-holder.h"
#include "status/status-container-manager.h"
#include "widgets/chat-edit-box-size-manager.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/custom-input.h"
#include "widgets/talkable-tree-view.h"
#include "widgets/toolbar.h"
#include "windows/message-dialog.h"
#include "debug.h"

#include "chat-edit-box.h"

QList<ChatEditBox *> chatEditBoxes;

ChatEditBox::ChatEditBox(const Chat &chat, QWidget *parent) :
		MainWindow(new BaseActionContext(this), "chat", parent), CurrentChat(chat)
{
}

ChatEditBox::~ChatEditBox()
{
// 	disconnect(m_chatWidgetActions->colorSelector(), 0, this, 0);
	disconnect(InputBox, 0, this, 0);

	chatEditBoxes.removeAll(this);
}

void ChatEditBox::setBoldAction(BoldAction *boldAction)
{
	m_boldAction = boldAction;
}

void ChatEditBox::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
	m_chatConfigurationHolder = chatConfigurationHolder;
}

void ChatEditBox::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void ChatEditBox::setItalicAction(ItalicAction *italicAction)
{
	m_italicAction = italicAction;
}

void ChatEditBox::setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder)
{
	m_statusConfigurationHolder = statusConfigurationHolder;
}

void ChatEditBox::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void ChatEditBox::setUnderlineAction(UnderlineAction *underlineAction)
{
	m_underlineAction = underlineAction;
}

void ChatEditBox::init()
{
	chatEditBoxes.append(this);

	Context = static_cast<BaseActionContext *>(actionContext());

	ChangeNotifierLock lock(Context->changeNotifier());

	RoleSet roles;
	if (CurrentChat.contacts().size() > 1)
		roles.insert(ChatRole);
	else
		roles.insert(BuddyRole);
	Context->setRoles(roles);

	Context->setChat(CurrentChat);
	Context->setContacts(CurrentChat.contacts());
	Context->setBuddies(CurrentChat.contacts().toBuddySet());
	updateContext();

	connect(m_statusConfigurationHolder, SIGNAL(setStatusModeChanged()), this, SLOT(updateContext()));

	InputBox = injectedFactory()->makeInjected<CustomInput>(CurrentChat, this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);

	setCentralWidget(InputBox);

	bool old_top = loadOldToolBarsFromConfig("chatTopDockArea", Qt::TopToolBarArea);
	bool old_middle = loadOldToolBarsFromConfig("chatMiddleDockArea", Qt::TopToolBarArea);
	bool old_bottom = loadOldToolBarsFromConfig("chatBottomDockArea", Qt::BottomToolBarArea);
	bool old_left = loadOldToolBarsFromConfig("chatLeftDockArea", Qt::LeftToolBarArea);
	bool old_right = loadOldToolBarsFromConfig("chatRightDockArea", Qt::RightToolBarArea);

	if (old_top || old_middle || old_bottom || old_left || old_right)
		writeToolBarsToConfig(); // port old config
	else
		loadToolBarsFromConfig(); // load new config

// 	connect(m_chatWidgetActions->colorSelector(), SIGNAL(actionCreated(Action *)),
// 			this, SLOT(colorSelectorActionCreated(Action *)));
	connect(InputBox, SIGNAL(keyPressed(QKeyEvent *,CustomInput *, bool &)),
			this, SIGNAL(keyPressed(QKeyEvent *,CustomInput *,bool &)));
	connect(InputBox, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
	connect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	connect(m_chatConfigurationHolder, SIGNAL(chatConfigurationUpdated()), this, SLOT(configurationUpdated()));

	configurationUpdated();
}

void ChatEditBox::fontChanged(QFont font)
{
	if (m_boldAction->action(actionContext()))
		m_boldAction->action(actionContext())->setChecked(font.bold());
	if (m_italicAction->action(actionContext()))
		m_italicAction->action(actionContext())->setChecked(font.italic());
	if (m_underlineAction->action(actionContext()))
		m_underlineAction->action(actionContext())->setChecked(font.underline());
}

void ChatEditBox::colorSelectorActionCreated(Action *action)
{
	if (action->parent() == this)
		setColorFromCurrentText(true);
}

void ChatEditBox::cursorPositionChanged()
{
	setColorFromCurrentText(false);
}

void ChatEditBox::configurationUpdated()
{
	setColorFromCurrentText(true);

	InputBox->setAutoSend(m_chatConfigurationHolder->autoSend());
}

void ChatEditBox::setAutoSend(bool autoSend)
{
	InputBox->setAutoSend(autoSend);
}

CustomInput * ChatEditBox::inputBox()
{
	return InputBox;
}

bool ChatEditBox::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeUser);
}

TalkableProxyModel * ChatEditBox::talkableProxyModel()
{
	ChatWidget *cw = chatWidget();
	if (cw && cw->chat().contacts().count() > 1)
		return cw->talkableProxyModel();

	return 0;
}

void ChatEditBox::updateContext()
{
	if (m_statusConfigurationHolder->isSetStatusPerIdentity())
		Context->setStatusContainer(CurrentChat.chatAccount().accountIdentity().statusContainer());
	else if (m_statusConfigurationHolder->isSetStatusPerAccount())
		Context->setStatusContainer(CurrentChat.chatAccount().statusContainer());
	else
		Context->setStatusContainer(m_statusContainerManager);
}

ChatWidget * ChatEditBox::chatWidget()
{
	ChatWidget *result = qobject_cast<ChatWidget *>(parentWidget());
	if (result)
		return result;

	result = qobject_cast<ChatWidget *>(parent()->parent());
	if (result)
		return result;

	return 0;
}

void ChatEditBox::createDefaultToolbars(Configuration *configuration, QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(configuration, toolbarsConfig, "chat_topDockArea");
	QDomElement toolbarConfig = configuration->api()->createElement(dockAreaConfig, "ToolBar");

	addToolButton(configuration, toolbarConfig, "autoSendAction");
	addToolButton(configuration, toolbarConfig, "clearChatAction");
	addToolButton(configuration, toolbarConfig, "insertEmoticonAction", Qt::ToolButtonTextBesideIcon);
	addToolButton(configuration, toolbarConfig, "insertImageAction");
	addToolButton(configuration, toolbarConfig, "showHistoryAction");
	addToolButton(configuration, toolbarConfig, "encryptionAction");
	addToolButton(configuration, toolbarConfig, "editUserAction");
	addToolButton(configuration, toolbarConfig, "__spacer1", Qt::ToolButtonTextBesideIcon);
	addToolButton(configuration, toolbarConfig, "sendAction", Qt::ToolButtonTextBesideIcon);
}

void ChatEditBox::openInsertImageDialog()
{
	ChatImageService *chatImageService = CurrentChat.chatAccount().protocolHandler()->chatImageService();
	if (!chatImageService)
		return;

	// QTBUG-849
	QString selectedFile = QFileDialog::getOpenFileName(this, tr("Insert image"), configuration()->deprecatedApi()->readEntry("Chat", "LastImagePath"),
							tr("Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP);;All Files (*)"));
	if (!selectedFile.isEmpty())
	{
		QFileInfo f(selectedFile);

		configuration()->deprecatedApi()->writeEntry("Chat", "LastImagePath", f.absolutePath());

		if (!f.isReadable())
		{
			MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("This file is not readable"), QMessageBox::Ok, this);
			return;
		}

		Error imageSizeError = chatImageService->checkImageSize(f.size());
		if (!imageSizeError.message().isEmpty())
		{
			MessageDialog *dialog = MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), imageSizeError.message(), this);
			dialog->addButton(QMessageBox::Yes, tr("Send anyway"));
			dialog->addButton(QMessageBox::No, tr("Cancel"));

			switch (imageSizeError.severity())
			{
				case NoError:
					break;
				case ErrorLow:
					if (dialog->ask())
						return;
					break;
				case ErrorHigh:
					MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-error")), tr("Kadu"), imageSizeError.message(), QMessageBox::Ok, this);
					return;
				default:
					break;
			}
		}

		int tooBigCounter = 0;
		int disconnectedCounter = 0;

		foreach (const Contact &contact, CurrentChat.contacts())
		{
			if (contact.currentStatus().isDisconnected())
				disconnectedCounter++;
			else if (contact.maximumImageSize() == 0 || contact.maximumImageSize() * 1024 < f.size())
				tooBigCounter++;
		}

		QString message;
		if (1 == CurrentChat.contacts().count())
		{
			Contact contact = *CurrentChat.contacts().constBegin();
			if (tooBigCounter > 0)
				message = tr("This image has %1 KiB and may be too big for %2.")
						.arg((f.size() + 1023) / 1024).arg(contact.display(true)) + '\n';
			else if (disconnectedCounter > 0)
				message = tr("%1 appears to be offline and may not receive images.").arg(contact.display(true)) + '\n';
		}
		else
		{
			if (tooBigCounter > 0)
				message = tr("This image has %1 KiB and may be too big for %2 of %3 contacts in this conference.")
						.arg((f.size() + 1023) / 1024).arg(tooBigCounter).arg(CurrentChat.contacts().count()) + '\n';
			if (disconnectedCounter > 0)
				message += tr("%1 of %2 contacts appear to be offline and may not receive images.").arg(disconnectedCounter).arg(CurrentChat.contacts().count()) + '\n';
		}
		if (tooBigCounter > 0 || disconnectedCounter > 0)
			message += tr("Do you really want to send this image?");

		MessageDialog *dialog = MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-question")), tr("Kadu"), message, this);
		dialog->addButton(QMessageBox::Yes, tr("Send anyway"));
		dialog->addButton(QMessageBox::No, tr("Cancel"));

		if (!message.isEmpty() && !dialog->ask())
			return;

		InputBox->insertHtml(QString("<img src='%1' />").arg(selectedFile));
	}
}

void ChatEditBox::changeColor(const QColor &newColor)
{
	CurrentColor = newColor;

	QPixmap p(12, 12);
	p.fill(CurrentColor);

// 	Action *action = m_chatWidgetActions->colorSelector()->action(this);
// 	if (action)
// 		action->setIcon(p);

	InputBox->setTextColor(CurrentColor);
}

void ChatEditBox::setColorFromCurrentText(bool force)
{
	Q_UNUSED(force);

/*
	Action *action = m_chatWidgetActions->colorSelector()->action(this);
	if (!action || (!force && (InputBox->textColor() == CurrentColor)))
		return;

	int i;
	for (i = 0; i < 16; ++i)
		if (InputBox->textColor() == QColor(colors[i]))
			break;

	QPixmap p(12, 12);
	if (i >= 16)
		CurrentColor = InputBox->palette().foreground().color();
	else
		CurrentColor = colors[i];

	p.fill(CurrentColor);

	action->QAction::setIcon(p);
*/
}

void ChatEditBox::insertPlainText(const QString &plainText)
{
	InputBox->insertPlainText( plainText);
}

#include "moc_chat-edit-box.cpp"
