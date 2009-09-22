/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "contacts/contact-kadu-data.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-box.h"

#include "activate.h"
#include "debug.h"
#include "misc/misc.h"

#include "chat-window.h"

ChatWindow::ChatWindow(QWidget *parent)
	: QWidget(parent), currentChatWidget(0), title_timer(new QTimer(this))
{
	kdebugf();

	connect(title_timer, SIGNAL(timeout()), this, SLOT(blinkTitle()));
	connect(this, SIGNAL(chatWidgetActivated(ChatWidget *)),
			ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)));

#ifdef Q_OS_MAC
	setAttribute(Qt::WA_MacBrushedMetal);
#endif
	setAttribute(Qt::WA_DeleteOnClose);

	configurationUpdated();
}

ChatWindow::~ChatWindow()
{
	kaduStoreGeometry();
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
	QVBoxLayout *layout = new QVBoxLayout(this);

	currentChatWidget = newChatWidget;
	newChatWidget->setParent(this);
	newChatWidget->show();

	layout->addWidget(newChatWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	connect(currentChatWidget, SIGNAL(closed()), this, SLOT(close()));
	connect(currentChatWidget->chat(), SIGNAL(titleChanged(Chat *, const QString &)), this, SLOT(updateTitle()));
	connect(currentChatWidget, SIGNAL(messageReceived(Chat *)), this, SLOT(alertNewMessage()));

	setFocusProxy(currentChatWidget);

	kaduRestoreGeometry();
	updateTitle();
}

ChatWidget * ChatWindow::chatWidget()
{
	return currentChatWidget;
}

// TODO: zrobi� od pocz�tku, strukturalnie spieprzone
void ChatWindow::kaduRestoreGeometry()
{
	// TODO: 0.6.6 apply this data to CurrentChat, it has ModuleData structures

// 	ContactSet contacts = currentChatWidget->contacts();
// 
// 	if (0 == contacts.count())
// 		return;
// 
// 	QRect geom = stringToRect(chat_manager->chatWidgetProperty(currentChatWidget->contacts(), "Geometry").toString());
// 
// 	if (contacts.count() == 1)
// 	{
// 		Contact contact = *contacts.begin();
// 		ContactKaduData *ckd = contact.moduleData<ContactKaduData>();
// 		if (ckd)
// 		{
// 			geom = ckd->chatGeometry();
// 			delete ckd;
// 		}
// 	}

// 	if (geom.isEmpty() && contacts.count() == 1)
// 		geom = stringToRect(ules[0].data("ChatGeometry").toString());
/*
	if (geom.isEmpty())
	{
		QSize size(0, 400);
		int x, y;
		x = pos().x();
		y = pos().y();
		if (contacts.count() > 1)
			size.setWidth(550);
		else
			size.setWidth(400);

		QDesktopWidget *desk = qApp->desktop();

		if ((size.width() + x) > desk->width())
			x = desk->width() - size.width() - 50;
		if ((size.height() + y) > desk->height())
			y = desk->height() - size.height() - 50;

		if (x < 50) x = 50;
		if (y < 50) y = 50;

		move(x, y);
		resize(size);
	}
	else
	{
		setGeometry(geom);
		currentChatWidget->setGeometry(geom);

		currentChatWidget->kaduRestoreGeometry();
	}*/
}

void ChatWindow::kaduStoreGeometry()
{
	// TODO: 0.6.6 as above
/*
	currentChatWidget->kaduStoreGeometry();

	ContactSet contacts = currentChatWidget->contacts();

	chat_manager->setChatWidgetProperty(currentChatWidget->contacts(), "Geometry", rectToString(geometry()));

	if (contacts.count() == 1)
	{
		Contact contact = *contacts.begin();
		ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
		if (ckd)
		{
			ckd->setChatGeometry(geometry());
			ckd->storeConfiguration();
			delete ckd;
		}
	}*/
}

void ChatWindow::closeEvent(QCloseEvent *e)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "ChatCloseTimer"))
	{
		unsigned int period = config_file.readUnsignedNumEntry("Chat",
			"ChatCloseTimerPeriod", 2);

		if (QDateTime::currentDateTime() < currentChatWidget->lastMessageTime().addSecs(period))
		{
			if (!MessageBox::ask(tr("New message received, close window anyway?")))
			{
				e->ignore();
				return;
			}
		}
	}

 	QWidget::closeEvent(e);
}

void ChatWindow::updateTitle()
{
	setWindowIcon(currentChatWidget->chat()->icon());
	setWindowTitle(currentChatWidget->chat()->title());

	if (showNewMessagesNum && currentChatWidget->newMessagesCount()) // if we don't have new messages or don't want them to be shown
		showNewMessagesNumInTitle();
}

void ChatWindow::blinkTitle()
{
 	if (!_isActiveWindow(this))
  	{
		if (!windowTitle().contains(currentChatWidget->chat()->title()) || !blinkChatTitle)
		{
  			if (!showNewMessagesNum) // if we don't show number od new messages waiting
  				setWindowTitle(currentChatWidget->chat()->title());
  			else
				showNewMessagesNumInTitle();
		}
		else
			setWindowTitle(QString().fill(' ', (currentChatWidget->chat()->title().length() + 5)));

		if (blinkChatTitle) // timer will not be started, if configuration option was changed
		{
			title_timer->setSingleShot(true);
			title_timer->start(500);
		}
	}
}

void ChatWindow::showNewMessagesNumInTitle()
{
	if (!_isActiveWindow(this))
		setWindowTitle("[" + QString().setNum(currentChatWidget->newMessagesCount()) + "] " + currentChatWidget->chat()->title());
}

void ChatWindow::windowActivationChange(bool b)
{
	kdebugf();
	if (_isActiveWindow(this))
	{
		currentChatWidget->markAllMessagesRead();
		setWindowTitle(currentChatWidget->chat()->title());

		if (title_timer->isActive())
			title_timer->stop();

		if (!b)
			emit chatWidgetActivated(currentChatWidget);
	}
	kdebugf2();
}

void ChatWindow::alertNewMessage()
{
	if (!_isActiveWindow(this))
	{
		if (activateWithNewMessages && qApp->activeWindow() && !isMinimized())
		{
			_activateWindow(this);
		}
		else if (blinkChatTitle)
		{
			if (!title_timer->isActive())
				blinkTitle(); // blinking is able to show new messages also...
		}
		else if (showNewMessagesNum) // ... so we check this condition as 'else'
			showNewMessagesNumInTitle();

		qApp->alert(this); // TODO: make notifier from this
	}
	else
		currentChatWidget->markAllMessagesRead();
}

void ChatWindow::setWindowTitle(const QString &title)
{
	// qt treats [*] as 'modified placeholder'
	// we escape each [*] with double [*][*] so it gets properly handled
	QString escaped = title;
	QWidget::setWindowTitle(escaped.replace(QLatin1String("[*]"), QLatin1String("[*][*]")));
}

void ChatWindow::closeChatWidget(ChatWidget *chatWidget)
{
	close();
}
