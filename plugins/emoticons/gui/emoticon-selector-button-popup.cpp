/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>

#include "expander/emoticon-path-provider.h"
#include "emoticon.h"

#include "emoticon-selector-button-popup.h"

EmoticonSelectorButtonPopup::EmoticonSelectorButtonPopup(const Emoticon &emoticon, EmoticonPathProvider *pathProvider, QWidget *parent) :
		QLabel(parent, Qt::Popup), DisplayEmoticon(emoticon)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(parent->sizeHint());
	setAlignment(Qt::AlignCenter);
	setMouseTracking(true);
	setToolTip(emoticon.triggerText());

	QString style =
		"QLabel {"
		"	background-color: palette(highlight);"
		"	padding: 4px;"
		"}";
	setStyleSheet(style);

	QMovie *movie = new QMovie(this);
	movie->setFileName(pathProvider->emoticonPath(emoticon));
	setMovie(movie);
	movie->start();

	// center on parent
	QPoint newPos = parent->mapToGlobal(QPoint(0, 0));
	newPos += QPoint(parent->sizeHint().width() / 2, parent->sizeHint().height() / 2);
	newPos -= QPoint(sizeHint().width() / 2, sizeHint().height() / 2);
	move(newPos);
}

EmoticonSelectorButtonPopup::~EmoticonSelectorButtonPopup()
{
}

void EmoticonSelectorButtonPopup::mouseMoveEvent(QMouseEvent *e)
{
	QLabel::mouseMoveEvent(e);
	if (!rect().contains(e->globalPos() - mapToGlobal(QPoint(0, 0))))
		close();
}

void EmoticonSelectorButtonPopup::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);
	emit clicked(DisplayEmoticon);
}

#include "moc_emoticon-selector-button-popup.cpp"
