#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include <qpair.h>
#include <qvaluelist.h>

#include "configuration_window.h"

class ConfigComboBox;
class ConfigLineEdit;

class ChatMessage;
class Preview;

class QCheckBox;

class MainConfigurationWindow;

class ConfigurationUiHandler : public virtual QObject
{
	Q_OBJECT

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) = 0;
};

class MainConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	static MainConfigurationWindow *Instance;
	static QValueList<QPair<QString, ConfigurationUiHandler *> > UiFiles;

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
	QValueList<ChatMessage *> chatMessages;

	void setLanguages();
	void setIconThemes();
	void setEmoticonThemes();
	void setQtThemes();
	void setToolTipClasses();

	QString findExecutable(const QStringList &paths, const QStringList &executableNames);
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

public:
	static MainConfigurationWindow * instance()
	{
		if (!Instance)
		{
			Instance = new MainConfigurationWindow();
			instanceCreated();
		}

		return Instance;
	}

	static void registerUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler);
	static void unregisterUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler);

	MainConfigurationWindow();
	virtual ~MainConfigurationWindow();

	virtual void show();
};

#endif // MAIN_CONFIGURATION_WINDOW_H
