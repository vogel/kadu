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
		SpeechSlots(QObject *parent=0, const char *name=0);
		~SpeechSlots();
	private slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToInvisible(const UserListElement &ule);
		void userChangedStatusToNotAvailable(const UserListElement &ule);
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);

		void say(const QString &s, const QString &path=QString::null,
					bool klatt=false, bool melodie=false,
					bool arts=false, bool esd=false, bool dsp=false, const QString &device=QString::null,
					int freq=0, int tempo=0, int basefreq=0);

		void onCreateConfigDialog();
		void chooseSpeechProgram();
		void useArts();
		void useEsd();
		void useDsp();
		void testSpeech();
};

extern SpeechSlots *speech_slots;

#endif
