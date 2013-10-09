/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef EMOTICON_SELECTOR_H
#define EMOTICON_SELECTOR_H

#include <QtGui/QScrollArea>

#include "expander/emoticon-path-provider.h"

#include "emoticon.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonSelector
 * @short Widget displaying grid of EmoticonSelectorButton for provided list of emoticons.
 */
class EmoticonSelector : public QScrollArea
{
	Q_OBJECT

	QScopedPointer<EmoticonPathProvider> PathProvider;

	void addEmoticonButtons(const QVector<Emoticon> &emoticons, QWidget *mainwidget);
	void calculatePositionAndSize(const QWidget *activatingWidget, const QWidget *mainwidget);

private slots:
	void emoticonClickedSlot(const Emoticon &emoticon);

protected:
	virtual bool event(QEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:
	/**
	 * @short Create new EmoticonSelector widget.
	 * @param emoticons emoticons to display
	 * @param pathProvider EmoticonPathProvider used to get image file name for emoticon for popup widget
	 * @param parent parent widget
	 *
	 * This object gets ownership of pathProvider pointer.
	 */
	explicit EmoticonSelector(const QVector<Emoticon> &emoticons, EmoticonPathProvider *pathProvider, QWidget *parent = 0);
	virtual ~EmoticonSelector();

signals:
	/**
	 * @short Signal emited when emoticon is clicked.
	 * @param emoticon clicked emoticon
	 */
	void emoticonClicked(const Emoticon &emoticon);

	/**
	 * @short Signal emited when emoticon is clicked.
	 * @param emoticon clicked emoticon's trigger text
	 */
	void emoticonClicked(const QString &emoticon);

};

/**
 * @}
 */

#endif // EMOTICON_SELECTOR_H
