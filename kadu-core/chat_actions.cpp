/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_widget.h"

#include "chat_actions.h"

AutoSendActionDescription::AutoSendActionDescription()
	: ActionDescription(ActionDescription::TypeChat, "autoSendAction", "", tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")), true)
{
}

AutoSendActionDescription::~AutoSendActionDescription()
{
}

void AutoSendActionDescription::triggered(QWidget *widget, bool checked)
{
}


void AutoSendActionDescription::toggled(QWidget *widget, bool checked)
{
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(widget);
	if (chatWidget)
	{
		chatWidget->setAutoSend(checked);
		// setAllChecked(checked)
		config_file.writeEntry("Chat", "AutoSend", checked);
	}
}

ClearChatActionDescription::ClearChatActionDescription()
	: ActionDescription(ActionDescription::TypeChat, "clearChatAction", "", tr("Clear messages in chat window"))
{
}

ClearChatActionDescription::~ClearChatActionDescription()
{
}

void ClearChatActionDescription::triggered(QWidget *widget, bool checked)
{
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(widget);
	if (chatWidget)
		chatWidget->clearChatWindow();
}


void ClearChatActionDescription::toggled(QWidget *widget, bool checked)
{
}
