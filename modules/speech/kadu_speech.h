#ifndef KADU_SPEAK_H
#define KADU_SPEAK_H
#include <qobject.h>
#include <qstring.h>
#include "misc.h"
#include "userlist.h"

class SpeechSlots : public QObject
{
	Q_OBJECT
	private:
		QTime lastSpeech;
	public:
		SpeechSlots();
	private slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseSpeechProgram();
		
		void say(const QString &s, const QString &path=QString::null,
					bool klatt=false, bool melodie=false,
					bool arts=false, bool esd=false, bool dsp=false, const QString &device=QString::null,
					int freq=0, int tempo=0, int basefreq=0);
		
		void chat(UinsList senders, const QString& msg, time_t time, bool& grab);
		void message(UinsList senders, const QString& msg, time_t time);
		void notify(const uin_t uin, const unsigned int oldstatus, const unsigned int status);
		void notify2(UserListElement *);
		
		void useArts(bool);
		void useEsd(bool);
		void useDsp(bool);
		void testSpeech();

};

extern SpeechSlots *slotsObj;

#endif

