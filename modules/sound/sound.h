#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qmap.h>

#include "config_file.h"
#include "modules.h"
#include "gadu.h"
#include "userlist.h"
#include "misc.h"

class SoundSlots : public QObject
{
	Q_OBJECT
	private:
		int muteitem;
		QMap<QString, QString> soundfiles;
		QStringList soundNames;
		QStringList soundTexts;

	private slots:
		void soundPlayer(bool value, bool toolbarChanged=false);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
		void chooseSoundTheme(const QString& string);
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);
		void muteUnmuteSounds();
	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

class SoundManager : public Themes
{
    Q_OBJECT
	private:
		QTime lastsoundtime;
		bool mute;

	private slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToNotAvailable(const UserListElement &ule);
		/* from i ule s± ignorowane, message wskazuje na plik z d¼wiêkiem do odtworzenia
		 * je¿eli message==QString::null, to odtwarzany jest standardowy d¼wiêk dla tego typu 
		 * je¿eli mapa jest!=NULL brane s± z niej nastêpuj±ce warto¶ci:
		 *		"Force"           - bool (wymuszenie odtwarzania mimo wyciszenia)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);

	public slots:
		void play(const QString &path, bool force=false);
		void setMute(const bool& enable);

	public:
		SoundManager(const QString& name, const QString& configname);
		~SoundManager();
		bool isMuted();
		int timeAfterLastSound();
		/**
			Uogólniony deskryptor urz±dzenia d¼wiêkowego.
		**/
		typedef void* SoundDevice;
		/**
			Otwiera urz±dzenie d¼wiêkowe do operacji
			odtwarzania i nagrywania sampli.
			Niektóre implementacje pozwalaj± na otwarcie
			wielu niezale¿nie dzia³aj±cych "po³±czeñ"
			z urz±dzeniami. Wystarczy wtedy kilkukrotnie
			wywo³aæ t± metodê.
			Emituje sygna³ openDeviceImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			@param sample_rate sample rate - np. 8000 lub 48000
			@channels ilo¶æ kana³ów: 1 - mono, 2 - stereo
			@return uogólniony deskryptor urz±dzenia lub NULL je¶li otwarcie siê nie powiod³o.
		**/
		SoundDevice openDevice(int sample_rate, int channels = 1);
		/**
			Zamyka urz±dzenie d¼wiêkowe otwarte za pomoc±
			metody openDevice().
			Niektóre implementacje pozwalaj± na otwarcie
			wielu niezale¿nie dzia³aj±cych "po³±czeñ"
			z urz±dzeniami. Ka¿de otwarte po³±czenie nale¿y
			zamkn±æ za pomoc± tej metody.
			Emituje sygna³ closeDeviceImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			@param device uogólniony deskryptor urz±dzenia.
		**/
		void closeDevice(SoundDevice device);
		/**
			Odtwarza próbkê d¼wiêkow±. Operacja blokuj±ca.
			Mo¿e byæ wywo³ana z innego w±tku (a nawet powinna).
			Emituje sygna³ playSampleImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik do danych sampla
			@length d³ugo¶æ danych sampla
			@return true je¶li odtwarzanie zakoñczy³o siê powodzeniem.
		**/
		bool playSample(SoundDevice device, char* data, int length);
		/**
			Nagrywa próbkê d¼wiêkow±. Operacja blokuj±ca.
			Mo¿e byæ wywo³ana z innego w±tku (a nawet powinna).
			Emituje sygna³ recordSampleImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik na bufor dla danych sampla
			@length d³ugo¶æ sampla do nagrania (wielko¶æ bufora)
			@return true je¶li nagrywanie zakoñczy³o siê powodzeniem.
		**/
		bool recordSample(SoundDevice device, char* data, int length);

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
		/**
			Pod ten sygna³ powinien podpi±æ siê modu³
			d¼wiêkowy je¶li obs³uguje funkcjê odtwarzania
			próbki d¼wiêkowej.
			Wyemitowanie sygna³u oznacza ¿±danie
			otwarcia urz±dzenia d¼wiêkowego do operacji
			odtwarzania i nagrywania sampli.
			@param sample_rate sample rate - np. 8000 lub 48000
			@channels ilo¶æ kana³ów: 1 - mono, 2 - stereo
			@device zwrócony uogólniony deskryptor urz±dzenia lub NULL je¶li otwarcie siê nie powiod³o.
		**/
		void openDeviceImpl(int sample_rate, int channels, SoundDevice& device);
		/**
			Pod ten sygna³ powinien podpi±æ siê modu³
			d¼wiêkowy je¶li obs³uguje funkcjê odtwarzania
			próbki d¼wiêkowej.
			Wyemitowanie sygna³u oznacza ¿±danie
			Zamkniêcia urz±dzenia d¼wiêkowego otwartegp za pomoc±
			metody openDevice().
			@param device uogólniony deskryptor urz±dzenia.
		**/
		void closeDeviceImpl(SoundDevice device);
		/**
			Pod ten sygna³ powinien podpi±æ siê modu³
			d¼wiêkowy je¶li obs³uguje funkcjê odtwarzania
			próbki d¼wiêkowej.
			Wyemitowanie sygna³u oznacza ¿±danie
			odtworzenia próbki d¼wiêkowej.
			Modu³ powinien oddaæ sterowanie dopiero po
			odtworzeniu próbki.
			Sygna³ zazwyczaj bêdzie emitowany z innego
			w±tku i slot musi byæ do tego przystosowany.
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik do danych sampla
			@length d³ugo¶æ danych sampla
			@result zwrócony rezultat operacji - true je¶li odtwarzanie zakoñczy³o siê powodzeniem.
		**/
		void playSampleImpl(SoundDevice device, char* data, int length, bool& result);
		/**
			Pod ten sygna³ powinien podpi±æ siê modu³
			d¼wiêkowy je¶li obs³uguje funkcjê odtwarzania
			próbki d¼wiêkowej.
			Wyemitowanie sygna³u oznacza ¿±danie
			nagrania próbki d¼wiêkowej.
			Modu³ powinien oddaæ sterowanie dopiero po
			nagraniu próbki.
			Sygna³ zazwyczaj bêdzie emitowany z innego
			w±tku i slot musi byæ do tego przystosowany.
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik na bufor dla danych sampla
			@length d³ugo¶æ sampla do nagrania (wielko¶æ bufora)
			@result zwrócony rezultat operacji - true je¶li nagrywanie zakoñczy³o siê powodzeniem.
		**/
		void recordSampleImpl(SoundDevice device, char* data, int length, bool& result);		
};

extern SoundManager* sound_manager;

#endif
