#include <iostream>
#include <fstream>
using namespace std;      
const char * filename =   "2016_7_24_22_29_A1.nhBreath";
      
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
          
          cout << "the   complete file is in a buffer" << endl;
	  for (int i=0; i < size; i+=16)
{
	int idata1 = (0xff & buffer[i+0]) << 24 | (0xff & buffer[i+1]) << 16| (0xff & buffer[i+2]) << 8 | 0xff & buffer[i+3];
	int ad = (0xff & buffer[i+4]);
	int adc = (0xff & buffer[i+5]);
	//int idata2 = (0xff & buffer[i+4]) << 24 | (0xff & buffer[i+5]) << 16| (0xff & buffer[i+6]) << 8 | 0xff & buffer[i+7];  
	//int idata3 = (0xff & buffer[i+8]) << 24 | (0xff & buffer[i+9]) << 16| (0xff & buffer[i+10]) << 8 | 0xff & buffer[i+11];
	//int idata4 = (0xff & buffer[i+12]) << 24 | (0xff & buffer[i+13]) << 16| (0xff & buffer[i+14]) << 8 | 0xff & buffer[i+15];
	//cout << "time=" << idata1 << ", 呼吸频率:" << idata2 << ", AHI*10:" << idata3 << ", 呼吸状态:" << idata4 << endl;
	cout << idata1 << ", ad:" << ad << ", adc:" << adc << endl;
}  
	  cout << "size:" << size << endl;	
          delete[] buffer;
          return 0;
      }
