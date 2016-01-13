#pragma once

#include <QtCore/QPointer>

#include "gui/windows/configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QCheckBox;
class QLineEdit;
class QSlider;

class BuddyListBackgroundColorsWidget;
class ConfigComboBox;
class ConfigLineEdit;
class PluginListWidget;
class Preview;
class SyntaxEditorWindow;

class MainConfigurationWindow;

class ConfigurationUiHandler;

/**
	@class MainConfigurationWindow
	@author Vogel
	@short G��wne okno konfiguracyjne.
 **/
class KADUAPI MainConfigurationWindow : public ConfigurationWindow, CompositingAwareObject
{
	Q_OBJECT

	QPointer<ConfigurationWindow> lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;
	QLineEdit *onStartupSetDescription;
	QCheckBox *userboxTransparency;
	QLineEdit *disconnectDescription;
	QSlider *userboxAlpha;
	QCheckBox *userboxBlur;
	BuddyListBackgroundColorsWidget *buddyColors;
	PluginListWidget *PluginList;

	void setLanguages();

	void setToolTipClasses();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private slots:
	void onChangeStartupStatus(int index);
	void onChangeStartupDescription(int index);
	void onChangeShutdownStatus(int index);
	void showLookChatAdvanced();
	void installIconTheme();
	void setIconThemes();
	void applied();
	void configurationUiHandlerAdded(ConfigurationUiHandler *configurationUiHandler);
	void configurationUiHandlerRemoved(ConfigurationUiHandler *configurationUiHandler);

public:
	static const char *SyntaxText;
	static const char *SyntaxTextNotify;

	explicit MainConfigurationWindow(QObject *parent = nullptr);
	virtual ~MainConfigurationWindow();

	virtual void show();

};
