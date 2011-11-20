#ifndef PCSPEAKER_CONFIG_WIDGET
#define PCSPEAKER_CONFIG_WIDGET

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLineEdit;
class QPushButton;

class PCSpeakerConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QLineEdit *soundEdit;
	QPushButton *testButton;
	QMap<QString, QString> Sounds;
	QString CurrentNotifyEvent;

private slots:
	void test();

public:
	PCSpeakerConfigurationWidget(QWidget *parent = 0);
	virtual ~PCSpeakerConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

#endif
