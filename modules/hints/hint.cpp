/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>

#include "hint.h"
#include "debug.h"
#include "config_file.h"

Hint::Hint(QWidget *parent, const QString& text, const QPixmap& pixmap, unsigned int timeout) : QHBoxLayout(0, "Hint")
{
	kdebugf();
	if (timeout==0)
		kdebugm(KDEBUG_INFO|KDEBUG_ERROR, "Hint error: timeout==0! text: %s\n", text.local8Bit().data());

	secs = timeout;
	ident = 0;

	setResizeMode(QLayout::Fixed);

	if (!pixmap.isNull() && config_file.readBoolEntry("Hints","Icons"))
	{
		icon = new QLabel(parent, "Icon");
		icon->setPixmap(pixmap);
		icon->hide();
		icon->installEventFilter(this);
		addWidget(icon);
	}
	else
		icon = NULL;

	label = new QLabel(" "+QString(text).replace(QRegExp(" "), "&nbsp;"), parent, "Label");
	label->setTextFormat(Qt::RichText);
	label->setAlignment(AlignVCenter | Qt::AlignLeft);
	label->hide();
	label->installEventFilter(this);
	addWidget(label, 1);
	kdebugf2();
}

bool Hint::nextSecond(void)
{
	if (secs==0)
		kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
	else if (secs>2000000000)
		kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");
	--secs;
	return secs>0;
}

void Hint::setShown(bool show)
{
	kdebugmf(KDEBUG_FUNCTION_START, "show=%d\n", show);

	if (show)
	{
		if (icon != NULL)
			icon->show();
		label->show();
	}
	kdebugf2();
}

void Hint::set(const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int id, bool show)
{
	kdebugf();

	ident = id;
	label->setFont(font);
	if (icon != NULL)
	{
		icon->setPaletteBackgroundColor(bgcolor);
		if (show)
			icon->show();
	}
	label->setPaletteForegroundColor(color);
	bcolor = bgcolor;
	label->setPaletteBackgroundColor(bcolor);
	if (show)
		label->show();
	kdebugf2();
}

bool Hint::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == label || (icon != NULL && obj == icon))
	{
		switch (ev->type())
		{
			case QEvent::Enter:
				enter();
				return true;
				break;
			case QEvent::Leave:
				leave();
				return true;
				break;
			case QEvent::MouseButtonPress:
			{
				switch ((dynamic_cast<QMouseEvent*>(ev))->button())
				{
					case Qt::LeftButton:
						emit leftButtonClicked(ident);
						return true;
						break;
					case Qt::RightButton:
						emit rightButtonClicked(ident);
						return true;
						break;
					case Qt::MidButton:
						emit midButtonClicked(ident);
						return true;
						break;
					default:
						return false;
						break;
				}
			}
			default:
				return false;
				break;
		}
	}
	else
		return QHBoxLayout::eventFilter(obj, ev);
}

void Hint::enter(void)
{
	if (icon != NULL)
		icon->setPaletteBackgroundColor(bcolor.light());
	label->setPaletteBackgroundColor(bcolor.light());
}

void Hint::leave(void)
{
	if (icon != NULL)
		icon->setPaletteBackgroundColor(bcolor);
	label->setPaletteBackgroundColor(bcolor);
}

Hint::~Hint(void)
{
	kdebugmf(KDEBUG_FUNCTION_START, "id=%d\n", ident);

	if (icon != NULL)
	{
#if QT_VERSION < 0x030100
		icon->hide();
#endif
		icon->deleteLater();
	}
#if QT_VERSION < 0x030100
	label->hide();
#endif
	label->deleteLater();
	kdebugf2();
}

void Hint::getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor)
{
	text=label->text().replace(QRegExp(" "), "");
	if (icon)
		pixmap=*(icon->pixmap());
	else
		pixmap=QPixmap();
	timeout=secs;
	font=label->font();
	fgcolor=label->paletteForegroundColor();
	bgcolor=bcolor;
}

