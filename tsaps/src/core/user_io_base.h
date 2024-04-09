#include<graphics_io_base.h>
class User_Io_Base {
protected:
	Graphics_Io_Base* Graphics_Io;
public:
	User_Io_Base(Graphics_Io_Base* Graphics_Io);
	virtual bool Get_Escape() = 0;
	//virtual void Send_Data() = 0;

};