#include <basetsd.h>
class Graphics_Io_Base {
public:
	virtual void Update_Screen() = 0;
	virtual void Send_Voxels(__int8* Voxels) = 0;

};