#ifndef UNREGISTER_H
#define UNREGISTER_H

#include <qhbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qevent.h>

class LayoutHelper;
class QResizeEvent;
/** @ingroup account_management
 * @{
 */
class Unregister : public QHBox 
{
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = "unregister_user");
		~Unregister();

	private:
		QLineEdit *uin;
		QLineEdit *pwd;
		QLabel *status;
		QCheckBox *updateconfig;
		LayoutHelper *layoutHelper;

		void deleteConfig();

	private slots:
		void doUnregister();
		void keyPressEvent(QKeyEvent *);

	public slots:
		void unregistered(bool ok);

	protected:
		virtual void resizeEvent(QResizeEvent *);
};

/** @} */

#endif
