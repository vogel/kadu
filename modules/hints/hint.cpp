/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Kermit (plaza.maciej@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "misc/misc.h"
#include "notify/chat-notification.h"
#include "notify/notification.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "hint.h"

/**
 * @ingroup hints
 * @{
 */
Hint::Hint(QWidget *parent, Notification *notification)
	: QWidget(parent), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), notification(notification),
	  requireCallbacks(notification->requireCallback())
{
	kdebugf();

	if (notification->type() == "Preview")
		requireCallbacks = true;

	notification->acquire();

	ChatNotification *chatNotification = dynamic_cast<ChatNotification *>(notification);
	CurrentChat = chatNotification ? chatNotification->chat() : Chat::null;

	if (!notification->details().isEmpty())
		details.append(notification->details());

	startSecs = secs = config_file.readNumEntry("Hints", "Event_" + notification->key() + "_timeout", 10);

	createLabels(notification->icon().pixmap(config_file.readNumEntry("Hints", "AllEvents_iconSize", 32)));
	updateText();

	const QList<Notification::Callback> callbacks = notification->getCallbacks();
	bool showButtons = !callbacks.isEmpty();
	if (showButtons)
		if (config_file.readBoolEntry("Hints", "ShowOnlyNecessaryButtons") && !notification->requireCallback())
			showButtons = false;

	if (showButtons)
	{
		QWidget *callbacksWidget = new QWidget(this);
		callbacksBox = new QHBoxLayout(callbacksWidget);
		callbacksBox->addStretch(10);
		vbox->addWidget(callbacksWidget);

		foreach (const Notification::Callback &i, callbacks)
		{
			QPushButton *button = new QPushButton(i.Caption, this);
			connect(button, SIGNAL(clicked(bool)), notification, i.Slot);
			connect(button, SIGNAL(clicked(bool)), notification, SLOT(clearDefaultCallback()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed()));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	configurationUpdated();
	show();

	kdebugf2();
}

Hint::~Hint()
{
	kdebugf();

	disconnect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed()));
	notification->release();

	kdebugf2();
}

void Hint::configurationUpdated()
{
	bcolor = config_file.readColorEntry("Hints", "Event_" + notification->key() + "_bgcolor", &qApp->palette().background().color());
	fcolor = config_file.readColorEntry("Hints", "Event_" + notification->key() + "_fgcolor", &qApp->palette().foreground().color());
	label->setFont(config_file.readFontEntry("Hints", "Event_" + notification->key() + "_font", &qApp->font()));
	QString style = narg("QWidget {color:%1; background-color:%2; border-width:0px; border-color:%2}", fcolor.name(), bcolor.name());

	setStyleSheet(style);

	setMinimumWidth(config_file.readNumEntry("Hints", "MinimumWidth", 100));
	setMaximumWidth(config_file.readNumEntry("Hints", "MaximumWidth", 500));
}

void Hint::createLabels(const QPixmap &pixmap)
{
	vbox = new QVBoxLayout(this);
	vbox->setSpacing(2);
	vbox->setMargin(1);
	vbox->setSizeConstraint(QLayout::SetMinimumSize);
	QWidget *widget = new QWidget(this);
	labels = new QHBoxLayout(widget);
	labels->setContentsMargins(5, 5, 5, 5);

	vbox->addWidget(widget);
	if (!pixmap.isNull())
	{
		icon = new QLabel(this);
		icon->setPixmap(pixmap);
		icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		labels->addWidget(icon, 0, Qt::AlignTop);
	}

	label = new QLabel(this);
	label->setTextInteractionFlags(Qt::NoTextInteraction);
	label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	label->setContentsMargins(10, 0, 0, 0);
	labels->addWidget(label);
}

void Hint::updateText()
{
	QString text;

	QString syntax = config_file.readEntry("Hints", "Event_" + notification->key() + "_syntax", "");
	if (syntax.isEmpty())
		text = notification->text();
	else
	{
		kdebug("syntax is: %s, text is: %s\n", syntax.toAscii().data(), notification->text().toAscii().data());

		if (CurrentChat)
		{
			Contact contact = *CurrentChat.contacts().begin();
			text = Parser::parse(syntax, BuddyOrContact(contact), notification);
		}
		else
			text = Parser::parse(syntax, notification);

		/* Dorr: the file:// in img tag doesn't generate the image on hint.
		 * for compatibility with other syntaxes we're allowing to put the file://
		 * so we have to remove it here */
		text = text.replace("file://", "");
	}

	if (config_file.readBoolEntry("Hints", "ShowContentMessage"))
	{
		int count = details.count();

		if (count)
		{
			int i = (count > 5) ? count - 5 : 0;

			int citeSign = config_file.readNumEntry("Hints","CiteSign");

			QString itemSyntax = config_file.readEntry("Hints", "Event_" + notification->key() + "_detailSyntax", "\n&bull; <small>%1</small>");
			for (; i < count; i++)
			{
				const QString &message = details[i];
				if (message.length() > citeSign)
					text += itemSyntax.arg(details[i].left(citeSign) + "...");
				else
					text += itemSyntax.arg(details[i]);
			}
		}
	}

	// this does not work
	//label->setText(' ' + text.replace(' ', QLatin1String("&nbsp;")).replace('\n', QLatin1String("<br />")));
	label->setText(' ' + text.replace('\n', QLatin1String("<br />")));

	emit updated(this);
}

void Hint::resetTimeout()
{
	secs = startSecs;
}

void Hint::notificationClosed()
{
	emit closing(this);
}

bool Hint::requireManualClosing()
{
	return requireCallbacks;
}

void Hint::nextSecond(void)
{
	if (!requireCallbacks)
	{
		if (startSecs == 0)
			return;

		if (secs == 0)
		{
			kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
		}
		else if (secs > 2000000000)
		{
			kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");
		}

		if (secs > 0)
			--secs;
	}
}

bool Hint::isDeprecated()
{
	return (!requireCallbacks) && startSecs != 0 && secs == 0;
}

void Hint::addDetail(const QString &detail)
{
	details.append(detail);
	if (details.count() > 5)
		details.pop_front();

	resetTimeout();
	updateText();
}

void Hint::mouseReleaseEvent(QMouseEvent *event)
{
	switch (event->button())
	{
		case Qt::LeftButton:
			emit leftButtonClicked(this);
			break;

		case Qt::RightButton:
			emit rightButtonClicked(this);
			break;

		case Qt::MidButton:
			emit midButtonClicked(this);
			break;

		default:
			break;
	}
}

void Hint::enterEvent(QEvent *)
{
	QString style = narg("QWidget {color:%1; background-color:%2; border-width:0px; border-color:%2}", fcolor.name(), bcolor.lighter().name());
	setStyleSheet(style);
}

void Hint::leaveEvent(QEvent *)
{
	QString style = narg("QWidget {color:%1; background-color:%2; border-width:0px; border-color:%2}", fcolor.name(), bcolor.name());
	setStyleSheet(style);
}

void Hint::getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor)
{
	text = label->text().remove(' ');

	if (icon)
		pixmap = *(icon->pixmap());
	else
		pixmap = QPixmap();

	timeout = secs;
	font = label->font();
	fgcolor = fcolor;
	bgcolor = bcolor;
}

void Hint::acceptNotification()
{
	notification->callbackAccept();
}

void Hint::discardNotification()
{
	notification->callbackDiscard();
}

/** @} */

