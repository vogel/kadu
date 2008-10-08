#ifndef FILTERING_H
#define FILTERING_H

#include <QEvent>
#include <QWidget>

#include "configuration_aware_object.h"
#include "usergroup.h"

class QPushButton;
class QLineEdit;

class Filtering : public QWidget, ConfigurationAwareObject 
{
	Q_OBJECT

	void clearFilter ();
	void hideFilter ();
	void filterWith (const QString& f);
	bool checkString (const QString& hay, const QString& needle, bool startsWith);

	QPushButton *clearPB;
	QLineEdit *textLE;

	UserGroup *filter;

	void createDefaultConfiguration();
	
private slots:
	void on_clearPB_clicked ();
	void on_textLE_textChanged (const QString& s);
	void on_textLE_returnPressed ();
	bool on_kadu_keyPressed(QKeyEvent*);

protected:
	virtual void keyPressEvent (QKeyEvent *e);
	bool eventFilter (QObject *o, QEvent *e);

	virtual void configurationUpdated();

public:
	Filtering();
	virtual ~Filtering();

};

extern Filtering *filtering;

#endif
