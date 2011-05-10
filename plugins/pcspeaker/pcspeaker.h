#ifndef PCSPEAKER_H
#define PCSPEAKER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

#include "notify/notifier.h"
#include "gui/windows/main-configuration-window.h"
#include "plugins/generic-plugin.h"

class PCSpeakerConfigurationWidget;

class PCSpeaker : public Notifier, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	public:
		PCSpeaker(QObject *parent = NULL);
		~PCSpeaker();

		virtual int init(bool firstLoad);
		virtual void done();

		virtual void notify(Notification *notification);
		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
		void createDefaultConfiguration();
		void parseAndPlay(QString linia);

		static PCSpeaker *instance() { return Instance; }

	private:
#ifdef Q_WS_X11
		Display *xdisplay;
#endif
		PCSpeakerConfigurationWidget *configWidget;
		static PCSpeaker *Instance;
		int volume;
		void ParseStringToSound(QString linia, int tablica[21], int tablica2[20]);
		void beep(int pitch, int duration);
		void play(int sound[21], int soundlength[20]);
};

#endif
