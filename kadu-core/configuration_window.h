/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_CONFIGURATION_WINDOW_H
#define KADU_CONFIGURATION_WINDOW_H

#include <qdom.h>
#include <qgroupbox.h>

#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "select_font.h"
#include "syntax_editor.h"
#include "userbox.h"

class ConfigGroupBox;
class ConfigLineEdit;
class ConfigSection;
class ConfigTab;

class QGridLayout;
class QGroupBox;
class QVBox;

class ConfigGroupBox
{
	QString name;
	ConfigTab *configTab;

	QGroupBox *groupBox;
	QWidget *container;
	QGridLayout *gridLayout;

public:
	ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox);
	~ConfigGroupBox();

	QWidget * widget() { return container; }
	QGridLayout * layout() { return gridLayout; }

	bool empty();

};

class ConfigurationWindow : public QVBox
{
	Q_OBJECT

	QVBox *left;
	QMap<QString, ConfigSection *> configSections;
	ConfigSection *currentSection;

	QMap<QString, QWidget *> widgets;

	QListBox *sectionsListBox;
	QWidget *container;

	void loadConfiguration(QObject *object);
	void saveConfiguration(QObject *object);

	ConfigSection *configSection(const QString &name, bool create = true);

	void processUiFile(const QString &fileName, bool append = true);
	void processUiSectionFromDom(QDomNode sectionNode, bool append = true);
	void processUiTabFromDom(QDomNode tabNode, const QString &sectionName, bool append = true);
	void processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append = true);
	void appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);
	void removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);

private slots:
	void updateAndCloseConfig();
	void updateConfig();

	void changeSection(const QString &newSectionName);

public:
	ConfigurationWindow();
	virtual ~ConfigurationWindow();

	ConfigGroupBox * configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create = true);

	virtual void show();

	void appendUiFile(const QString &fileName);
	void removeUiFile(const QString &fileName);

	QWidget *widgetById(const QString &id);

	void removedConfigSection(const QString &sectionName);

signals:
	void configurationUpdated();

};

#endif // KADU_CONFIGURATION_WINDOW_H
