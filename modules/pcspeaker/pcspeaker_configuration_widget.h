#ifndef PCSPEAKER_CONFIG_WIDGET
#define PCSPEAKER_CONFIG_WIDGET

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>
#include "../notify/notify.h"

class PCSpeakerConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT
	
private slots:
	void test();

public:
	PCSpeakerConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual ~PCSpeakerConfigurationWidget();

	virtual void loadNotifyConfigurations() {};
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

#endif
