/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef CONFIG_SECTION_H
#define CONFIG_SECTION_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtGui/QTabWidget>

#include "gui/widgets/configuration/configuration-widget.h"

class ConfigGroupBox;
class ConfigTab;
class ConfigurationWidget;
class ConfigWidget;

class QGridLayout;
class QGroupBox;
class QListWidgetItem;
//class QTabWidget;

class ConfigSection : public QObject
{
	Q_OBJECT

	QString Name;
	ConfigurationWidget *MyConfigurationWidget;
	QString IconPath;

	QListWidgetItem *ListWidgetItem;
	bool Activated;
	QMap<QString, ConfigTab *> ConfigTabs;

	QWidget *ParentConfigGroupBoxWidget;
	QWidget *MainWidget;
	QTabWidget *TabWidget;

	ConfigTab * configTab(const QString &name, bool create = true);

	void switchTabView(bool tabView);

private slots:
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &iconPath);
	~ConfigSection();

	void activate();

	void show() { MainWidget->show(); }
	void hide() { MainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

	void removedConfigTab(const QString &configTabName);

};


#endif
