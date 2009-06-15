/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomElement>

#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-account-data.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "misc/image-dialog.h"

#include "chat-widget.h"
#include "custom_input.h"
#include "debug.h"
#include "emoticons.h"
#include "message_box.h"
#include "toolbar.h"

#include "chat-edit-box.h"

QList<ChatEditBox *> chatEditBoxes;

ChatEditBox::ChatEditBox(Chat *chat, QWidget *parent) :
		KaduMainWindow(parent), CurrentChat(chat)
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

	connect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(KaduAction *)),
			this, SLOT(colorSelectorActionCreated(KaduAction *)));
	connect(InputBox, SIGNAL(keyPressed(QKeyEvent *,CustomInput *, bool &)),
			this, SIGNAL(keyPressed(QKeyEvent *,CustomInput *,bool &)));
	connect(InputBox, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
	connect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	configurationUpdated();
}

ChatEditBox::~ChatEditBox()
{
	disconnect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(KaduAction *)),
			this, SLOT(colorSelectorActionCreated(KaduAction *)));
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

void ChatEditBox::colorSelectorActionCreated(KaduAction *action)
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

ContactsListWidget * ChatEditBox::contactsListWidget()
{
	ChatWidget *cw = chatWidget();
	if (cw && cw->chat()->contacts().count() > 1)
		return cw->contactsListWidget();

	return 0;
}

ContactSet ChatEditBox::contacts()
{
	ChatWidget *cw = chatWidget();
	if (cw)
		return cw->chat()->contacts();

	return ContactSet();
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

	addToolButton(toolbarConfig, "sendAction", true);
}

void ChatEditBox::addAction(const QString &actionName, bool showLabel)
{
	addToolButton(findExistingToolbar("chat"), actionName, showLabel);

	foreach (ChatEditBox *chatEditBox, chatEditBoxes)
		chatEditBox->refreshToolBars("chat");
}

void ChatEditBox::openEmoticonSelector(const QWidget *activatingWidget)
{
	//emoticons_selector zawsze b�dzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj� na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	EmoticonSelector *emoticonSelector = new EmoticonSelector(this, this);
	emoticonSelector->alignTo(const_cast<QWidget*>(activatingWidget)); //TODO: do something about const_cast
	emoticonSelector->show();
}

void ChatEditBox::openColorSelector(const QWidget *activatingWidget)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	ColorSelector *colorSelector = new ColorSelector(InputBox->palette().foreground().color(), this);
	colorSelector->alignTo(const_cast<QWidget*>(activatingWidget)); //TODO: do something about const_cast
	colorSelector->show();
	connect(colorSelector, SIGNAL(colorSelect(const QColor &)), this, SLOT(changeColor(const QColor &)));
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
			MessageBox::msg(tr("This file is not readable"), true, "Warning", this);
			continue;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageBox::msg(tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), true, "Warning", this);
			continue;
		}

		int counter = 0;

		foreach (Contact contact, CurrentChat->contacts())
		{
			// TODO: 0.6.6
			ContactAccountData *contactAccountData = contact.accountData(CurrentChat->account());
			if (contactAccountData && contactAccountData->hasFeature(/*EmbedImageInChatMessage*/))
			{
// 				unsigned long maxImageSize = contactAccountData->maxEmbededImageSize();
// 				if (f.size() > maxImageSize)
					counter++;
			}
			else
				counter++;
			// unsigned int maximagesize = user.protocolData("Gadu", "MaxImageSize").toUInt();
		}
		if (counter == 1 && CurrentChat->contacts().count() == 1)
		{
			if (!MessageBox::ask(tr("This file is too big for %1.\nDo you really want to send this image?\n").arg((*CurrentChat->contacts().begin()).display())))
				continue;
		}
		else if (counter > 0 &&
			!MessageBox::ask(tr("This file is too big for %1 of %2 contacts.\nDo you really want to send this image?\nSome of them probably will not get it.").arg(counter).arg(CurrentChat->contacts().count())))
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

	KaduAction *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);
	if (action)
		action->setIcon(p);

	InputBox->setTextColor(CurrentColor);
}

void ChatEditBox::setColorFromCurrentText(bool force)
{
	KaduAction *action = ChatWidgetManager::instance()->actions()->colorSelector()->action(this);

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
