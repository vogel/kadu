#ifndef SEARCH_H
#define SEARCH_H

#include <qradiobutton.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qvaluelist.h>

#include "gadu.h"

/**
	Dialog umo¿liwiaj±cy wyszukiwanie w katalogu publicznym
**/
class SearchDialog : public QDialog {
	Q_OBJECT
	public:
		SearchDialog(QWidget *parent=0, const char *name=0, uin_t whoisSearchUin = 0);
		~SearchDialog(void);

	private:
		QCheckBox *only_active;
		QLineEdit *e_uin;
		QLineEdit *e_name;
		QLineEdit *e_nick;
		QLineEdit *e_byrFrom;
		QLineEdit *e_byrTo;
		QLineEdit *e_surname;
		QComboBox *c_gender;
		QLineEdit *e_city;
		QListView *results;
		QLabel *progress;
		QRadioButton *r_uin;
		QRadioButton *r_pers;
		QPushButton *b_chat;
		QPushButton *b_sendbtn;
		QPushButton *b_nextbtn;
		QPushButton *b_addbtn;
		uin_t _whoisSearchUin;
		uint32_t seq;

		SearchRecord *searchRecord;

		bool searchhidden;

	private slots:
		void clearResults(void);
		void prepareMessage(QListViewItem*);
		void uinTyped(void);
		void personalDataTyped(void);
		void AddButtonClicked();
		void updateInfoClicked();
		void openChat();

	public slots:
		void firstSearch(void);
		void nextSearch(void);
		void newSearchResults(SearchResults& searchResults, int seq, int fromUin);
		void selectionChanged(QListViewItem *);

	protected:
		void closeEvent(QCloseEvent * e);

};

#endif

