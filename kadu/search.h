#ifndef KADU_SEARCH_H
#define KADU_SEARCH_H

#include <qdialog.h>
#include <qvaluelist.h>

#include "gadu.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;
class QRadioButton;

/**
	Dialog umo¿liwiaj±cy wyszukiwanie w katalogu publicznym
**/
class SearchDialog : public QDialog {
	Q_OBJECT
	public:
		SearchDialog(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0);
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
		UinType _whoisSearchUin;
		uint32_t seq;

		SearchRecord *searchRecord;

		bool searchhidden;

	public slots:
		void firstSearch(void);
		void nextSearch(void);
		void newSearchResults(SearchResults& searchResults, int seq, int fromUin);
		void selectionChanged(QListViewItem *);

	private slots:
		void clearResults(void);
		void prepareMessage(QListViewItem *);
		void uinTyped(void);
		void personalDataTyped(void);
		void AddButtonClicked();
		void updateInfoClicked();
		void openChat();

	protected:
		void closeEvent(QCloseEvent * e);
};

#endif

