#ifndef HISTORY_MODULE_H
#define HISTORY_MODULE_H

#include <qobject.h>
#include "usergroup.h"

class Chat;
class UserGroup;

class HistorySlots: public QObject
{
	Q_OBJECT
	public:
		HistorySlots(QObject *parent=0, const char *name=0);
	public slots:
		void onCreateTabHistory();
		void onApplyTabHistory();
		void updateQuoteTimeLabel(int);
};

class HistoryModule : public QObject
{
	Q_OBJECT

	private:
		static HistorySlots* historyslots;

	private slots:
		void historyActionActivated(const UserGroup* users);
		void chatCreated(Chat *chat, time_t time);
		void messageSentAndConfirmed(UserListElements receivers, const QString& message);
		void viewHistory();
		void deleteHistory();
		void userboxMenuPopup();

	public:
		HistoryModule();
		~HistoryModule();
};

extern HistoryModule* history_module;

#endif
