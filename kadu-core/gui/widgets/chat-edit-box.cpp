/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QFileDialog>
#include <QtGui/QResizeEvent>
#include <QtXml/QDomElement>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "emoticons/emoticon-selector.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/windows/message-dialog.h"
#include "protocols/services/chat-image-service.h"

#include "chat-widget.h"
#include "custom-input.h"
#include "debug.h"
#include "toolbar.h"

#include "chat-edit-box.h"

QList<ChatEditBox *> chatEditBoxes;

ChatEditBox::ChatEditBox(const Chat &chat, QWidget *parent) :
		MainWindow("chat", parent), CurrentChat(chat)
{
	chatEditBoxes.append(this);

	InputBox = new CustomInput(this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);
#ifdef Q_WS_MAEMO_5
	InputBox->setMinimumHeight(64);
#endif
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

// 	connect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(Action *)),
// 			this, SLOT(colorSelectorActionCreated(Action *)));
	connect(InputBox, SIGNAL(keyPressed(QKeyEvent *,CustomInput *, bool &)),
			this, SIGNAL(keyPressed(QKeyEvent *,CustomInput *,bool &)));
	connect(InputBox, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
	connect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	configurationUpdated();
}

ChatEditBox::~ChatEditBox()
{
// 	disconnect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(Action *)),
// 			this, SLOT(colorSelectorActionCreated(Action *)));
	disconnect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	chatEditBoxes.removeAll(this);
}

void ChatEditBox::fontChanged(QFont font)
{
	if (ChatWidgetManager::instance()->actions()->bold()->action(this))
		ChatWidgetManager::instance()->actions()->bold()->action(this)->setChecked(font.bold());
	if (ChatWidgetManager::instance()->actions()->italic()->action(this))
		ChatWidgetManager::instance()->actions()->italic()->action(this)->setChecked(font.italic());
	if (ChatWidgetManager::instance()->actions()->underline()->action(this))
		ChatWidgetManager::instance()->actions()->underline()->action(this)->setChecked(font.underline());
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

	InputBox->setAutoSend(config_file.readBoolEntry("Chat", "AutoSend"));
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

BuddiesListView * ChatEditBox::buddiesListView()
{
	ChatWidget *cw = chatWidget();
	if (cw && cw->chat().contacts().count() > 1)
		return cw->contactsListWidget();

	return 0;
}

StatusContainer * ChatEditBox::statusContainer()
{
	return CurrentChat.chatAccount().statusContainer();
}

ContactSet ChatEditBox::contacts()
{
	ChatWidget *cw = chatWidget();
	if (cw)
		return cw->chat().contacts();

	return ContactSet();
}

BuddySet ChatEditBox::buddies()
{
	return contacts().toBuddySet();
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

void ChatEditBox::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "chat_topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "autoSendAction");
	addToolButton(toolbarConfig, "clearChatAction");
	addToolButton(toolbarConfig, "insertEmoticonAction", Qt::ToolButtonTextBesideIcon);

#ifndef Q_WS_MAEMO_5
	addToolButton(toolbarConfig, "insertImageAction");
	addToolButton(toolbarConfig, "showHistoryAction");
	addToolButton(toolbarConfig, "encryptionAction");
	addToolButton(toolbarConfig, "editUserAction");
#endif

	addToolButton(toolbarConfig, "__spacer1", Qt::ToolButtonTextBesideIcon);
	addToolButton(toolbarConfig, "sendAction", Qt::ToolButtonTextBesideIcon);
}

void ChatEditBox::openEmoticonSelector(const QWidget *activatingWidget)
{
	//emoticons_selector zawsze b�dzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	EmoticonSelector *emoticonSelector = new EmoticonSelector(activatingWidget, this);
	connect(emoticonSelector, SIGNAL(emoticonSelect(const QString &)), this, SLOT(addEmoticon(const QString &)));
	emoticonSelector->show();
}

void ChatEditBox::openColorSelector(const QWidget *activatingWidget)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	ColorSelector *colorSelector = new ColorSelector(InputBox->palette().foreground().color(), activatingWidget, this);
	connect(colorSelector, SIGNAL(colorSelect(const QColor &)), this, SLOT(changeColor(const QColor &)));
	colorSelector->show();
}

void ChatEditBox::openInsertImageDialog()
{
	QString selectedFile = QFileDialog::getOpenFileName(this, tr("Insert image"), config_file.readEntry("Chat", "LastImagePath"),
							tr("Images") + " (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP)");
	if (!selectedFile.isEmpty())
	{
		QFileInfo f(selectedFile);

		config_file.writeEntry("Chat", "LastImagePath", f.absolutePath());

		if (!f.isReadable())
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("This file is not readable"), QMessageBox::Ok, this);
			return;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), QMessageBox::Ok, this);
			return;
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
			Contact contact = *CurrentChat.contacts().begin();
			if (tooBigCounter > 0)
				message = tr("This file is too big for %1.\nDo you really want to send this image?").arg(contact.ownerBuddy().display());
			else if (disconnectedCounter > 0)
				message = tr("%1 is disconnected and cannot receive images.\nDo you really want to send this image?").arg(contact.ownerBuddy().display());
		}
		else
		{
			if (tooBigCounter > 0)
				message = tr("This file is too big for %1 of %2 contacts.\n").arg(tooBigCounter).arg(CurrentChat.contacts().count());
			if (disconnectedCounter > 0)
				message += tr("%1 of %2 contacts are disconnected and cannot receive images.\n").arg(disconnectedCounter).arg(CurrentChat.contacts().count());
			if (tooBigCounter > 0 || disconnectedCounter > 0)
				message += tr("Do you really want to send this image?\nSome of them probably will not get it.");
		}

		if (!message.isEmpty() && !MessageDialog::ask(QString(), tr("Kadu"), message))
			return;

		QString path = ChatImageService::imagesPath();
		if (QFileInfo(path).isDir() ||QDir().mkdir(path))
		{
			QString copyFilePath = path + QUuid::createUuid().toString();
			if (QFile::copy(selectedFile, copyFilePath))
				selectedFile = copyFilePath;
		}

		InputBox->insertPlainText(QString("[IMAGE %1]").arg(selectedFile));
	}
}

void ChatEditBox::addEmoticon(const QString &emot)
{
	if (!emot.isEmpty())
	{
		QString escaped = emot;
		escaped = escaped.replace("&lt;", "<");
		escaped = escaped.replace("&gt;", ">");
		InputBox->insertPlainText(escaped);
	}
}

void ChatEditBox::changeColor(const QColor &newColor)
{
	CurrentColor = newColor;

	QPixmap p(12, 12);
	p.fill(CurrentColor);

// 	Action *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);
// 	if (action)
// 		action->setIcon(p);

	InputBox->setTextColor(CurrentColor);
}

void ChatEditBox::setColorFromCurrentText(bool force)
{
// 	Action *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);

	Action *action = 0;
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

	action->setIcon(p);
}
