/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_TAB_H
#define CONFIG_TAB_H

#include <QMap>

class QTabWidget;
class QScrollArea;
class QVBoxLayout;
class ConfigGroupBox;
class ConfigSection;
class KaduScrollArea;
class QGroupBox;

class ConfigTab
{
	QString name;
	ConfigSection *configSection;

	QMap<QString, ConfigGroupBox *> configGroupBoxes;

	QScrollArea *scrollArea;
	QVBoxLayout *mainLayout;
	QWidget *mainWidget;

public:
	ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *tabWidget);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name, bool create = true);

	void removedConfigGroupBox(const QString &groupBoxName);

	QWidget *widget() { return mainWidget; }
	QWidget *tabWidget() { return mainWidget; }

};

#endif