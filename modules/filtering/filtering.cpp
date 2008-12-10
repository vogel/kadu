
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "userbox.h"
#include "userlist.h"

#include "filtering.h"
#ifdef Q_OS_MAC
#include "searchbox_mac.h"
#endif

extern "C" KADU_EXPORT int filtering_init(bool firstLoad)
{
	kdebugf();
	
	filtering = new Filtering ();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/filtering.ui"), 0);
	
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void filtering_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/filtering.ui"), 0);
	
	delete filtering;
	filtering = NULL;
	
	kdebugf2();
}

const char * LABELTEXT = "Filtering, by default, uses contact's first name, last name,\nnick name and alternative display nick name.\nThis is done by using 'contains' method.";

Filtering::Filtering(): QWidget(kadu)
{
	kdebugf();
	
	kadu->mainLayout ()->insertWidget (0, this);

	filter = new UserGroup(userlist->toUserListElements());
	
	QHBoxLayout *l = new QHBoxLayout(this, 0, 5);

#ifdef Q_OS_MAC
	setContentsMargins(0, 2, 0, 0);
	
	search = new QMacSearchBox(this);
	search->setMinimumHeight(24);
	l->addWidget(search);

	connect(search, SIGNAL(textChanged(const QString&)), this, SLOT(on_textLE_textChanged(const QString&)));
#else
	l->addWidget (clearPB = new QPushButton(this, "clearPB"));
	clearPB->setPixmap (QPixmap (dataPath ("kadu/modules/data/filtering/clear.png")));
	l->addWidget (new QLabel (tr("Filter") + ":", this, "filterLBL"));
	l->addWidget (textLE = new QLineEdit(this, "textLE"));
	
	connect(clearPB, SIGNAL(clicked()), this, SLOT(on_clearPB_clicked()));
	connect(textLE, SIGNAL(textChanged(const QString&)), this, SLOT(on_textLE_textChanged(const QString&)));
	connect(textLE, SIGNAL(returnPressed()), this, SLOT(on_textLE_returnPressed()));
#endif
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(on_kadu_keyPressed(QKeyEvent*)));
	kadu->userbox ()->installEventFilter (this);

	createDefaultConfiguration();
	configurationUpdated();
	
	kdebugf2();
}
	
Filtering::~Filtering()
{
	kdebugf();
#ifdef Q_OS_MAC
	kadu->userbox()->removeFilter(filter);
	disconnect(search, SIGNAL(textChanged(const QString&)), this, SLOT(on_textLE_textChanged(const QString&)));
#else
	if (! textLE->text ().isEmpty ()) {
		kadu->userbox ()->removeFilter (filter);
	}
	
	disconnect(clearPB, SIGNAL(clicked()), this, SLOT(on_clearPB_clicked()));
	disconnect(textLE, SIGNAL(textChanged(const QString&)), this, SLOT(on_textLE_textChanged(const QString&)));
#endif
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(on_kadu_keyPressed(QKeyEvent*)));

	kadu->userbox ()->removeEventFilter (this);

	kdebugf2();
}

bool Filtering::on_kadu_keyPressed (QKeyEvent *e)
{
//	kdebugf();

	QString text = e->text();
	kdebugm (KDEBUG_INFO, QString("text=[%1] key=%2\n").arg (e->text ()).arg (e->key ()).local8Bit ());
	
	bool startonany = config_file.readBoolEntry ("filtering", "filter-startonany", false);
	bool shortcut = HotKey::shortCut (e, "ShortCuts", "filtering_start");

	if (shortcut || (startonany && (text.ref (0).isPrint())) && kadu->userbox()->count ()) {
		show ();
#ifdef Q_OS_MAC
		search->activate();
#else
		textLE->setFocus();
#endif
		if (startonany && (! shortcut)) {
#ifdef Q_OS_MAC
			if (search->text().isEmpty())
				search->setText(text);
#else
			textLE->setText(text);
#endif
		}

	    return true;
	} else if (e->key() == Qt::Key_Escape && isVisible()) {
	    hideFilter();
	    return true;
	} else {
	    return false;
	}
//	kdebugf2();
}

