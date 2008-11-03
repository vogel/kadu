/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QPainter>

#include "config_file.h"
#include "debug.h"
#include "osd_hint.h"
#include "icons_manager.h"
#include "kadu_parser.h"
#include "misc.h"

#include "../notify/notification.h"

/**
 * @ingroup hints
 * @{
 */
Hint::Hint(QWidget *parent, Notification *notification)
	: QWidget(parent, "OSDHint"), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), notification(notification),
	  haveCallbacks(notification->getCallbacks().count() != 0)
{
	kdebugf();

	notification->acquire();

	if (notification->details() != "")
		details.append(notification->details());

	if (config_file.readBoolEntry("OSDHints", "SetAll", false))
		startSecs = secs = config_file.readNumEntry("OSDHints", "SetAll_timeout", 10);
	else
		startSecs = secs = config_file.readNumEntry("OSDHints", "Event_" + notification->type() + "_timeout", 10);

	int iconSize = config_file.readNumEntry("OSDHints", "IconSize", 32);
	QString icon = dataPath("kadu/modules/data/osd_hints/") + notification->icon().lower() + QString::number(iconSize) + ".png";

	QPixmap pix = icons_manager->loadPixmap(icon);
    	if (pix.isNull())
		pix = icons_manager->loadPixmap(dataPath("kadu/modules/data/osd_hints/osd_icon.png"));

	createLabels(pix);
	updateText();

	const QList<Notification::Callback> callbacks = notification->getCallbacks();
	if (notification->getCallbacks().count())
	{
		QWidget *callbacksWidget = new QWidget(this);
		callbacksBox = new QHBoxLayout(callbacksWidget);
		callbacksBox->addStretch(10);
		vbox->addWidget(callbacksWidget);

		foreach(const Notification::Callback &i, callbacks)
		{
			QPushButton *button = new QPushButton(i.first, this);
			connect(button, SIGNAL(clicked()), notification, i.second);
			connect(button, SIGNAL(clicked()), notification, SLOT(clearDefaultCallback()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed()));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	double opacity = config_file.readNumEntry("OSDHints", "Opacity", 100);
	opacity /= 100;
	setWindowOpacity(opacity);
	configurationUpdated();

	show();
#if 0
	mask.resize(width(), height());
	mask.fill(Qt::black);
	QPainter maskPainter(&mask);
	maskPainter.setBrush(Qt::white);
	maskPainter.setPen(Qt::white);
	maskPainter.drawRoundRect(0, 0, width(), height(), 1600 / width(), 1600 / height());
	setMask(mask);
#endif
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
	QString configurationDirective;

	if (config_file.readBoolEntry("OSDHints", "SetAll", false))
		configurationDirective = "SetAll";
	else
		configurationDirective = "Event_" + notification->type();

	bcolor = config_file.readColorEntry("OSDHints", configurationDirective + "_bgcolor", &paletteBackgroundColor());
	fcolor = config_file.readColorEntry("OSDHints", configurationDirective + "_fgcolor", &paletteForegroundColor());
	label->setFont(config_file.readFontEntry("OSDHints", configurationDirective + "_font"));
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), bcolor.name());
	setStyleSheet(style);

	setMinimumWidth(config_file.readNumEntry("OSDHints", "MinimumWidth", 100));
	setMaximumWidth(config_file.readNumEntry("OSDHints", "MaximumWidth", 500));
}

void Hint::createLabels(const QPixmap &pixmap)
{
	vbox = new QVBoxLayout(this);
	vbox->setSpacing(2);
	vbox->setMargin(0);
	vbox->setResizeMode(QLayout::FreeResize);

	QWidget *widget = new QWidget(this);
	labels = new QHBoxLayout(widget);
	labels->setSpacing(0);
	labels->setMargin(10);
	vbox->addWidget(widget);

	if (!pixmap.isNull())
	{
		icon = new QLabel(this, "Icon");
		icon->setPixmap(pixmap);
		icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		icon->setBackgroundMode(Qt::NoBackground);
		labels->addWidget(icon, 0, Qt::AlignTop);
	}

	label = new QLabel(this, "Label");
	label->setTextInteractionFlags(Qt::NoTextInteraction);
	label->setTextFormat(Qt::RichText);
	label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	label->setBackgroundMode(Qt::NoBackground);
	labels->addWidget(label);
}

void Hint::updateText()
{
	QString text;

	QString syntax = config_file.readEntry("OSDHints", "Event_" + notification->type() + "_syntax", "");
	if (syntax == "")
		text = notification->text();
	else
	{
		UserListElement ule;
		if (notification->userListElements().count())
			ule = notification->userListElements()[0];

		kdebug("syntax is: %s, text is: %s\n", syntax.ascii(), notification->text().ascii());
		text = KaduParser::parse(syntax, ule, notification);
	}

	if (config_file.readBoolEntry("OSDHints", "ShowContentMessage"))
	{
		int count = details.count();
		if (count)
		{
			int i = (count > 5) ? count - 5 : 0;
			int citeSign = config_file.readNumEntry("OSDHints","CiteSign");
			for (; i < count; i++)
			{
				const QString &message = details[i];
				if (message.length() > citeSign)
					text += "\n" + message.left(citeSign) + "...";
				else
					text += "\n" + message;
			}
		}
	}

	label->setText(" " + text.replace(" ", "&nbsp;").replace("\n", "<br />"));

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
	return haveCallbacks;
}

void Hint::nextSecond(void)
{
	if (!haveCallbacks)
	{
		if (secs == 0)
			kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
		else if (secs>2000000000)
			kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");

		if (secs >= 0)
			--secs;
	}
}

bool Hint::isDeprecated()
{
	return (!haveCallbacks) && secs == 0;
}

void Hint::addDetail(const QString &detail)
{
	details.append(detail);
	if (details.count() > 5)
		details.pop_front();

	resetTimeout();
	updateText();
}

bool Hint::hasUsers() const
{
	return notification->userListElements().count() != 0;
}

const UserListElements & Hint::getUsers() const
{
	return notification->userListElements();
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
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), bcolor.lighter().name());
	setStyleSheet(style);
}

void Hint::leaveEvent(QEvent *)
{
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), bcolor.name());
	setStyleSheet(style);
}

void Hint::getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor)
{
	text = label->text().remove(" ");

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

