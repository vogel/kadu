#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include "configuration/configuration-window.h"

#include "exports.h"

class QCheckBox;

class ChatMessage;
class ConfigComboBox;

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
class KADUAPI MainConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	typedef QPair<QString, ConfigurationUiHandler *> ConfigurationHandelUiPair;
	static MainConfigurationWindow *Instance;
	static ConfigFileDataManager *InstanceDataManager;

	static QList<ConfigurationHandelUiPair> UiFiles;

	static void instanceCreated();

	ConfigurationWindow *lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;

	ConfigComboBox *emoticonsStyleComboBox;
	ConfigComboBox *emoticonsThemeComboBox;

	ConfigComboBox *browserComboBox;
	ConfigLineEdit *browserCommandLineEdit;

	ConfigComboBox *mailComboBox;
	ConfigLineEdit *mailCommandLineEdit;

	Preview *chatPreview;
	QList<ChatMessage *> chatMessages;

	void setLanguages();

	void setQtThemes();
	void setToolTipClasses();

	static QString findExecutable(const QStringList &paths, const QStringList &executableNames);
	// TODO: remove append parameter
	void prepareChatPreview(Preview *preview, bool append = false);

private slots:
	void onChangeEmoticonsStyle(int index);

	void onChangeStartupStatus(int index);
	void onChangeBrowser(int index);
	void onChangeMail(int index);

	void showLookChatAdvanced();
	void lookChatAdvancedDestroyed();

	void onChatSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);
	void chatSyntaxFixup(QString &syntax);
	void chatFixup(Preview *preview);

	void setIconThemes();
	void setEmoticonThemes();

public:
	static MainConfigurationWindow * instance();

	/**
		Rejestracja nowego pliku *.ui (dokonywana przez modu�u).
		Plik uiFile zostanie wczytany wraz z otwarciem g��wnego okna konfiguracyjnego.
		Obiekt uiHandle zostanie poinformowany o stworzeniu okna i b�dzie m�g� doda�
		do niego w�asne interakcje.
	 **/
	static void registerUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler);
	/**
		Wyrejestrowanie pliku *.ui i klasy obs�uguj�cej okno konfiguracyjne.
	 **/
	static void unregisterUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler);

	MainConfigurationWindow();
	virtual ~MainConfigurationWindow();

	virtual void show();

	static QString getBrowserExecutable(int browserIndex);
	static QString getEMailExecutable(int emailIndex);

	static QString browserIndexToString(int browserIndex);
	static QString emailIndexToString(int emailIndex);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
};

#endif // MAIN_CONFIGURATION_WINDOW_H
