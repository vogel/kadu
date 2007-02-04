#ifndef HISTORY_MODULE_H
#define HISTORY_MODULE_H

#include <qobject.h>

class UserGroup;

class HistoryModule : public QObject
{
	Q_OBJECT

	private:

	private slots:
		void historyActionActivated(const UserGroup* users);

	public:
		HistoryModule();
		~HistoryModule();
};

extern HistoryModule* history_module;

#endif
