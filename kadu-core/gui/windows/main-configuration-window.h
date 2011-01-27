#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include "gui/windows/configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QCheckBox;
class QSlider;

class BuddyListBackgroundColorsWidget;
class ConfigComboBox;
class ConfigLineEdit;
class Preview;
class SyntaxEditorWindow;

class MainConfigurationWindow;
/**
	@class ConfigurationUiHandler
	@author Vogel
	@short Klasa bazowa dla klas obs�uguj�cych okno konfiguracyjne.

	Klasa dziedzicz�ca z tej b�dzie informowana o stworzeniu nowego g��wnego okna
	konfiguracyjnego co da jej mo�liwo�� podpi�cia si� pod sygna�y odpowiednich
	kontrolek i odpowiedniej reakcji na nie.
 **/
// TODO: zamiast slota zwyk�a metoda wirtualna?
class KADUAPI ConfigurationUiHandler : public virtual QObject
{
	Q_OBJECT

public:
	explicit ConfigurationUiHandler(QObject *parent = 0) : QObject(parent) {}
	virtual ~ConfigurationUiHandler() {}

public slots:
	/**
		Slot wywo�ywany, gdy g��wne okno konfiguracyjne zostanie stworzone.
	 **/
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) = 0;
};

class ConfigFileDataManager;

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
	static QList<ConfigurationUiHandler *> ConfigurationUiHandlers;

	static void instanceCreated();

	ConfigurationWindow *lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;
	QCheckBox *userboxTransparency;
	QSlider *userboxAlpha;
	ConfigComboBox *emoticonsStyleComboBox;
	ConfigComboBox *emoticonsThemeComboBox;
	ConfigComboBox *emoticonsScalingComboBox;
	BuddyListBackgroundColorsWidget *buddyColors;

	explicit MainConfigurationWindow();

	void setLanguages();

	void setToolTipClasses();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private slots:
	void onChangeEmoticonsTheme(int index);

	void onChangeStartupStatus(int index);

	void showLookChatAdvanced();
	void lookChatAdvancedDestroyed();

	void onInfoPanelSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);

	void setIconThemes();
	void setEmoticonThemes();

	void chatPreviewSyntaxChanged(const QString &syntaxName);

public:
	static MainConfigurationWindow * instance();
	static bool hasInstance();
	static ConfigFileDataManager * instanceDataManager();

	/**
		Rejestracja nowego pliku *.ui (dokonywana przez modu�u).
		Plik uiFile zostanie wczytany wraz z otwarciem g��wnego okna konfiguracyjnego.
		Obiekt uiHandle zostanie poinformowany o stworzeniu okna i b�dzie m�g� doda�
		do niego w�asne interakcje.
	 **/
	static void registerUiFile(const QString &uiFile);
	/**
		Wyrejestrowanie pliku *.ui i klasy obs�uguj�cej okno konfiguracyjne.
	 **/
	static void unregisterUiFile(const QString &uiFile);

	static void registerUiHandler(ConfigurationUiHandler *uiHandler);
	static void unregisterUiHandler(ConfigurationUiHandler *uiHandler);

	static const char *SyntaxText;
	static const char *SyntaxTextNotify;

	virtual ~MainConfigurationWindow();

	virtual void show();

};

#endif // MAIN_CONFIGURATION_WINDOW_H
