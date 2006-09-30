#ifndef NOTIFY_SLOTS_H
#define NOTIFY_SLOTS_H

#include <qobject.h>
#include <qlistbox.h>
//#include <qstringlist.h>
//#include <qstring.h>
#include <qcstring.h>
#include <qvaluelist.h>

/** @ingroup notify
 * @{
 */
class NotifySlots : public QObject
{
	Q_OBJECT
	private:
		QValueList<QCString> disabledControls;

	public slots:
		void onCreateTabNotify();
		void onApplyTabNotify();

		void _Left();
		void _Right();
		void _Left2(QListBoxItem *item);
		void _Right2(QListBoxItem *item);
		void ifNotifyAll(bool toggled);
	public:
		NotifySlots(QObject *parent=0, const char *name=0);
		~NotifySlots();
		void registerDisabledControl(const QCString &name);
		void unregisterDisabledControl(const QCString &name);
};

/** @} */

#endif
