// ---------------------------------------------------------------------------

#ifndef unTUtilsH
#define unTUtilsH

// ---------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstddef>
#include<IniFiles.hpp>
//#include "workWithX502.h"
// #include "UnDataMod.h"
//#include <System.Classes.hpp>
 #include <VCLTee.Chart.hpp>
 #include <Vcl.Controls.hpp>
#include <vector>
#include <iterator>
#include "TLCardData.h"
using namespace std;
 //����� ����� ��������������� � ��������� ������� - �����������
// ����� ��� ������ � ������� � ������  � ����
class TLog {
public:
	TLog();
	~TLog(void);
	//������ ������ � ���������������� ��� ������
	//���� � ����������\LOGs\errLog_" + ������� ���_�����_���� + ".log";
	static int SaveStrMsgLog(AnsiString _Msg);
	//������ � ��������� ���� � ��������� ������� ����
	static int SaveStrTxtFile(AnsiString _fullNameFile,AnsiString _Msg);
	//������ ����� ������ � ��������� ���� � ��������� ������� ����
    static int SaveTxtArrDoubleFile(AnsiString _fullFileName,AnsiString _str,double *_arr,int _arrSize);
	//�������� ������ � ������� � ���������������� ��� ������
	static int ErrFullSaveLog(Exception *_ex);
	//������ � ��������� ����
	static int SaveTxtVectDoubleFile(AnsiString _fullFileName,   vector<double>& _vectorData , int _vectSize);
	//������ � �������� ���� � ������� � ������������� �� ���������� �������
	static int SaveChTxtDoubleFile(AnsiString _fullFileName, double *_data,int _dataSize,int _countCh);
	static int Save123TxtVectDoubleFile(AnsiString _fullFileName, vector<double> &_vectorData);
	static int SaveNEWTxtVectDoubleFile(AnsiString _fullFileName, vector<double> &_vectorData, int _vectSize);
	static int LoadTxtChDoubleFile(AnsiString _fullFileName, TLCardData *_lCardData, int _frec,int _countCh);
	// ��������� �� ��������� � �������
	int LoadTubeFromFileBin(AnsiString _fullFileName,int _countZonesAll,int _wdFirstSensorsThick,int _wdCountSensorsThick,
	  int _wdFirstSensorsCross,int _wdCountSensorsCross,int _wdFullSensorSize,int _wdCountLOGchX502);
private:
	ofstream Log;
};

// ��� ���������� � �������� �������� �� INI
//�� ������������, ���������
class TSettingsINI {
public:

	TSettingsINI();
	~TSettingsINI();
	bool loadSettingsINI();
	bool saveSettingsINI();

private:
	AnsiString applPath;
	TIniFile *Ini;
};

#endif
