#ifndef CONFIGURATION_WINDOW_WIDGETS_H
#define CONFIGURATION_WINDOW_WIDGETS_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdom.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qvbox.h>

#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "preview.h"
#include "select_file.h"
#include "select_font.h"
#include "syntax_editor.h"

class ConfigGroupBox;

/**
	&lt;widget id="id" tool-tip="toolTip" /&gt;

	@arg id - id dostêpne dla aplikacji/modu³ów
	@arg toolTip - podpowied¼ dla u¿ytkownika
 **/
class ConfigWidget
{
	friend class ConfigurationWindow;

protected:
	ConfigGroupBox *parentConfigGroupBox;
	QString widgetCaption;
	QString toolTip;

	virtual void createWidgets() = 0;

public:
	ConfigWidget(ConfigGroupBox *parentConfigGroupBox);
	ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox);
	virtual ~ConfigWidget() {}

	virtual void loadConfiguration() = 0;
	virtual void saveConfiguration() = 0;

	virtual void show() = 0;
	virtual void hide() = 0;
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;widget config-section="section" config-item="item" /&gt;

	@arg section - sekcja w której trzymany jest element
	@arg item - element, w którym trzymana jest warto¶æ parametry
 **/
class ConfigWidgetValue : public ConfigWidget
{
protected:
	QString section;
	QString item;

public:
	ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox);
	ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox);
	virtual ~ConfigWidgetValue() {}

	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;line-edit caption="caption" id="id" /&gt;
 **/
class ConfigLineEdit : public QLineEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigLineEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;gg-password-edit caption="caption" id="id" /&gt;
 **/
class ConfigGGPasswordEdit : public ConfigLineEdit
{
public:
	ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigGGPasswordEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

/**
	&lt;check-box caption="caption" id="id" /&gt;
 **/
class ConfigCheckBox : public QCheckBox, public ConfigWidgetValue
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigCheckBox() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;spin-box caption="caption" id="id" min-value="minValue" max-value="maxValue" step="step" /&gt;

	@arg minValue - minimalna warto¶æ (obowi±zkowo)
	@arg maxValue - maksymalna warto¶æ (obowi±zkowo)
	@arg step - krok warto¶ci (nieobowiazkowo, domy¶lnie 1)
 **/
class ConfigSpinBox : public QSpinBox, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	virtual ~ConfigSpinBox();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;combo-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/combo-box&gt;

	@arg value - warto¶æ zapisana do pliku konfiguracyjnego
	@arg caption - warto¶æ wy¶wietlana
 **/
class ConfigComboBox : public QComboBox, public ConfigWidgetValue
{
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	virtual ~ConfigComboBox();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue() { return itemValues[currentItem()]; }

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;hot-key-edit caption="caption" id="id" /&gt;
 **/
class ConfigHotKeyEdit : public HotKeyEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigHotKeyEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;path-list-edit caption="caption" id="id" /&gt;
 **/
class ConfigPathListEdit : public PathListEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigPathListEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;colob-button caption="caption" id="id" /&gt;
 **/
class ConfigColorButton : public ColorButton, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigColorButton();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;select-font caption="caption" id="id" /&gt;
 **/
class ConfigSelectFont : public SelectFont, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSelectFont();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

/**
	&lt;syntax-editro caption="caption" id="id" /&gt;
 **/
class ConfigSyntaxEditor : public SyntaxEditor, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSyntaxEditor();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;action-button caption="caption" id="id" /&gt;
 **/
class ConfigActionButton : public QPushButton, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigActionButton(const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigActionButton() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;select-file caption="caption" id="id" type="type" /&gt;

	@arg type all = wszystkie pliki, images = obrazki, audio = pliki d¼wiêkowe
 **/
class ConfigSelectFile : public SelectFile, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSelectFile(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QString &type, ConfigGroupBox *parentConfigGroupBox,  char *name = 0);
	ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSelectFile();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;preview caption="caption" id="id" /&gt;

	Podgl±d sk³adni.
 **/
class ConfigPreview : public Preview, public ConfigWidget
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigPreview(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigPreview();

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;slider caption="caption" id="id" /&gt;

	@arg minValue - minimalna warto¶æ (obowi±zkowo)
	@arg maxValue - maksymalna warto¶æ (obowi±zkowo)
	@arg pageStep - krok warto¶ci (obowi±zkowo)
 **/
class ConfigSlider : public QSlider, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, const char *name);
	ConfigSlider(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigSlider();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

/**
	&lt;label caption="caption" id="id" /&gt;
 **/
class ConfigLabel : public QLabel, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigLabel(const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	ConfigLabel(ConfigGroupBox *parentConfigGroupBox, char *name = 0);
	virtual ~ConfigLabel() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

/**
	&lt;list-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/list-box&gt;

	@arg value - warto¶æ zapisana do pliku konfiguracyjnego
	@arg caption - warto¶æ wy¶wietlana
 **/
class ConfigListBox : public QListBox, public ConfigWidget
{
	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigListBox(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	ConfigListBox(ConfigGroupBox *parentConfigGroupBox, const char *name = 0);
	virtual ~ConfigListBox();

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	QString currentItemValue() { return itemValues[currentItem()]; }

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);
};

#endif // CONFIGURATION_WINDOW_WIDGETS_H
