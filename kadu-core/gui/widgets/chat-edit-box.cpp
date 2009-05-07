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
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"

#include "color_selector.h"
#include "config_file.h"
#include "chat-widget.h"
#include "custom_input.h"
#include "emoticons.h"
#include "toolbar.h"
#include "xml_config_file.h"

#include "chat-edit-box.h"

QList<ChatEditBox *> chatEditBoxes;

ChatEditBox::ChatEditBox(QWidget *parent) : KaduMainWindow(parent)
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
	connect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
}

ChatEditBox::~ChatEditBox()
{
	disconnect(ChatWidgetManager::instance()->actions()->colorSelector(), SIGNAL(actionCreated(KaduAction *)),
			this, SLOT(colorSelectorActionCreated(KaduAction *)));
	disconnect(InputBox, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	chatEditBoxes.removeAll(this);

	writeToolBarsToConfig("chat");
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
