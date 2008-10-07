#ifndef PCSPEAKER_H
#define PCSPEAKER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>
#include "../notify/notify.h"

class PCSpeaker : public Notifier, public ConfigurationUiHandler
{	
	Q_OBJECT
	public:
		PCSpeaker();
		~PCSpeaker();

		virtual void notify(Notification *notification);
		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {};
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);
		void createDefaultConfiguration();

	private:
		Display *xdisplay;
		int volume;
		void ParseStringToSound(QString linia, int tablica[21], int tablica2[20]);
		void beep(int pitch, int duration);
		void parseAndPlay(QString linia);
		void play(int sound[21], int soundlength[20]);
	private slots:
		void test1();
		void test2();
		void test3();
		void test4();
		void test5();
};

extern PCSpeaker *pcspeaker;
#endif
