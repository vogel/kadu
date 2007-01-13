/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "hint.h"
#include "debug.h"
#include "config_file.h"

/**
 * @ingroup hints
 * @{
 */
Hint::Hint(QWidget *parent, const QString& text, const QPixmap& pixmap, unsigned int timeout) :
	QWidget(parent, "Hint"), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), secs(timeout), users()
{
	kdebugf();
	if (timeout==0)
		kdebugm(KDEBUG_INFO|KDEBUG_ERROR, "Hint error: timeout==0! text: %s\n", text.local8Bit().data());

	createLabels(text, pixmap);

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

void Hint::close()
{
	hintClosing = true;
	deleteLater();
	emit deleting(this);
}

void Hint::nextSecond(void)
{
	if (secs==0)
		kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
	else if (secs>2000000000)
		kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");
	--secs;
}

bool Hint::isDeprecated()
{
	return secs == 0;
}

void Hint::setShown(bool doShow)
{
	kdebugmf(KDEBUG_FUNCTION_START, "show=%d\n", doShow);

	if (doShow)
		show();
	kdebugf2();
}

void Hint::set(const QFont &font, const QColor &color, const QColor &bgcolor, bool doShow)
{
 	kdebugf();

	label->setFont(font);

	setPaletteForegroundColor(color);
	setPaletteBackgroundColor(bgcolor);

	bcolor = bgcolor;

	if (doShow)
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

/** @} */

