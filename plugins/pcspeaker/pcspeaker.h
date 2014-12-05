#ifndef PCSPEAKER_H
#define PCSPEAKER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QWidget>

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include <QtX11Extras/QX11Info>
#endif

#include "gui/windows/main-configuration-window.h"
#include "notify/notifier.h"
#include "plugin/plugin-root-component.h"

class PCSpeakerConfigurationWidget;

class PCSpeaker : public Notifier, public PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	public:
		PCSpeaker(QObject *parent = NULL);
		~PCSpeaker();

		virtual bool init(bool firstLoad);
		virtual void done();

		virtual void notify(Notification *notification);
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
		void createDefaultConfiguration();
		void parseAndPlay(QString linia);

		static PCSpeaker *instance() { return Instance; }

	private:
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
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
