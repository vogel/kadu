#ifndef UNREGISTER_H
#define UNREGISTER_H

#include <qhbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qevent.h>

class Unregister : public QHBox {
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = 0);
		~Unregister();

	private:
		QLineEdit *uin, *pwd;
		QLabel *status;
		QCheckBox *updateconfig;

		void deleteConfig();

	private slots:
		void doUnregister();
		void keyPressEvent(QKeyEvent *);
	
	public slots:
		void unregistered(bool ok);

};

#endif

