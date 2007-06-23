#ifndef KADU_SPEAK_H
#define KADU_SPEAK_H
#include <qobject.h>
#include <qstring.h>

#include "../notify/notify.h"

#include "main_configuration_window.h"
#include "misc.h"
#include "protocol.h"
#include "userlist.h"

class QSlider;

class SelectFile;

/**
 * @defgroup speech Speech
 * @{
 */

class SpeechConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, QString> maleFormat;
	QMap<QString, QString> femaleFormat;
	QString currentNotifyEvent;

	QLineEdit *maleLineEdit;
	QLineEdit *femaleLineEdit;

public:
	SpeechConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual ~SpeechConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

class Speech : public Notifier, public ConfigurationUiHandler
{
	Q_OBJECT

	QTime lastSpeech;

	void import_0_5_0_Configuration();
	void import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to);

	QSlider *frequencySlider;
	QSlider *tempoSlider;
	QSlider *baseFrequencySlider;
	QCheckBox *melodyCheckBox;

	SelectFile *programSelectFile;

	ConfigComboBox *soundSystemComboBox;
	QLineEdit *dspDeviceLineEdit;
	QCheckBox *klattSyntCheckBox;

private slots:
	void say(const QString &s,
		const QString &path = QString::null,
		bool klatt = false, bool melodie = false,
		const QString &sound_system = QString::null, const QString &device = QString::null,
		int freq = 0, int tempo = 0, int basefreq = 0);

	void testSpeech();
	void soundSystemChanged(int index);

public:
	Speech();
	virtual ~Speech();

	virtual void notify(Notification *notification);

	void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);
};

extern Speech *speech;

/** @} */

#endif
