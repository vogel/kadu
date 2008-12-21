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

#include <QObject>
#include <QMap>
#include <QtGui/QTabWidget>
#include "gui/widgets/configuration/configuration-window.h"

class ConfigGroupBox;
class ConfigTab;
//class ConfigurationWindow;
class ConfigWidget;

class QGridLayout;
class QGroupBox;
class QListWidgetItem;
//class QTabWidget;

class ConfigSection : public QObject
{
	Q_OBJECT

	QString name;
	ConfigurationWindow *configurationWindow;
	QString pixmap;

	QListWidgetItem *listWidgetItem;
	QMap<QString, ConfigTab *> configTabs;

	QTabWidget *mainWidget;

	ConfigTab *configTab(const QString &name, bool create = true);
	bool activated;

private slots:
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap);
	~ConfigSection();

	void activate();

	void show() { mainWidget->show(); }
	void hide() { mainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

	void removedConfigTab(const QString &configTabName);

};


#endif
