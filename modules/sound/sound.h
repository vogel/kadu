#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qthread.h>
#include <qmutex.h>
#include <qsemaphore.h>

#include "../notify/notify.h"

#include "config_file.h"
#include "gadu.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "message_box.h"
#include "modules.h"
#include "themes.h"
#include "usergroup.h"
#include "userlist.h"

/**
 * @defgroup sound Sound
 * The sound module.
 * @{
 */

/**
	Uogólniony deskryptor urz±dzenia d¼wiêkowego.
**/
typedef void* SoundDevice;

/**
**/
enum SoundDeviceType {RECORD_ONLY, PLAY_ONLY, PLAY_AND_RECORD};

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SamplePlayThread : public QObject, public QThread
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
		SamplePlayThread(SoundDevice device);
		void playSample(const int16_t* data, int length);
		void stop();

	signals:
		void samplePlayed(SoundDevice device);
};

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SampleRecordThread : public QObject, public QThread
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
		SampleRecordThread(SoundDevice device);
		void recordSample(int16_t* data, int length);
		void stop();

	signals:
		void sampleRecorded(SoundDevice device);
};

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SndParams
{
	public:
		SndParams(QString fm = QString::null, bool volCntrl = false, float vol = 1);
		SndParams(const SndParams &p);

		QString filename;
		bool volumeControl;
		float volume;
};

/**
	To jest klasa u¿ywana wewnêtrznie przez klasê SoundManager
	i nie powiniene¶ mieæ potrzeby jej u¿ywania.
**/
class SoundPlayThread : public QThread
{
	public:
		SoundPlayThread();
		~SoundPlayThread();
		void run();
		void tryPlay(const char *path, bool volCntrl=false, float volume=1.0);
		void endThread();

	private:
		static bool play(const char *path, bool volCntrl=false, float volume=1.0);
		QMutex mutex;
		QSemaphore *semaphore;
		bool end;
		QValueList<SndParams> list;
};

class SoundManager : public Notifier, public ConfigurationUiHandler
{
    Q_OBJECT
	private:
		Themes *themes;
		friend class SamplePlayThread;
		friend class SampleRecordThread;
		QTime lastsoundtime;
		bool mute;
		QMap<SoundDevice, SamplePlayThread*> PlayingThreads;
		QMap<SoundDevice, SampleRecordThread*> RecordingThreads;
		SoundPlayThread *play_thread;

		int simple_player_count;
		virtual void connectNotify(const char *signal);
		virtual void disconnectNotify(const char *signal);

		void playSound(const QString &soundName);

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

		void import_0_5_0_configuration();

	public slots:
		void play(const QString &path, bool force=false);
		void play(const QString &path, bool volCntrl, double vol);
		void setMute(const bool& enable);

	public:

		SoundManager(const QString& name, const QString& configname);
		~SoundManager();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		virtual void notify(Notification *notification);

		Themes *theme();

		bool isMuted() const;
		int timeAfterLastSound() const;
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
			@param type
			@param sample_rate sample rate - np. 8000 lub 48000
			@param channels ilo¶æ kana³ów: 1 - mono, 2 - stereo
			@return uogólniony deskryptor urz±dzenia lub NULL je¶li otwarcie siê nie powiod³o.
		**/
		SoundDevice openDevice(SoundDeviceType type, int sample_rate, int channels = 1);
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
			Standardowo po przekazaniu odtwarzanego sampla
			do sterownika d¼wiêkowego program (w trybie blokuj±cym)
			lub w±tek odtwarzaj±cy (w trybie nieblokuj±cym) czeka
			na zakoñczenie emitowania d¼wiêku przez sterownik, aby
			zagwarantowaæ, ¿e wyj¶cie z funkcji playSample() (w trybie
			blokuj±cym) lub wyemitowanie sygna³u samplePlayed() (w
			trybie nieblokuj±cym) nast±pi dopiero po fizycznym
			odegraniu d¼wiêku. Inaczej mog³o by to nast±piæ dopiero
			przy wywo³aniu metody closeDevice() co ma ró¿ne nieprzyjemne
			efekty uboczne.
			Czasem przy odtwarzaniu kilku ma³ych sampli jeden po drugim
			powoduje to powstanie przerw miêdzy nimi. Aby tego unikn±æ
			mo¿emy u¿yæ setFlushingEnabled(device, false) umo¿liwiaj±c
			w ten sposób p³ynne odtwarzanie kilku sampli bezpo¶rednio
			po sobie.
		**/
		void setFlushingEnabled(SoundDevice device, bool enabled);
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
			@param data wska¼nik do danych sampla
			@param length d³ugo¶æ danych sampla (w bajtach)
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
			@param data wska¼nik na bufor dla danych sampla
			@param length d³ugo¶æ sampla do nagrania (wielko¶æ bufora w bajtach)
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
			@param type
			@param sample_rate sample rate - np. 8000 lub 48000
			@param channels ilo¶æ kana³ów: 1 - mono, 2 - stereo
			@device zwrócony uogólniony deskryptor urz±dzenia lub NULL je¶li otwarcie siê nie powiod³o.
		**/
		void openDeviceImpl(SoundDeviceType type, int sample_rate, int channels, SoundDevice& device);
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
			@param data wska¼nik do danych sampla
			@param length d³ugo¶æ danych sampla (w bajtach)
			@param result zwrócony rezultat operacji - true je¶li odtwarzanie zakoñczy³o siê powodzeniem.
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
			@param data wska¼nik na bufor dla danych sampla
			@param length d³ugo¶æ sampla do nagrania (wielko¶æ bufora w bajtach)
			@param result zwrócony rezultat operacji - true je¶li nagrywanie zakoñczy³o siê powodzeniem.
		**/
		void recordSampleImpl(SoundDevice device, int16_t* data, int length, bool& result);
		/**
		**/
		void setFlushingEnabledImpl(SoundDevice device, bool enabled);
};

extern SoundManager* sound_manager;
/** @} */

#endif
