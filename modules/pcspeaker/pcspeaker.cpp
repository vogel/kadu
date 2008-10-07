/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /*
 * autor:
 * Tomasz "Dorr(egaray)" Rostanski
 * rozteck (at) interia.pl
 *
 */
 
#include "pcspeaker.h"
#include "pcspeaker_configuration_widget.h"
#include "debug.h"
#include "config_file.h"
#include "misc.h"
#include <modules.h>

#include <QtGui/QLineEdit>
#include <QtGui/QSlider>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>

//czestotliwosci dzwiekow
//wiersze - dzwieki: C, C#, D, D#, E, F, F#, G, G#, A, A#, B
//kolumny to oktawy - od 0 do 7
int dzwieki[96] = {
	16,33, 65,131,262,523,1046,2093,
	17,35, 69,139,277,554,1109,2217,
	18,37, 73,147,294,587,1175,2349,
	19,39, 78,155,311,622,1244,2489,
	21,41, 82,165,330,659,1328,2637,
	22,44, 87,175,349,698,1397,2794,
	23,46, 92,185,370,740,1480,2960,
	24,49, 98,196,392,784,1568,3136,
	26,52,104,208,415,831,1661,3322,
	27,55,110,220,440,880,1760,3520,
	29,58,116,233,466,932,1865,3729,
	31,62,123,245,494,988,1975,3951};

PCSpeaker *pcspeaker;

void PCSpeaker::beep(int pitch, int duration) {
	if (pitch == 0)
		usleep(duration * 200);
	else {
		XKeyboardState s;			//zachowuje stare parametry dzwieku
		XGetKeyboardControl(xdisplay, &s);
		XKeyboardControl v;			//dla 0 nie wysyla zadnego dzwieku tylko odczekuje podany czas
		v.bell_pitch = pitch;			//dzwiek w Hz
		v.bell_duration = duration;		//czas trwania w ms
		v.bell_percent = 100;			//ustawiamy glosnosc na maks
		XChangeKeyboardControl(xdisplay, (KBBellPitch | KBBellDuration | KBBellPercent), &v); //ustawia parametry dzwieku
		XBell(xdisplay, volume);  		//robimy pik skalujac glosnosc wzgledem maksa
		XFlush(xdisplay);			//czysci bufor wywalajac na display i powodujac de facto pik
		usleep(pitch * 100);			//poczeka az skonczy pipac
		v.bell_pitch = s.bell_pitch;		//odzyskuje stare parametry
		v.bell_duration = s.bell_duration;
		v.bell_percent = s.bell_percent;
		XChangeKeyboardControl(xdisplay, (KBBellPitch | KBBellDuration | KBBellPercent), &v); //ustawia poprzednie parametry dzwieku
    }
}

extern "C" int pcspeaker_init() {
	kdebugf();

	pcspeaker = new PCSpeaker();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/pcspeaker.ui"), pcspeaker);

	kdebugf2();
	return 0;
}


extern "C" void pcspeaker_close() {
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/pcspeaker.ui"), pcspeaker);
	delete(pcspeaker);

	kdebugf2();
} 


PCSpeaker::PCSpeaker() {
	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "PC Speaker"), this);
	createDefaultConfiguration();
}


PCSpeaker::~PCSpeaker() {
	notification_manager->unregisterNotifier("PC Speaker");
}

void PCSpeaker::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) {
	connect(mainConfigurationWindow->widgetById("pcspeaker/test1"), SIGNAL(clicked()),
		this, SLOT(test1()));
	connect(mainConfigurationWindow->widgetById("pcspeaker/test2"), SIGNAL(clicked()),
		this, SLOT(test2()));
	connect(mainConfigurationWindow->widgetById("pcspeaker/test3"), SIGNAL(clicked()),
		this, SLOT(test3()));
	connect(mainConfigurationWindow->widgetById("pcspeaker/test4"), SIGNAL(clicked()),
		this, SLOT(test4()));
	connect(mainConfigurationWindow->widgetById("pcspeaker/test5"), SIGNAL(clicked()),
		this, SLOT(test5()));
}

NotifierConfigurationWidget *PCSpeaker::createConfigurationWidget(QWidget *parent, char *name) {
    return NULL;
}

void PCSpeaker::notify(Notification *notification) {
	kdebugf();
	notification->acquire();
	
	QString linia;
	if (notification->type().compare("NewChat") == 0) {
		linia = config_file.readEntry("PC Speaker", "OnChatPlayString");
	}
	else if (notification->type().compare("NewMessage") == 0) {
		linia = config_file.readEntry("PC Speaker", "OnMessagePlayString");
	}
	else if (notification->type().compare("ConnectionError") == 0) {
		linia = config_file.readEntry("PC Speaker", "OnConnectionErrorPlayString");
	}
	else if (notification->type().contains("StatusChanged", true)) {
		linia = config_file.readEntry("PC Speaker", "OnNotifyPlayString");
	}
	else {
		linia = config_file.readEntry("PC Speaker", "OnOtherMessagePlayString");
	}

	if (linia.length() > 0)
		parseAndPlay(linia);
	else
	    kdebugmf(KDEBUG_ERROR, "\n\nMelody String is empty!\n");

	notification->release();
	kdebugf2();
}

