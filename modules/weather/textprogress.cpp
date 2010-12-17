/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>

#include "textprogress.h"

const int TextProgress::dotCount_ = 5;

TextProgress::TextProgress(QWidget *parent, const QString &text) :
	QLabel(parent),
	counter_(0)
{
	timer_ = new QTimer(this);
	connect(timer_, SIGNAL(timeout()), this, SLOT(timeout()));

	setText(text);

	timer_->start(250);
}

void TextProgress::setText(const QString &text)
{
	counter_ = 0;
	text_ = text;
	setProgressText();
}

void TextProgress::setProgressText()
{
	if (!text_.isEmpty())
	{
		QString text = ' ' + text_ + ' ';
		for (int i = 0; i < counter_; ++i)
			text = '.' + text + '.';

		QLabel::setText(text);
	}
}

void TextProgress::clearProgressText()
{
	text_.clear();
	clear();
}

void TextProgress::timeout()
{
	if (counter_ >= dotCount_)
		counter_ = 0;
	else
		++counter_;

	setProgressText();
}
