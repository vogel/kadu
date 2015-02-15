/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PLUGIN_ERROR_DIALOG_H
#define PLUGIN_ERROR_DIALOG_H

#include <QtWidgets/QDialog>

#include "os/generic/desktop-aware-object.h"

class QCheckBox;

class PluginErrorDialog : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	QString m_pluginName;
	QCheckBox *LoadInFutureCheck;

public:
	PluginErrorDialog(QString pluginName, const QString &text, bool offerLoadInFutureChoice, QWidget *parent = 0);
	virtual ~PluginErrorDialog();

public slots:
	void accept();

signals:
	void accepted(const QString &pluginName, bool loadInFuture);

};

#endif // PLUGIN_ERROR_DIALOG_H
