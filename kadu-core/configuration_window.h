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

class QGridLayout;
class QGroupBox;
class QVBox;

class ConfigGroupBox
{
	QGroupBox *groupBox;
	QWidget *container;
	QGridLayout *gridLayout;

public:
	ConfigGroupBox(QGroupBox *groupBox);
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

	ConfigSection *configSection(const QString &name);

	void appendUiSectionFromDom(QDomNode sectionNode);
	void appendUiTabFromDom(QDomNode tabNode, const QString &sectionName);
	void appendUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName);
	void appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);

private slots:
	void updateAndCloseConfig();
	void updateConfig();

	void changeSection(const QString &newSectionName);

public:
	ConfigurationWindow();
	virtual ~ConfigurationWindow();

	ConfigGroupBox * configGroupBox(const QString &section, const QString &tab, const QString &groupBox);

	virtual void show();

	void appendUiFile(const QString &fileName);
	void removeUiFile(const QString &fileName) {}

	QWidget *widgetById(const QString &id);

signals:
	void configurationUpdated();

};

#endif // KADU_CONFIGURATION_WINDOW_H