bool Filtering::eventFilter (QObject *o, QEvent *e)
{
//	kdebugf();
	
	if (e->type () == QEvent::KeyPress) {
		return on_kadu_keyPressed ((QKeyEvent*) e);
	} else {
		return false;
	}
	
//	kdebugf2();
}

void Filtering::keyPressEvent (QKeyEvent *e)
{
	kdebugf();
	
	if (e->key() == Qt::Key_Escape) {
		hideFilter();
		e->accept ();
	} else if (e->key() == Qt::Key_Down) {
		on_textLE_returnPressed();
		e->accept ();
	} else {
		e->ignore ();
	}
	
	kdebugf2();
}

void Filtering::hideFilter ()
{
    hide ();
#ifdef Q_OS_MAC
	search->setText(QString::null);
#else
    textLE->setText(QString::null);
#endif
    kadu->userbox()->setFocus();
}

void Filtering::on_clearPB_clicked ()
{
	kdebugf();
#ifndef Q_OS_MAC
	textLE->clear();
#endif
	kdebugf2();
}

void Filtering::on_textLE_textChanged (const QString& s)
{
	kdebugf();
	
	clearFilter();
	
	if (!s.isEmpty())
		filterWith (s);
	
	kdebugf2();
}

void Filtering::clearFilter ()
{
	kdebugf();
	
	kadu->userbox()->removeFilter (filter);
	filter->removeUsers (userlist);
	
	kdebugf2();
}

void Filtering::filterWith (const QString& f)
{
	kdebugf();
	
	bool filter_number = config_file.readBoolEntry("filtering", "filter-number", false);
	bool filter_email = config_file.readBoolEntry("filtering", "filter-email", false);
	bool filter_mobile = config_file.readBoolEntry("filtering", "filter-mobile", false);
	bool filter_startswith = (config_file.readNumEntry("filtering", "filter-startswith", 1) == 1);
	
	foreach(const UserListElement &u, userlist->toUserListElements()) {
		if (checkString(u.firstName (), f, filter_startswith)
				|| checkString(u.lastName (), f, filter_startswith)
				|| checkString(u.altNick (), f, filter_startswith)
				|| checkString (u.nickName (), f, filter_startswith)
				|| (filter_number && u.usesProtocol ("Gadu") && checkString (u.ID ("Gadu"), f, filter_startswith))
				|| (filter_email && checkString (u.email (), f, filter_startswith))
				|| (filter_mobile && checkString (u.mobile (), f, filter_startswith))) {
			filter->addUser (u);
		}
	}
	
	kadu->userbox ()->applyFilter (filter);
	
	kdebugf2();
}

bool Filtering::checkString (const QString &hay, const QString& needle, bool startsWith)
{
	if (startsWith)
		return hay.startsWith(needle, Qt::CaseInsensitive);
	else
		return hay.contains(needle, Qt::CaseInsensitive);
}

void Filtering::on_textLE_returnPressed ()
{
	if (kadu->userbox ()->count () == 0)
		return;
	
	kadu->userbox ()->setFocus ();
	kadu->userbox ()->clearSelection ();
	kadu->userbox ()->setSelected (0, true);
	kadu->userbox ()->setCurrentItem (0);
}

void Filtering::createDefaultConfiguration()
{
	config_file.addVariable("filtering", "filter-startonany", true);
	config_file.addVariable("filtering", "filter-showonload", false);
	config_file.addVariable("filtering", "filter-number", true);
	config_file.addVariable("filtering", "filter-email", true);
	config_file.addVariable("filtering", "filter-mobile", true);
	config_file.addVariable("filtering", "filter-startswith", false);
	config_file.addVariable("ShortCuts", "filtering_start", "/");
}

void Filtering::configurationUpdated()
{
	setShown(config_file.readBoolEntry("filtering", "filter-showonload"));
}

Filtering *filtering = 0;

