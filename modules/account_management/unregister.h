#ifndef UNREGISTER_H
#define UNREGISTER_H

#include <QWidget>

class LayoutHelper;
class QLineEdit;
class QResizeEvent;
/** @ingroup account_management
 * @{
 */
class Unregister : public QWidget 
{
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = "unregister_user");
		~Unregister();

	private:
		QLineEdit *uin;
		QLineEdit *pwd;
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
