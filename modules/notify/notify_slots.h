#ifndef NOTIFY_SLOTS_H
#define NOTIFY_SLOTS_H

#include <qobject.h>
#include <qlistbox.h>
#include <qstringlist.h>
#include <qstring.h>

class NotifySlots : public QObject
{
	Q_OBJECT
	private:
		QStringList disabledControls;

	public slots:
		void onCreateConfigDialog();
		void onApplyConfigDialog();

		void _Left();
		void _Right();
		void _Left2(QListBoxItem *item);
		void _Right2(QListBoxItem *item);
		void ifNotifyAll(bool toggled);
	public:
		NotifySlots();
		~NotifySlots();
		void registerDisabledControl(const QString &name);
		void unregisterDisabledControl(const QString &name);
};

#endif
