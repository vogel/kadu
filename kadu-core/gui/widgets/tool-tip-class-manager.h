/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/widgets/abstract-tool-tip.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <injeqt/injeqt.h>

class Configuration;

class KADUAPI ToolTipClassManager : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ToolTipClassManager(QObject *parent = nullptr);
	virtual ~ToolTipClassManager();

	void registerToolTipClass(const QString &toolTipClassName, AbstractToolTip *toolTipClass);
	void unregisterToolTipClass(const QString &toolTipClassName);

	QStringList getToolTipClasses();
	void useToolTipClass(const QString &toolTipClassName);

	bool showToolTip(const QPoint &point, Talkable talkable);
	bool hideToolTip();

	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;

	QMap<QString, AbstractToolTip *> ToolTipClasses;
	QString ToolTipClassName;
	AbstractToolTip *CurrentToolTipClass;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

};
