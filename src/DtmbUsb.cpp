#include "pch.h"
#include "DtmbUsb.h"
#include <string>
GlobalClass gc;
#define MAX_BUF_SIZE 0xfc00
DtmbUsb::DtmbUsb(short port)
{

	gCCyUSBDevice = &USBDevice;
	sockServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int nRecvBuf = MAX_BUF_SIZE;
	setsockopt(sockServer, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	addr_Server.sin_family = AF_INET;
	addr_Server.sin_port = htons(port);
	addr_Server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	isExit = false;

}

DtmbUsb::~DtmbUsb()
{
	gCCyUSBDevice->Close();

	if (sockServer != INVALID_SOCKET) closesocket(sockServer);
}

bool DtmbUsb::initOpen()
{
	bool bret = false;
	int devices = gCCyUSBDevice->DeviceCount();
	int usbindex = 0;

	do
	{
		if (!devices)
		{
			break;
		}
		while (gCCyUSBDevice->VendorID != 1204)
		{
			if (++usbindex >= devices) {
				break;
			}
		}
		if (!gCCyUSBDevice->Open(usbindex)) {
			gCCyUSBDevice->Reset();
			gCCyUSBDevice->Open(usbindex);
			if (!gCCyUSBDevice->IsOpen()) {
				break;
			}
		}
		int ends = gCCyUSBDevice->EndPointCount();
		//CCyUSBEndPoint* ptrEndPoint=nullptr;
		int i = 0;
		for (CCyUSBEndPoint** Points = gCCyUSBDevice->EndPoints; i < ends; ++Points) {
			if ((*Points)->Address == 0x82) {
				gCCyUSBEndPoint = *Points;
				break;
			}
			++i;
		}
		//if (!gCCyUSBEndPoint) {
		//	printf("ptrControlEndPoint null\n");
		//	//ptrControlEndPoint = ptrUSBDevice->ControlEndPt;
		//}
		gControlEndPt = gCCyUSBDevice->ControlEndPt;
		bret = true;
		//gControlEndPt->SetXferSize(0xFC00u);
		//gCCyUSBEndPoint_410B44->SetXferSize(0xFC00u);
		getVersion();
	} while (false);
	return bret;
}

void DtmbUsb::getVersion()
{
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = 0xED;
	gControlEndPt->Value = 0xFE;
	unsigned char outdata[4] = { 0 };
	LONG outsize = 4;
	gControlEndPt->Read(outdata, outsize);

	CString cs;
	cs.Format(_T("获取到固件的版本号为 V%d.%d.%d%d\n"),
		outdata[0],
		outdata[1],
		outdata[2],
		outdata[3]);
	dlg->log(cs);
}
#define BYTEN(w,n) ((BYTE)((((DWORD_PTR)(w)) >> 8*n) & 0xff))
#define BYTE1(w) ((BYTE)((((DWORD_PTR)(w)) >> 8*1) & 0xff))
#define BYTE2(w) ((BYTE)((((DWORD_PTR)(w)) >> 8*2) & 0xff))
#define BYTE3(w) ((BYTE)((((DWORD_PTR)(w)) >> 8*3) & 0xff))

bool DtmbUsb::setPort(short port)
{
	std::lock_guard<std::mutex> lock(mx);
	addr_Server.sin_port = htons(port);
	return true;
}

bool DtmbUsb::setIp(DWORD dwip)
{
	std::lock_guard<std::mutex> lock(mx);
	addr_Server.sin_addr.S_un.S_addr = dwip;
	return true;
}

std::vector<int> DtmbUsb::getAvailableRates(std::vector<int>& u_rates, std::vector<int>& u_strenth, std::vector<float>& u_quality)
{
	//std::lock_guard<std::mutex> lock(mx);
	setExit(false);
	std::vector<int> rs;
	CString cs;
	dlg->m_ProGressCtrl.SetRange(0, u_rates.size() + 1);
	for (int i = 0; i < u_rates.size(); ++i) {
		int x = u_rates[i];
		dlg->m_ProGressCtrl.SetPos(i);
		cs.Format(_T("[-]rate:%d\n"), x);
		dlg->log(cs);
		int s = 0;
		float q = 0;
		if (setRate(x)) {
			cs.Format(_T("[+]rate success:%d\n"), x);
			dlg->log(cs);
			if (!getQuality(s, q)) {
				cs.Format(_T("[!]%d:getQuality error\n", x));
				dlg->log(cs);
			}
			rs.push_back(x);
			u_strenth.push_back(s);
			u_quality.push_back(q);
		}
		if (isExit) {
			break;
		}
	}
	cs.Format(_T("[+]getRates ov\n"));
	dlg->log(cs);
	return rs;
}







//true:success false:faild
bool DtmbUsb::setRate(int rate)
{
	LONG outsize; // [esp+4h] [ebp-14h] BYREF
	UCHAR outdata[4]; // [esp+Ch] [ebp-Ch] BYREF

	outdata[0] = BYTE3(rate);
	outdata[1] = BYTE2(rate);
	outdata[2] = BYTE1(rate);
	outdata[3] = rate & 0xff;
	outsize = 4;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = 0xFC;
	gControlEndPt->Value = 254;
	if (!gControlEndPt->Write(outdata, outsize)) return false;
	outsize = 1;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = 0xEA;
	gControlEndPt->Value = 254;
	UCHAR isok = 0;
	if (!gControlEndPt->Read(&isok, outsize)) return false;
	return isok == 1;

}


bool DtmbUsb::getQuality(int& strength, float& quality)
{

	CString cs;
	LONG outsize; // [esp+10h] [ebp-Ch] BYREF
	UCHAR outdata[4] = { 0 }; // [esp+14h] [ebp-8h] BYREF

	outsize = 1;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = -20;
	gControlEndPt->Value = 254;
	if (!gControlEndPt->CCyControlEndPoint::Read(outdata, outsize))
		return 1;

	outsize = 2;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = -24;
	gControlEndPt->Value = 254;
	if (!gControlEndPt->CCyControlEndPoint::Read(outdata, outsize))
		return 1;


	double a1 = (double)outdata[0] * 0.01 + (double)outdata[0];


	outsize = 4;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = -21;
	gControlEndPt->Value = 254;
	if (!gControlEndPt->CCyControlEndPoint::Read(outdata, outsize))
		return false;

	strength = outdata[3] + ((outdata[2] + ((outdata[1] + (outdata[0] << 8)) << 8)) << 8);
	cs.Format(_T("[-]strength:%d%%"), strength);
	dlg->log(cs);

	outsize = 3;
	gControlEndPt->Target = TGT_DEVICE;
	gControlEndPt->ReqType = REQ_VENDOR;
	gControlEndPt->ReqCode = -23;
	gControlEndPt->Value = 254;
	if (!gControlEndPt->CCyControlEndPoint::Read(outdata, outsize))
		return false;

	//double quality = (double)outdata[1] * 0.01 + (double)outdata[0] + (double)outdata[2] * 0.0001;
	quality = (float)outdata[1] * 100 + (float)outdata[0] * 10000 + (float)outdata[2];
	if (quality == 0) {
		quality = 100;
	}
	cs.Format(_T("[-]quality:%.2f%%"), quality);
	dlg->log(cs);
	return true;
}

void DtmbUsb::dtmbRead()
{
	setExit(false);
	UCHAR buf[MAX_BUF_SIZE] = { 0 };
	LONG len = MAX_BUF_SIZE;
	if (sockServer == INVALID_SOCKET)
	{
		dlg->log(CString("dtmb_read Failed socket()"));
		return;
	}
	while (!isExit && gCCyUSBEndPoint->XferData(buf, len, 0))
	{
		sendto(sockServer, (const char*)buf, len, 0, (SOCKADDR*)&addr_Server, sizeof(addr_Server));
		len = MAX_BUF_SIZE;
	}
	dlg->log(CString("dtmb_read ERROR OR EXIT"));
	::SendMessage(dlg->GetDlgItem(1016)->GetSafeHwnd(), BM_CLICK, 0, 0);
	//dlg->SendMessage(BM_CLICK)
}
bool DtmbUsb::refreshSignal(int index, int rate)
{
	//std::lock_guard<std::mutex> lock(mx);
	int s = 0;
	float q = 0;
	getQuality(s, q);
	CString css, csq;
	css.Format(_T("%d"), s);
	csq.Format(_T("%.2f%%"), q);
	dlg->m_AvailableRateListCtrl.SetItemText(index, 1, css);
	dlg->m_AvailableRateListCtrl.SetItemText(index, 2, csq);
	return true;

}

const char* C_STRS[] = {
	"C=1",
	"C=3780",0
};
const char* PN_STRS[] = {
	"PN945","PN595","PN420","NULL-3",0
};
const char* N_STRS[] = {
	"0.4","0.6","0.8","NULL-3",0
};
const char* N2_STRS[] = {
	"720","240",0
};
const char* AM_STRS[] = {
	"4QAM-NR","4QAM","16QAM","32QAM","64QAM","NULL-6","NULL-7",0
};
const char* P_STRS[] = {
	"Phase: Variable","Phase: Fixed",0
};
CString DtmbUsb::getSignal()
{
	UCHAR out[6] = { 0 };
	CCyControlEndPoint* ptr = gControlEndPt;
	LONG outsize = 6;
	gControlEndPt->Target = TGT_DEVICE;
	ptr->ReqType = REQ_VENDOR;
	ptr->ReqCode = 0xE7;
	ptr->Value = 254;
	ptr->CCyControlEndPoint::Read(out, outsize);

	CString cs;
	cs.Format(_T("地面实际信号格式：%S  %S  %S  %S  %S  %S\n"),
		C_STRS[out[0]],
		PN_STRS[out[1]],
		N_STRS[out[2]],
		N2_STRS[out[3]],
		AM_STRS[out[4]],
		P_STRS[out[5]]);
	return cs;
	//dlg->log(cs);

}


