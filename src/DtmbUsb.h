#pragma once

#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include "inc/CyAPI.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include "LeDtmbDlg.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"CyAPI.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")
class DtmbUsb
{
public:
	DtmbUsb(short port = 4567);
	~DtmbUsb();
	CLeDtmbDlg* dlg;
	//first func
	bool initOpen();


	void setExit(bool b) { 
		std::lock_guard<std::mutex> lock(mx);
		isExit = b; 
	}
	bool getIsExit() {
		return isExit;
	}
	bool setPort(short port);
	bool setIp(DWORD dwip);
	std::vector<int> getAvailableRates(std::vector<int>& u_rates, std::vector<int>& u_strenth, std::vector<float>& u_quality);
	bool setRate(int rate);
	CString getSignal();
	void dtmbRead();
	bool refreshSignal(int index, int rate);
	bool refreshSignal(int index);


private:
	bool getQuality(int& u_strenth, float& u_quality);

	void getVersion();


private:
	CCyUSBEndPoint* gCCyUSBEndPoint = nullptr;
	CCyControlEndPoint* gControlEndPt = nullptr;
	CCyUSBDevice* gCCyUSBDevice = nullptr;
	bool isExit;
	//bool isBreak;
	SOCKET sockServer = INVALID_SOCKET;
	SOCKADDR_IN addr_Server;
	CCyUSBDevice USBDevice;
	std::mutex mx;

};

class GlobalClass {
public:
	GlobalClass() {
		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
			printf("≥ı ºªØ ß∞‹");
			exit(1);
		}

	}
	~GlobalClass()
	{
		WSACleanup();
	}
};
extern GlobalClass gc;
static int rates[57]= {

	52500000, 60500000, 68500000, 80000000, 88000000, 
	171000000, 179000000, 187000000, 195000000, 
	203000000, 211000000, 219000000, 
	474000000, 482000000, 490000000, 498000000, 
	506000000, 514000000, 522000000, 530000000, 538000000, 546000000, 554000000, 562000000, 
	610000000, 618000000, 626000000, 634000000, 642000000, 650000000, 658000000, 666000000, 674000000, 682000000, 690000000, 698000000, 
	706000000, 714000000, 722000000, 730000000, 738000000, 746000000, 754000000, 762000000, 770000000, 778000000, 786000000, 794000000, 
	802000000, 810000000, 818000000, 826000000, 834000000, 842000000, 850000000, 858000000,866000000
};