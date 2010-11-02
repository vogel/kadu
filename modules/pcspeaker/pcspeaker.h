#ifndef PCSPEAKER_H
#define PCSPEAKER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

#include "notify/notifier.h"
#include "gui/windows/main-configuration-window.h"

class PCSpeakerConfigurationWidget;

class PCSpeaker : public Notifier
{	
	Q_OBJECT
	public:
		PCSpeaker();
		~PCSpeaker();

		virtual void notify(Notification *notification);
		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
		void createDefaultConfiguration();
		void parseAndPlay(QString linia);

	private:
#ifdef Q_WS_X11
		Display *xdisplay;
#endif
		PCSpeakerConfigurationWidget *configWidget;
		int volume;
		void ParseStringToSound(QString linia, int tablica[21], int tablica2[20]);
		void beep(int pitch, int duration);
		void play(int sound[21], int soundlength[20]);
};

extern PCSpeaker *pcspeaker;
#endif
