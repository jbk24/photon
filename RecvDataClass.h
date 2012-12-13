#ifndef RECVDATACLASS_H
#define RECVDATACLASS_H

class RecvDataClass
{
public:
	RecvDataClass();
	~RecvDataClass();
	
	int chunk; //GID of chunk associated with data recieve action
	int dataBuffer; //Number of recv data buffer associated with data recieve action

};

#endif // RECVDATACLASS_H
