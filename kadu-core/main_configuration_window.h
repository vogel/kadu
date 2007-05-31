#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include "configuration_window.h"

class ConfigComboBox;
class ConfigLineEdit;

class QCheckBox;

class MainConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	QCheckBox *onStartupSetLastDescription;

	ConfigComboBox *browserComboBox;
	ConfigLineEdit *browserCommandLineEdit;

	ConfigComboBox *mailComboBox;
	ConfigLineEdit *mailCommandLineEdit;

	void setLanguages();
	void setIconThemes();
	void setEmoticonThemes();
	void setQtThemes();
	void setToolTipClasses();

	QString findExecutable(const QStringList &paths, const QStringList &executableNames);

	void import_0_5_0_configuration();

private slots:
	void onChangeStartupStatus(int index);
	void onChangeBrowser(int index);
	void onChangeMail(int index);

public:
	static void initModule();
	static void closeModule();

	MainConfigurationWindow();
	virtual ~MainConfigurationWindow();

	virtual void show();
};

#endif // MAIN_CONFIGURATION_WINDOW_H
