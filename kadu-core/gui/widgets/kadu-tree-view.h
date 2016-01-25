/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QTreeView>
#include <injeqt/injeqt.h>

class Configuration;
class IconsManager;

class QTemporaryFile;

class KADUAPI KaduTreeView : public QTreeView, ConfigurationAwareObject
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

	explicit KaduTreeView(QWidget *parent = nullptr);
	virtual ~KaduTreeView();

	void setBackground(const QString& backgroundColor, const QString& alternateColor,
	                   const QString& file = QString(), KaduTreeView::BackgroundMode mode = BackgroundNone);
	void updateBackground();

protected:
	virtual void configurationUpdated();

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void startDrag(Qt::DropActions supportedActions);

private:
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;

	QString BackgroundColor;
	QString AlternateBackgroundColor;
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;
	QTemporaryFile *BackgroundTemporaryFile;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

};
