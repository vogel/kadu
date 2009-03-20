#include "choose-description.h"

ChooseDescription *ChooseDescription::Dialog = 0;

void ChooseDescription::show(const Status &status, const QPoint &position)
{
	if (!Dialog)
	{
		Dialog = new ChooseDescription(kadu);
		Dialog->setPosition(position);
	}

	Dialog->setStatus(status);
	((QDialog *)Dialog)->show();
	Dialog->raise();
}

ChooseDescription::ChooseDescription(QWidget *parent)
	: QDialog(parent, false)
{
	kdebugf();
	setWindowTitle(tr("Select description"));
	setAttribute(Qt::WA_DeleteOnClose);

// TODO: 0.6.6
// 	while (defaultdescriptions.count() > config_file.readNumEntry("General", "NumberOfDescriptions"))
// 		defaultdescriptions.pop_back();

  	Description = new QComboBox(this);
	Description->setMaxVisibleItems(30);
// 	Description->insertStringList(defaultdescriptions);

	QLineEdit *ss = new QLineEdit(this);
#if 1
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	ss->setMaxLength(gadu->maxDescriptionLength());
#endif
	Description->setLineEdit(ss);

	AvailableChars = new QLabel(this);

	updateAvailableChars(Description->currentText());

	connect(Description, SIGNAL(textChanged(const QString &)), this, SLOT(updateAvailableChars(const QString &)));

	OkButton = new QPushButton(tr("&OK"), this);
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);

	connect(OkButton, SIGNAL(clicked()), this, SLOT(okPressed()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

	QGridLayout *grid = new QGridLayout(this);

	grid->addWidget(Description, 0, 0, 1, -1);
	grid->addWidget(AvailableChars, 1, 0);
	grid->addWidget(OkButton, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelButton, 1, 2, Qt::AlignRight);

	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
}

void ChooseDescription::setStatus(const Status &status)
{
	CurrentStatus = status;

	switch (CurrentStatus.type())
	{
		case Status::Online:
			OkButton->setIcon(icons_manager->loadIcon("OnlineWithDescription"));
			break;
		case Status::Busy:
			OkButton->setIcon(icons_manager->loadIcon("BusyWithDescription"));
			break;
		case Status::Invisible:
			OkButton->setIcon(icons_manager->loadIcon("InvisibleWithDescription"));
			break;
		case Status::Offline:
			OkButton->setIcon(icons_manager->loadIcon("OfflineWithDescription"));
			break;
		default:
			break;
	}
}

void ChooseDescription::setPosition(const QPoint &position)
{
	int width = 250;
	int height = 80;

	QSize sh = sizeHint();

	if (sh.width() > width)
		width = sh.width();

	if (sh.height() > height)
		height = sh.height();

	QDesktopWidget *d = QApplication::desktop();

	QPoint p = position;
	if (p.x() + width + 20 >= d->width())
		p.setX(d->width() - width - 20);
	if (p.y() + height + 20 >= d->height())
		p.setY(d->height() - height - 20);
	move(p);

	resize(width, height);
}

void ChooseDescription::okPressed()
{
	QString description = Description->currentText();
	Account *account = AccountManager::instance()->defaultAccount();
// TODO: 0.6.6
	//je�eli ju� by� taki opis, to go usuwamy
// 	defaultdescriptions.remove(description);
	//i dodajemy na pocz�tek
// 	defaultdescriptions.prepend(description);

// 	while (defaultdescriptions.count() > config_file.readNumEntry("General", "NumberOfDescriptions"))
// 		defaultdescriptions.pop_back();

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = KaduParser::parse(description, account, Core::instance()->myself(), true);

	CurrentStatus.setDescription(description);
	Core::instance()->setStatus(CurrentStatus);

	cancelPressed();
}

void ChooseDescription::cancelPressed()
{
	Dialog = 0;
	close();
}

void ChooseDescription::updateAvailableChars(const QString &text)
{
	int length = text.length();

#if 0
	int count = (length - 10) / (gadu->maxDescriptionLength() - 10);
	int rest = (count + 1) * (gadu->maxDescriptionLength() - 10) - length + 10;

	AvailableChars->setText(' ' + QString::number(rest) + " (" + QString::number(count) + ")");
#else
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	AvailableChars->setText(' ' + QString::number(gadu->maxDescriptionLength() - length));
#endif
}
