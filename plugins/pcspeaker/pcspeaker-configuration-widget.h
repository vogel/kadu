#ifndef PCSPEAKER_CONFIG_WIDGET
#define PCSPEAKER_CONFIG_WIDGET

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class PCSpeakerNotifier;

class QLineEdit;
class QPushButton;

class PCSpeakerConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QPointer<PCSpeakerNotifier> m_notifier;

	QLineEdit *soundEdit;
	QPushButton *testButton;
	QMap<QString, QString> Sounds;
	QString CurrentNotificationEvent;

private slots:
	void test();

public:
	PCSpeakerConfigurationWidget(PCSpeakerNotifier *notifier, QWidget *parent = 0);
	virtual ~PCSpeakerConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

#endif
