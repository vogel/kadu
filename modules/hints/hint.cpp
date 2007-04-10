/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>

#include "config_file.h"
#include "debug.h"
#include "hint.h"
#include "icons_manager.h"
#include "kadu_parser.h"
#include "misc.h"

#include "../notify/notification.h"

/**
 * @ingroup hints
 * @{
 */
Hint::Hint(QWidget *parent, const QString& text, const QPixmap& pixmap, unsigned int timeout) :
	QWidget(parent, "Hint"), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), secs(timeout), users(), notification(0),
	haveCallbacks(false)
{
	kdebugf();
	if (timeout==0)
		kdebugm(KDEBUG_INFO|KDEBUG_ERROR, "Hint error: timeout==0! text: %s\n", text.local8Bit().data());

	createLabels(text, pixmap);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	hide();

	kdebugf2();
}

Hint::Hint(QWidget *parent, Notification *notification)
	: QWidget(parent, "Hint"), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), users(), notification(notification),
	  haveCallbacks(notification->getCallbacks().count() != 0)
{
	kdebugf();

	secs = config_file.readNumEntry("Hints", "Event_" + notification->type() + "_timeout", 2);

	QString text;

	QString syntax = config_file.readEntry("Hints", "Event_" + notification->type() + "_syntax", "");
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

	createLabels(text, icons_manager->loadIcon(notification->icon()));

	callbacksBox = new QHBoxLayout();
	vbox->addLayout(callbacksBox);

	const QValueList<QPair<QString, const char *> > callbacks = notification->getCallbacks();
	if (notification->getCallbacks().count())
	{
		callbacksBox->addStretch(10);

		FOREACH(i, callbacks)
		{
			QPushButton *button = new QPushButton((*i).first, this);
			connect(button, SIGNAL(clicked()), notification, (*i).second);
			connect(button, SIGNAL(clicked()), notification, SLOT(clearDefaultCallback()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}

	connect(notification, SIGNAL(closed()), this, SLOT(notificationClosed()));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	hide();

	kdebugf2();
}

void Hint::createLabels(const QString &text, const QPixmap &pixmap)
{
	vbox = new QVBoxLayout(this);
	vbox->setSpacing(2);
	vbox->setMargin(1);
	vbox->setResizeMode(QLayout::FreeResize);

	labels = new QHBoxLayout();
	vbox->addLayout(labels);

	if (!pixmap.isNull())
	{
		icon = new QLabel(this, "Icon");
		icon->setPixmap(pixmap);
		icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
		labels->addWidget(icon);
	}

	label = new QLabel(this, "Label");
	label->setTextFormat(Qt::RichText);
	label->setText(" " + QString(text).replace(" ", "&nbsp;").replace("\n", "<br />"));
	label->setAlignment(AlignVCenter | Qt::AlignLeft);
	labels->addWidget(label);
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
		if (secs==0)
			kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
		else if (secs>2000000000)
			kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");

		--secs;
	}
}

bool Hint::isDeprecated()
{
	return (!haveCallbacks) && secs == 0;
}

void Hint::set(const QFont &font, const QColor &color, const QColor &bgcolor)
{
 	kdebugf();

	label->setFont(font);

	setPaletteForegroundColor(color);
	setPaletteBackgroundColor(bgcolor);

	bcolor = bgcolor;

	show();

	kdebugf2();
}

void Hint::mousePressEvent(QMouseEvent * event)
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
	setPaletteBackgroundColor(bcolor.light());
}

void Hint::leaveEvent(QEvent *)
{
	setPaletteBackgroundColor(bcolor);
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
	fgcolor = label->paletteForegroundColor();
	bgcolor = bcolor;
}

void Hint::acceptNotification()
{
	if (notification)
		notification->callbackAccept();
}

void Hint::discardNotification()
{
	if (notification)
		notification->callbackDiscard();
}

/** @} */

