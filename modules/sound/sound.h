#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qthread.h>

#include "config_file.h"
#include "modules.h"
#include "gadu.h"
#include "userlist.h"
#include "misc.h"
#include "message_box.h"

/**
	Uogólniony deskryptor urz±dzenia d¼wiêkowego.
**/
typedef void* SoundDevice;

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SoundPlayThread : public QObject, public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		const int16_t* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore PlayingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual void customEvent(QCustomEvent* event);

	public:
		SoundPlayThread(SoundDevice device);
		void playSample(const int16_t* data, int length);
		void stop();
		
	signals:
		void samplePlayed(SoundDevice device);
};

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SoundRecordThread : public QObject, public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		int16_t* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore RecordingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual void customEvent(QCustomEvent* event);

	public:
		SoundRecordThread(SoundDevice device);
		void recordSample(int16_t* data, int length);
		void stop();
		
	signals:
		void sampleRecorded(SoundDevice device);
};

class SoundSlots : public QObject
{
	Q_OBJECT
	private:
		int muteitem;
		QMap<QString, QString> soundfiles;
		QStringList soundNames;
		QStringList soundTexts;
		MessageBox* SamplePlayingTestMsgBox;
		SoundDevice SamplePlayingTestDevice;
		int16_t* SamplePlayingTestSample;
		MessageBox* SampleRecordingTestMsgBox;
		SoundDevice SampleRecordingTestDevice;
		int16_t* SampleRecordingTestSample;
		MessageBox* FullDuplexTestMsgBox;
		SoundDevice FullDuplexTestDevice;
		int16_t* FullDuplexTestSample;

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
		void testSamplePlaying();
		void samplePlayingTestSamplePlayed(SoundDevice device);
		void testSampleRecording();
		void sampleRecordingTestSampleRecorded(SoundDevice device);
		void sampleRecordingTestSamplePlayed(SoundDevice device);
		void testFullDuplex();
		void fullDuplexTestSampleRecorded(SoundDevice device);
		void closeFullDuplexTest();

	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

class SoundManager : public Themes
{
    Q_OBJECT
	private:
		friend class SoundPlayThread;
		friend class SoundRecordThread;
		QTime lastsoundtime;
		bool mute;
		QMap<SoundDevice, SoundPlayThread*> PlayingThreads;
		QMap<SoundDevice, SoundRecordThread*> RecordingThreads;

	private slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToInvisible(const UserListElement &ule);
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
			Je¶li w³±czyli¶my operacje nieblokuj±ce to
			metoda ta czeka na zakoñczenie trwaj±cych operacji
			i koñczy dzia³anie w±tków.
			Emituje sygna³ closeDeviceImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			@param device uogólniony deskryptor urz±dzenia.
		**/
		void closeDevice(SoundDevice device);
		/**
			Powo³uje do ¿ycia w±tki zajmuj±ce siê odtwarzaniem
			i nagrywaniem próbek dla danego po³±czenia z
			urz±dzeniem d¼wiêkowym.
			Od tej chwili playSample() i recordSample()
			bêd± operacjami nieblokuj±cymi.
			@param device uogólniony deskryptor urz±dzenia.
		**/
		void enableThreading(SoundDevice device);
		/**
			Odtwarza próbkê d¼wiêkow±. Standardowo jest to
			operacja blokuj±ca. Mo¿e byæ wywo³ana z innego
			w±tku (a nawet powinna).
			Emituje sygna³ playSampleImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			Po uprzednim wywo³aniu enableThreading() dzia³anie
			metoda jest nieblokuj±ca i przekazuje jedynie polecenie
			odtwarzania do w±tku.
			W takim wypadku nale¿y uwa¿aæ, aby nie zwolniæ pamiêci
			zajmowanej przez dane sampla zanim odtwarzanie siê nie
			zakoñczy.
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik do danych sampla
			@length d³ugo¶æ danych sampla
			@return true je¶li odtwarzanie zakoñczy³o siê powodzeniem.
		**/
		bool playSample(SoundDevice device, const int16_t* data, int length);
		/**
			Nagrywa próbkê d¼wiêkow±. Standardowo jest to
			operacja blokuj±ca. Mo¿e byæ wywo³ana z innego
			w±tku (a nawet powinna).
			Emituje sygna³ recordSampleImpl() w celu
			przekazania ¿±dania do konkrentego modu³u
			d¼wiêkowego.
			Po uprzednim wywo³aniu enableThreading() dzia³anie
			metoda jest nieblokuj±ca i przekazuje jedynie polecenie
			nagrywania do w±tku.
			W takim wypadku nale¿y uwa¿aæ, aby nie zwolniæ pamiêci
			bufora na dane sampla zanim nagrywanie siê nie
			zakoñczy.			
			@param device uogólniony deskryptor urz±dzenia
			@data wska¼nik na bufor dla danych sampla
			@length d³ugo¶æ sampla do nagrania (wielko¶æ bufora)
			@return true je¶li nagrywanie zakoñczy³o siê powodzeniem.
		**/
		bool recordSample(SoundDevice device, int16_t* data, int length);

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
		/**
			Sygna³ emitowany gdy odtwarzanie sampla siê
			zakoñczy³o (odnosi siê tylko do sytuacji gdy
			w³±czone s± operacje nieblokuj±ce).
		**/
		void samplePlayed(SoundDevice device);
		/**
			Sygna³ emitowany gdy nagrywanie sampla siê
			zakoñczy³o (odnosi siê tylko do sytuacji gdy
			w³±czone s± operacje nieblokuj±ce).
		**/
		void sampleRecorded(SoundDevice device);
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
		void playSampleImpl(SoundDevice device, const int16_t* data, int length, bool& result);
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
		void recordSampleImpl(SoundDevice device, int16_t* data, int length, bool& result);		
};

extern SoundManager* sound_manager;

#endif
