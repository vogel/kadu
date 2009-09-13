/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "delayed-line-edit.h"


DelayedLineEdit::DelayedLineEdit(QWidget *parent) :
		QLineEdit(parent)
{
	Timer.setSingleShot(true);
	Timer.setInterval(1000);
	connect(&Timer, SIGNAL(timeout()), this, SLOT(timeout()));

	connect(this, SIGNAL(textChanged(const QString &)),
			this, SIGNAL(delayedTextChanged(const QString &)));
}

DelayedLineEdit::~DelayedLineEdit()
{
}

void DelayedLineEdit::setDelay(unsigned int delay)
{
	Timer.setInterval(delay);
}

void DelayedLineEdit::textChangedSlot(const QString &text)
{
	if (Timer.isActive())
		Timer.stop();

	Timer.start();
}

void DelayedLineEdit::timeout()
{
	emit delayedTextChanged(text());
}
