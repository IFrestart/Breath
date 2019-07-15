#include <iostream>
#include <fstream>
using namespace std;
const char* filename ="2016_7_24_22_29_A1.nhStates";

int main ()
{
	char * buffer;
	long size;
	ifstream file (filename,   ios::in|ios::binary|ios::ate);
	size = file.tellg();
	file.seekg (0, ios::beg);
	buffer = new char [size];
	file.read (buffer, size);
	file.close();
	cout << "the complete file is in a buffer" << endl;

	for (int i=0; i < size; i++)
	{
		int idata1 = (0xff & buffer[i]) << 24 | (0xff & buffer[++i]) << 16| (0xff & buffer[++i]) << 8 | 0xff & buffer[++i];
		int idata2 = (0xff & buffer[++i]) << 24 | (0xff & buffer[++i]) << 16| (0xff & buffer[++i]) << 8 | 0xff & buffer[++i];
		int idata3 = (0xff & buffer[++i]) << 24 | (0xff & buffer[++i]) << 16| (0xff & buffer[++i]) << 8 | 0xff & buffer[++i];
		int idata4 = (0xff & buffer[++i]) << 24 | (0xff & buffer[++i]) << 16| (0xff & buffer[++i]) << 8 | 0xff & buffer[++i];
		cout << "time=" << idata1 << ", 呼吸频率:" << idata2 << ", AHI*10:" << idata3 << ", 呼吸状态:" << idata4 << endl;
	}
	cout << "size:" << size << endl;
	delete[] buffer;
	return 0;
}

