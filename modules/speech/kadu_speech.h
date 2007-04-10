#ifndef KADU_SPEAK_H
#define KADU_SPEAK_H
#include <qobject.h>
#include <qstring.h>

#include "misc.h"
#include "protocol.h"
#include "userlist.h"
#include "../notify/notify.h"

/**
 * @defgroup speech Speech
 * @{
 */
class SpeechSlots : public Notifier
{
	Q_OBJECT
	private:
		QTime lastSpeech;

		void import_0_5_0_Configuration();

	public:
		SpeechSlots(QObject *parent=0, const char *name=0);
		~SpeechSlots();

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

	private slots:
		void newChat(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);
		void newMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
		virtual void externalEvent(Notification *notification);

		void say(const QString &s, const QString &path=QString::null,
					bool klatt=false, bool melodie=false,
					bool arts=false, bool esd=false, bool dsp=false, const QString &device=QString::null,
					int freq=0, int tempo=0, int basefreq=0);

		void onCreateTabSpeech();
		void chooseSpeechProgram();
		void useArts();
		void useEsd();
		void useDsp();
		void testSpeech();
};

extern SpeechSlots *speech_slots;

/** @} */

#endif
