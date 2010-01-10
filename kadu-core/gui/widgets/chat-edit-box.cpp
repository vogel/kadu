/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtXml/QDomElement>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/image-dialog.h"

#include "chat-widget.h"
#include "custom-input.h"
#include "debug.h"
#include "emoticons.h"
#include "toolbar.h"

#include "chat-edit-box.h"

QList<ChatEditBox *> chatEditBoxes;

ChatEditBox::ChatEditBox(Chat chat, QWidget *parent) :
		MainWindow(parent), CurrentChat(chat)
{
	chatEditBoxes.append(this);

	InputBox = new CustomInput(this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);
	InputBox->setAcceptRichText(true);

	setCentralWidget(InputBox);

	bool old_top = loadToolBarsFromConfig("chatTopDockArea", Qt::TopToolBarArea, true);
	bool old_middle = loadToolBarsFromConfig("chatMiddleDockArea", Qt::TopToolBarArea, true);
	bool old_bottom = loadToolBarsFromConfig("chatBottomDockArea", Qt::BottomToolBarArea, true);
	bool old_left = loadToolBarsFromConfig("chatLeftDockArea", Qt::LeftToolBarArea, true);
	bool old_right = loadToolBarsFromConfig("chatRightDockArea", Qt::RightToolBarArea, true);

	if (old_top || old_middle || old_bottom || old_left || old_right)
		writeToolBarsToConfig("chat"); // port old config
	else
		loadToolBarsFromConfig("chat"); // load new config

	connect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(Action *)),
			this, SLOT(colorSelectorActionCreated(Action *)));
	connect(InputBox, SIGNAL(keyPressed(QKeyEvent *,CustomInput *, bool &)),
			this, SIGNAL(keyPressed(QKeyEvent *,CustomInput *,bool &)));
	connect(InputBox, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
	connect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	configurationUpdated();
}

ChatEditBox::~ChatEditBox()
{
	disconnect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(Action *)),
			this, SLOT(colorSelectorActionCreated(Action *)));
	disconnect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	chatEditBoxes.removeAll(this);

	writeToolBarsToConfig("chat");
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

BuddiesListView * ChatEditBox::contactsListView()
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
	ChatWidget *result = dynamic_cast<ChatWidget *>(parent());
	if (result)
		return result;

	result = dynamic_cast<ChatWidget *>(parent()->parent());
	if (result)
		return result;

	return 0;
}

void ChatEditBox::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "chat_topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");
	toolbarConfig.setAttribute("align", "right");

	addToolButton(toolbarConfig, "autoSendAction");
	addToolButton(toolbarConfig, "clearChatAction");
	addToolButton(toolbarConfig, "insertEmoticonAction");
	addToolButton(toolbarConfig, "whoisAction");
	addToolButton(toolbarConfig, "insertImageAction");
	addToolButton(toolbarConfig, "editUserAction");

	dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "chat_bottomDockArea");
	toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");
	toolbarConfig.setAttribute("x_offset", 0);

	addToolButton(toolbarConfig, "boldAction");
	addToolButton(toolbarConfig, "italicAction");
	addToolButton(toolbarConfig, "underlineAction");
	addToolButton(toolbarConfig, "colorAction");

	toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");
	toolbarConfig.setAttribute("x_offset", 200);
	toolbarConfig.setAttribute("align", "right");

	addToolButton(toolbarConfig, "sendAction", Qt::ToolButtonTextBesideIcon);
}

void ChatEditBox::addAction(const QString &actionName, Qt::ToolButtonStyle style)
{
	addToolButton(findExistingToolbar("chat"), actionName, style);

	foreach (ChatEditBox *chatEditBox, chatEditBoxes)
		chatEditBox->refreshToolBars("chat");
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
	ImageDialog *id = new ImageDialog(this);
	id->setDirectory(config_file.readEntry("Chat", "LastImagePath"));
	id->setWindowTitle(tr("Insert image"));

	while (id->exec() == QDialog::Accepted && 0 < id->selectedFiles().count())
	{
		config_file.writeEntry("Chat", "LastImagePath", id->directory().absolutePath());

		QString selectedFile = id->selectedFiles()[0];
		QFileInfo f(selectedFile);

		if (!f.isReadable())
		{
			MessageDialog::msg(tr("This file is not readable"), true, "Warning", this);
			continue;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageDialog::msg(tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), true, "Warning", this);
			continue;
		}

		int counter = 0;

		foreach (const Contact &contact, CurrentChat.contacts())
		{
			// TODO: 0.6.6
// 			if (contact && contact->hasFeature(/*EmbedImageInChatMessage*/))
// 			{
// 				unsigned long maxImageSize = contact->maxEmbededImageSize();
// 				if (f.size() > maxImageSize)
// 					counter++;
// 			}
// 			else
				counter++;
			// unsigned int maximagesize = user.protocolData("Gadu", "MaxImageSize").toUInt();
		}
		if (counter == 1 && CurrentChat.contacts().count() == 1)
		{
			if (!MessageDialog::ask(tr("This file is too big for %1.\nDo you really want to send this image?\n").arg((*CurrentChat.contacts().begin()).ownerBuddy().display())))
				continue;
		}
		else if (counter > 0 &&
			!MessageDialog::ask(tr("This file is too big for %1 of %2 contacts.\nDo you really want to send this image?\nSome of them probably will not get it.").arg(counter).arg(CurrentChat.contacts().count())))
			continue;

		InputBox->insertPlainText(QString("[IMAGE %1]").arg(selectedFile));
		break;
	}

	id = 0;
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

	Action *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);
	if (action)
		action->setIcon(p);

	InputBox->setTextColor(CurrentColor);
}

void ChatEditBox::setColorFromCurrentText(bool force)
{
	Action *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);

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
