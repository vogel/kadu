/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qtimer.h>
#include "chat_window.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "debug.h"
#include "config_file.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "kadu_splitter.h"
#include "kadu_text_browser.h"
#include "message_box.h"
#include "misc.h"
#include "search.h"
#include "syntax_editor.h"
#include "userbox.h"
#include "protocol.h"

ChatWindow::ChatWindow(QWidget* parent, const char* name) : QMainWindow(parent, name, WType_TopLevel | WDestructiveClose),
	currentChatWidget(0), title_timer(new QTimer(this, "title_timer"))
{
	configurationUpdated();
	connect(title_timer, SIGNAL(timeout()), this, SLOT(blinkTitle()));
}

ChatWindow::~ChatWindow()
{
	storeGeometry();
}

void ChatWindow::configurationUpdated()
{
	activateWithNewMessages = config_file.readBoolEntry("Chat", "ActivateWithNewMessages", false);
	showNewMessagesNum = config_file.readBoolEntry("Chat", "NewMessagesInChatTitle", false);
	blinkChatTitle = config_file.readBoolEntry("Chat", "BlinkChatTitle", true);

	if (currentChatWidget && currentChatWidget->newMessagesCount())
		blinkTitle();
}

// TODO: fix it
void ChatWindow::setChatWidget(ChatWidget *newChatWidget)
{
	currentChatWidget = newChatWidget;
	setCentralWidget(currentChatWidget);

	connect(currentChatWidget, SIGNAL(closed()), this, SLOT(close()));
	connect(currentChatWidget, SIGNAL(captionUpdated()), this, SLOT(updateTitle()));
	connect(currentChatWidget, SIGNAL(messageReceived(ChatWidget *)), this, SLOT(alertNewMessage()));

	setFocusProxy(currentChatWidget);
	restoreGeometry();
}

ChatWidget* ChatWindow::chatWidget()
{
	return currentChatWidget;
}

// TODO: zrobiæ od pocz±tku, strukturalnie spieprzone
void ChatWindow::restoreGeometry()
{
	const UserGroup *group = currentChatWidget->users();
	QRect geometry = chat_manager->getChatWidgetProperty(group, "Geometry").toRect();
	if (geometry.isEmpty() && group->count() == 1)
	{
		QString geo_str = (*(group->constBegin())).data("ChatGeometry").toString();
		if (!geo_str.isEmpty())
		{
			bool ok[4];
			QStringList s = QStringList::split(",", geo_str);
			geometry.setX(s[0].toInt(ok));
			geometry.setY(s[1].toInt(ok + 1));
			geometry.setWidth(s[2].toInt(ok + 2));
			geometry.setHeight(s[3].toInt(ok + 3));
			if (int(ok[0]) + ok [1] + ok [2] + ok [3] != 4)
				geometry = QRect();
		}
	}
	if (geometry.isEmpty())
	{
		QPoint pos = QCursor::pos();
		int x,y,width,height;
		QDesktopWidget *desk = qApp->desktop();
		x = pos.x() + 50;
		y = pos.y() + 50;
		height=400;

		if (group->count() > 1)
			width=550;
		else
			width=400;
		if (x + width > desk->width())
			x = desk->width() - width - 50;
		if (y + height>desk->height())
			y = desk->height() - height - 50;
		if (x<50) x = 50;
		if (y<50) y = 50;
		geometry.setX(x);
		geometry.setY(y);
		geometry.setWidth(width);
		geometry.setHeight(height);
	}
	setGeometry(geometry);

	currentChatWidget->setGeometry(geometry);
	currentChatWidget->restoreGeometry();
}

void ChatWindow::storeGeometry()
{
	currentChatWidget->storeGeometry();

	const UserGroup *users = currentChatWidget->users();
	chat_manager->setChatWidgetProperty(users, "Geometry", QRect(pos().x(), pos().y(), size().width(), size().height()));
	if (users->count() == 1)
		(*users->begin()).setData("ChatGeometry", QString("%1,%2,%3,%4").arg(pos().x()).arg(pos().y()).arg(size().width()).arg(size().height()));
}

void ChatWindow::closeEvent(QCloseEvent* e)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "ChatCloseTimer"))
	{
		unsigned int period = config_file.readUnsignedNumEntry("Chat",
			"ChatCloseTimerPeriod", 2);
		if (QDateTime::currentDateTime() < currentChatWidget->getLastMsgTime().addSecs(period))
		{
			if (!MessageBox::ask(tr("New message received, close window anyway?")))
			{
				e->ignore();
				return;
			}
		}
	}
 	QMainWindow::closeEvent(e);
}

void ChatWindow::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	currentChatWidget->edit()->setFocus();
 	QMainWindow::mouseReleaseEvent(e);
}

void ChatWindow::updateTitle()
{
	setIcon(currentChatWidget->icon());
	setCaption(currentChatWidget->caption());

	if (showNewMessagesNum && currentChatWidget->newMessagesCount()) // if we don't have new messages or don't want them to be shown
		showNewMessagesNumInTitle();
}

void ChatWindow::blinkTitle()
{
 	if (!isActiveWindow())
  	{
		if (!caption().contains(currentChatWidget->caption()) || !blinkChatTitle)
		{
  			if (!showNewMessagesNum) // if we don't show number od new messages waiting
  				setCaption(currentChatWidget->caption());
  			else
				showNewMessagesNumInTitle();
		}
		else
			setCaption(QString().fill(' ', (currentChatWidget->caption().length() + 5)));

		if (blinkChatTitle) // timer will not be started, if configuration option was changed
			title_timer->start(500,TRUE);
	}
}

void ChatWindow::showNewMessagesNumInTitle()
{
	if (!isActiveWindow())
		setCaption("[" + QString().setNum(currentChatWidget->newMessagesCount()) + "] " + currentChatWidget->caption());
}

void ChatWindow::windowActivationChange(bool b)
{
	kdebugf();
	if (isActiveWindow())
	{
		currentChatWidget->markAllMessagesRead();
		setCaption(currentChatWidget->caption());

		if (title_timer->isActive())
			title_timer->stop();

		if (!b)
			emit chatWidgetActivated(currentChatWidget);
	}
	kdebugf2();
}

void ChatWindow::alertNewMessage()
{
	if (!isActiveWindow())
	{
		if (activateWithNewMessages && qApp->activeWindow() && !isMinimized())
		{
			currentChatWidget->setActiveWindow();
			raise();
		}
		else if (blinkChatTitle)
		{
			if (!title_timer->isActive())
				blinkTitle(); // blinking is able to show new messages also...
		}
		else if (showNewMessagesNum) // ... so we check this condition as 'else'
			showNewMessagesNumInTitle();
	}
	else
		currentChatWidget->markAllMessagesRead();
}
