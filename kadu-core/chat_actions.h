/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_ACTIONS_H
#define CHAT_ACTIONS_H

#include "action.h"

class AutoSendActionDescription : ActionDescription
{

protected:
	virtual void triggered(QWidget *widget, bool checked);
	virtual void toggled(QWidget *widget, bool checked);

public:
	AutoSendActionDescription();
	virtual ~AutoSendActionDescription();

};

class ClearChatActionDescription : ActionDescription
{

protected:
	virtual void triggered(QWidget *widget, bool checked);
	virtual void toggled(QWidget *widget, bool checked);

public:
	ClearChatActionDescription();
	virtual ~ClearChatActionDescription();

};

#endif // CHAT_ACTIONS_H
