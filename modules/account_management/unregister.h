#ifndef UNREGISTER_H
#define UNREGISTER_H

#include <QWidget>

class QLineEdit;

/** @ingroup account_management
 * @{
 */
class Unregister : public QWidget 
{
	Q_OBJECT

		QLineEdit *uin;
		QLineEdit *pwd;

		void deleteConfig();

	private slots:
		void doUnregister();
		void keyPressEvent(QKeyEvent *);

	public:
		Unregister(QDialog* parent = 0);
		~Unregister();
	
	public slots:
		void unregistered(bool ok);

};

/** @} */

#endif