void PCSpeaker::ParseStringToSound(QString linia, int tablica[21], int tablica2[21]) {
	unsigned int dlugosc = linia.length();
	linia = linia.upper();							//zamienia na wielkie litery
	int pom, k = 0;								//k - indeksuje tablice dzwiekow a pom przechowuje indeks dzwieku w tabl.
	char znak, pom3;							//aktualny znak przetwazanego Qstringa
	unsigned int i;
	if (dlugosc > 0) {
		for (i=0; i<dlugosc; ++i) {					//dla kazdego dzwieku
			if (k == 20) break;
			znak=linia[i].latin1();
			switch (znak) {						//oblicz przesuniecie w tabl. dzwiekow
				case 'C':  pom=0;	break;
				case 'D':  pom=2;	break;
				case 'E':  pom=4;	break;
				case 'F':  pom=5;	break;
				case 'G':  pom=7;	break;
				case 'A':  pom=9;	break;
				case 'B':  pom=11;	break;
				case '_':  {	tablica[k]=0; 			//jak _ to zrob pauze
						pom=-1;
						if (linia[i+1]=='/')	{	//to jest ustawienie dlugosci pauzy
							if (linia[i+2]=='F') pom3=16;
							else if ((linia[i+2]>='1') && (linia[i+2]<='8'))
								pom3=linia[i+2].latin1()-48;
							else pom3=1;
								tablica2[k]=(1000/pom3);
							i+=2;
						}
						else tablica2[k]=1000;		//jak nie podana to 1000
							++k;
					} break;
					default: pom=-1;					
			}
			if (pom>=0) {
				pom*=8;
				if (linia[i+1]=='#')	{			//jak byl polton
					pom+=8;					//ustaw przesuniecie
					++i;					//przeskocz dalej
				}
				if ((linia[i+1]>='0') && (linia[i+1]<='7')) {
					pom+=linia[i+1].latin1()-48;		//ustaw przesuniecie o podana oktawe
					++i;					//przeskocz dalej
				}
				if (linia[i+1]=='#')	{			//jak byl polton
					pom+=8;					//ustaw przesuniecie
					++i;					//przeskocz dalej
				}
				tablica[k]=dzwieki[pom];			//wpisz do tablicy czestotliwosc dzwieku
				if (linia[i+1]=='/') {
						pom3=0;				//ustaw dlugosc dzwieku
						if (linia[i+2]=='F') pom3=16;
						else if ((linia[i+2]>='1') && (linia[i+2]<='8')) pom3=linia[i+2].latin1()-48;
						else pom3=1;
						tablica2[k]=(1000/pom3);
						i+=2;
				}
				else tablica2[k]=1000;				//jak nie podana to walnij 1000
				++k;						//przeskocz dalej w tablicy dzwiekow
			}
		}
	}
	tablica[k]=-1;								//na koncu zawsze musi byc -1
}

void PCSpeaker::test1() {
	QString linia = dynamic_cast<QLineEdit *>(MainConfigurationWindow::instance()->widgetById("pcspeaker/OnMessagePlayString"))->text();
	if (linia.length()>0)
		parseAndPlay(linia);
}

void PCSpeaker::test2() {
	QString linia = dynamic_cast<QLineEdit *>(MainConfigurationWindow::instance()->widgetById("pcspeaker/OnChatPlayString"))->text();
	if (linia.length()>0)
		parseAndPlay(linia);
}

void PCSpeaker::test3() {
	QString linia = dynamic_cast<QLineEdit *>(MainConfigurationWindow::instance()->widgetById("pcspeaker/OnNotifyPlayString"))->text();
	if (linia.length()>0)
		parseAndPlay(linia);
}

void PCSpeaker::test4() {
	QString linia = dynamic_cast<QLineEdit *>(MainConfigurationWindow::instance()->widgetById("pcspeaker/OnConnectionErrorPlayString"))->text();
	if (linia.length()>0)
		parseAndPlay(linia);
}

void PCSpeaker::test5() {
	QString linia = dynamic_cast<QLineEdit *>(MainConfigurationWindow::instance()->widgetById("pcspeaker/OnOtherMessagePlayString"))->text();
	if (linia.length()>0)
		parseAndPlay(linia);
}

void PCSpeaker::play(int sound[21], int soundlength[20]) {
	xdisplay = XOpenDisplay(NULL);
	for (int i=0; i<20; ++i) {
		if (sound[i] == -1) break;	
			beep(sound[i], soundlength[i]);
	}
	XCloseDisplay(pcspeaker->xdisplay);	
}

void PCSpeaker::parseAndPlay(QString linia) {
	volume = config_file.readNumEntry("PC Speaker", "SpeakerVolume");	
	int sound[21], soundLength[20];
	ParseStringToSound(linia, sound, soundLength);
	play(sound, soundLength);	
}

void PCSpeaker::createDefaultConfiguration() {
	config_file.addVariable("PC Speaker", "OnChatPlayString", "C4/2");
	config_file.addVariable("PC Speaker", "OnMessagePlayString", "F2/2");
	config_file.addVariable("PC Speaker", "OnConnectionErrorPlayString", "D3/4");
	config_file.addVariable("PC Speaker", "OnNotifyPlayString", "A3/2");
	config_file.addVariable("PC Speaker", "OnOtherMessagePlayString", "E4/4");
}
