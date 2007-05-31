#ifndef CONFIGURATION_WINDOW_WIDGETS_H
#define CONFIGURATION_WINDOW_WIDGETS_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>

#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "select_font.h"
#include "syntax_editor.h"

class ConfigGroupBox;

class ConfigWidget
{
protected:
	ConfigGroupBox *parentConfigGroupBox;

	virtual void createWidgets() = 0;

public:
	ConfigWidget(ConfigGroupBox *parentConfigGroupBox);
	virtual ~ConfigWidget() {}

	virtual void loadConfiguration() = 0;
	virtual void saveConfiguration() = 0;

	virtual bool fromDomElement(QDomElement domElement) ;
};

class ConfigWidgetValue : public ConfigWidget
{
protected:
	QString widgetCaption;
	QString section;
	QString item;

public:
	ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox);
	ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, const QString &widgetCaption, const QString &section, const QString &item);
	virtual ~ConfigWidgetValue() {}

	virtual bool fromDomElement(QDomElement domElement);
};

class ConfigLineEdit : public QLineEdit, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigLineEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigGGPasswordEdit : public ConfigLineEdit
{
public:
	ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigGGPasswordEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigCheckBox : public QCheckBox, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigCheckBox() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigSpinBox : public QSpinBox, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	virtual ~ConfigSpinBox() {};

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);
};

class ConfigComboBox : public QComboBox, public ConfigWidgetValue
{
	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	virtual ~ConfigComboBox() {};

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);
};

class ConfigHotKeyEdit : public HotKeyEdit, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigHotKeyEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigPathListEdit : public PathListEdit, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigPathListEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigColorButton : public ColorButton, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigColorButton() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigSelectFont : public SelectFont, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSelectFont() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

class ConfigSyntaxEditor : public SyntaxEditor, public ConfigWidgetValue
{
protected:
	virtual void createWidgets();

public:
	ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSyntaxEditor() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);
};

#endif // CONFIGURATION_WINDOW_WIDGETS_H
