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

#include <QtCore/QMap>

class QGroupBox;
class QScrollArea;
class QTabWidget;
class QWidget;
class QVBoxLayout;

class ConfigGroupBox;
class ConfigSection;
class KaduScrollArea;

class ConfigTab
{
	QString MyName;
	ConfigSection *MyConfigSection;

	QMap<QString, ConfigGroupBox *> MyConfigGroupBoxes;

	QScrollArea *MyScrollArea;
	QVBoxLayout *MyMainLayout;
	QWidget *MyMainWidget;

public:
	ConfigTab(const QString &name, ConfigSection *configSection, QWidget *mainWidget);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name, bool create = true);

	void removedConfigGroupBox(const QString &groupBoxName);

	QString name() { return MyName; }
	QWidget * widget() { return MyMainWidget; }
	QWidget * tabWidget() { return MyMainWidget; }
	QWidget * scrollWidget();

};

#endif
