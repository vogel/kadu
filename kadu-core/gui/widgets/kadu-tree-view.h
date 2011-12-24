/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef KADU_TREE_VIEW_H
#define KADU_TREE_VIEW_H

#include <QtGui/QTreeView>

#include "configuration/configuration-aware-object.h"

class QTemporaryFile;

class KaduTreeView : public QTreeView, ConfigurationAwareObject
{
	Q_OBJECT

public:
	enum BackgroundMode
	{
		BackgroundNone,
		BackgroundCentered,
		BackgroundTiled,
		BackgroundTiledAndCentered,
		BackgroundStretched
	};

private:
	QString BackgroundColor;
	QString AlternateBackgroundColor;
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;
	QTemporaryFile *BackgroundTemporaryFile;

protected:
	virtual void configurationUpdated();

	virtual void resizeEvent(QResizeEvent *event);
	virtual void startDrag(Qt::DropActions supportedActions);

public:
	explicit KaduTreeView(QWidget *parent = 0);
	virtual ~KaduTreeView();

	void setBackground(const QString& backgroundColor, const QString& alternateColor,
	                   const QString& file = QString(), KaduTreeView::BackgroundMode mode = BackgroundNone);
	void updateBackground();

};

#endif // KADU_TREE_VIEW_H
