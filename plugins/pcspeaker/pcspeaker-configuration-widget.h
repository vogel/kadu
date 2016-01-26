#ifndef PCSPEAKER_CONFIG_WIDGET
#define PCSPEAKER_CONFIG_WIDGET

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class Configuration;
class IconsManager;
class PCSpeakerNotifier;

class QLineEdit;
class QPushButton;

class PCSpeakerConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PCSpeakerNotifier> m_notifier;

	QLineEdit *soundEdit;
	QPushButton *testButton;
	QMap<QString, QString> Sounds;
	QString CurrentNotificationEvent;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void test();

public:
	PCSpeakerConfigurationWidget(PCSpeakerNotifier *notifier, QWidget *parent = nullptr);
	virtual ~PCSpeakerConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

#endif
