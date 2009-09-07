#include <QtCore/QObject>

struct gadu_resolver_data
{
	int rfd;		/*< Deskryptor do odczytu */
	int wfd;		/*< Deskryptor do zapisu */
};

class QTimer;
class QString;
class QHostAddress;
class QHostInfo;

class GaduResolver : public QObject
{
	Q_OBJECT

private:
	QTimer *timer;
	int id;
	gadu_resolver_data *data;

public:
	GaduResolver();
	~GaduResolver();
	void resolve(const QString &hostname);
	void setData(gadu_resolver_data *);
	gadu_resolver_data *getData();

public slots:
	void resolved(const QHostInfo &host);
	void abort();
};

extern GaduResolver *resolver;

int gadu_resolver_start(int *fd, void **priv_data, const char *hostname);
void gadu_resolver_cleanup(void **priv_data, int force);
