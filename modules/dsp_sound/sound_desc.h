
class sound_desc
{
	public:
	int length;
	char *data;
	int channels;
	int speed;
	int unit;
	
	sound_desc(const char *path);
	~sound_desc();
	bool isOk();
};
