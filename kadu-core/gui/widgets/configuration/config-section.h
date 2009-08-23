/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	QString name;
	ConfigurationWidget *configurationWidget;
	QString pixmap;

	QListWidgetItem *listWidgetItem;
	QMap<QString, ConfigTab *> configTabs;

	QWidget *parentConfigGroupBoxWidget;
	QWidget *mainWidget;
	QTabWidget *tabWidget;

	ConfigTab *configTab(const QString &name, bool create = true);
	bool activated;

	void switchTabView(bool tabView);

private slots:
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap);
	~ConfigSection();

	void activate();

	void show() { mainWidget->show(); }
	void hide() { mainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

	void removedConfigTab(const QString &configTabName);

};


#endif
