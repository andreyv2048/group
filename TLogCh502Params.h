#ifndef TLogCh502ParamsH
#define TLogCh502ParamsH

#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
//#include <IniFiles.hpp>
#include <vector>
//---------------------------------------------------------------------------
//��������� ������ ����������� ������
//������ RChannel_parameters
class TLogCh502Params
{
private:
	//TIniFile* ini;
	//AnsiString sect;
public:
	// ����������� ��������� ������ ����������� ������
	TLogCh502Params();
	// ����������
	~TLogCh502Params(void);
	// ! @brief ������� ��������
	// ! @li 0 �������� +/-10V
	// ! @li 1 �������� +/-5V
	// ! @li 2 �������� +/-2.5V
	// ! @li 3 �������� +/-1.25V
	// ! @li 4 �������� +/-0.625V
	// ! @li 5 �������� +/-0.3125V
	//unsigned int adcRange;
	int adcRangeIndex;
	// ! ����� ����������� ������ 0-31
	unsigned int logicalChannel;
//	unsigned int fizChannel;
	// ! @brief ����� �����
	// ! @li 0 ��������� ���������� ������������ ����� �����
	// ! @li 1 ���������������� ��������� ����������
	// ! @li 2 ��������� ������������ ����
	unsigned int collectedMode;
	double chGain;
	AnsiString chName;
	AnsiString ToString(void);
};
//---------------------------------------------------------------------------
#endif
