#include <user_io_base.h>
#include <graphics_io>

class User_Io : public User_Io_Base {
public:
	User_Io();
	~User_Io();
	bool Get_Escape();
};