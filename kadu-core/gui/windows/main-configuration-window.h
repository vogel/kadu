#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

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

class ConfigFileDataManager;
class ConfigurationUiHandler;

/**
	@class MainConfigurationWindow
	@author Vogel
	@short G��wne okno konfiguracyjne.
 **/
class KADUAPI MainConfigurationWindow : public ConfigurationWindow, CompositingAwareObject
{
	Q_OBJECT

	static MainConfigurationWindow *Instance;
	static ConfigFileDataManager *InstanceDataManager;

	static QList<QString> UiFiles;

	static void instanceCreated();

	QPointer<ConfigurationWindow> lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;
	QLineEdit *onStartupSetDescription;
	QCheckBox *userboxTransparency;
	QLineEdit *disconnectDescription;
	QSlider *userboxAlpha;
	QCheckBox *userboxBlur;
	BuddyListBackgroundColorsWidget *buddyColors;
	PluginListWidget *PluginList;

	explicit MainConfigurationWindow();

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
	static MainConfigurationWindow * instance();
	static bool hasInstance();
	static ConfigFileDataManager * instanceDataManager();

	/**
		Rejestracja nowego pliku *.ui (dokonywana przez modu�u).
		Plik uiFile zostanie wczytany wraz z otwarciem g��wnego okna konfiguracyjnego.
		Obiekt uiHandle zostanie poinformowany o stworzeniu okna i b�dzie m�g� doda�
		do niego w�asne interakcje.
		@note Należy pamiętać, aby wywołać tę metodę zawsze przed korespondującym
		      wywołaniem metody \c registerUiHandler() (o ile takie istnieje).
	 **/
	static void registerUiFile(const QString &uiFile);
	/**
		Wyrejestrowanie pliku *.ui i klasy obs�uguj�cej okno konfiguracyjne.
		@note Należy pamiętać, aby wywołać tę metodę zawsze po korespondującym
		      wywołaniu metody \c unregisterUiHandler() (o ile takie istnieje).
	 **/
	static void unregisterUiFile(const QString &uiFile);

	static const char *SyntaxText;
	static const char *SyntaxTextNotify;

	virtual ~MainConfigurationWindow();

	virtual void show();

};

#endif // MAIN_CONFIGURATION_WINDOW_H
