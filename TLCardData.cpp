#pragma hdrstop
#include "TLCardData.h"
#include "TProtocol.h"
#include "unSQLDbModule.h"
#include "Filters.h"
#include "unTUtils.h"
#include "unTExtFunction.h"
#ifdef BERLIN_10
//
#else
 #pragma package(smart_init)
#endif


TLCardData* lCardData = NULL;

// ---------------------------------------------------------------------------
TLCardData::TLCardData(TLCard502* _lCard502,int _countFrequencies, int _chLogCount)
{
	dtLcard502=_lCard502;
	//����� ���������� ��������� �� �����
	countMeasures = _countFrequencies;
//	vecMeasuresData.reserve(_countMeasures);
	for (int i = 0; i < _countFrequencies; i++) {
		TMeasuresData* mData = new TMeasuresData(_chLogCount);
		vecMeasuresData.push_back(*mData);
		delete mData;
	}
	LoadSettings();
	freqNum = 0;
}

// ---------------------------------------------------------------------------
TLCardData::~TLCardData(void) {
	for (int i = 0; i < countMeasures; i++) {
		//delete measuresData[i];
	}
	vecMeasuresData.clear();
}
// ---------------------------------------------------------------------------
void TLCardData::LoadSettings(void) {
int err=-99;
	//Parameters.LoadSettings();
//	GVoltPercent.resize(dtLcard502->vecLogChannels.size());
//	int x=GVoltPercent.size();
//	GZone.resize(dtLcard502->vecLogChannels.size());
//  //�������� ������� � ????
//	double K[6] =
//	{10, 20, 50, 100, 200, 500};
//	//for (int i = 0; i < Parameters.GSensors; i++)
//	for (int i = 0; i < dtLcard502->vecLogChannels.size(); i++)
//	{
//		GZone[i].reserve(LCard502_INIT_SIZE);
//		//GVoltPercent[i] = K[Parameters.channels[i/* + Parameters.CSensors + Parameters.LSensors*/].range];
//		GVoltPercent[i] = K[dtLcard502->vecLogChannels[i].adcRangeIndex];
//	}
	IsSGStarted = false;
	BufTime = SqlDBModule->GetFloatFieldSQL("SettingsGlobal","ReadBufTime",NULL, 1.0,err);
	// //lcard502.LoadSettings(&Parameters);
}
// ---------------------------------------------------------------------------
void TLCardData::ClearSGM(void) {
	int m = vecMeasuresData.size();
	for (int i = 0; i < vecMeasuresData.size(); i++)
		vecMeasuresData[i].ClearData();
}
// ---------------------------------------------------------------------------
void TLCardData::StartSGM(int _freqNum) {
	IsSGStarted = true;
	freqNum = _freqNum;
	if(freqNum == 0)
		ClearSGM();
    vecMeasure.clear();
	dtLcard502->Start();
//	dtLcard502->StartRead();

}
// ---------------------------------------------------------------------------
void TLCardData::StopSGM(void) {
//	dtLcard502->StopRead();
	dtLcard502->Stop();
	if (IsSGStarted)
		ReCalcMeasuresToChannels();
	IsSGStarted = false;
}
// ---------------------------------------------------------------------------
bool TLCardData::Read(int _freqNum) {
	freqNum = _freqNum;
	int size;
	//������ �������� ??? -> size
	double* buf = dtLcard502->Read(&size);
	// double* raw_buf=lcard502->GetRawC();
	if (size < 0)
	{
		lastError = dtLcard502->LastError;
		return (false);
	}
	//int packet_size = /* CZone.size() + LZone.size()+ */ GZone.size();
	//������ ����� - ���������� ��� �������
//	int kadrSize = vecMeasuresData[_freqNum].vecSensorsData.size(); //GZone.size();
//	int kadrs = size / kadrSize;

	if (IsSGStarted) {
		// for (int i = 0; i < size; i++) {
		// vecMeasure.push_back(buf[i]);
		// }
		// serg - ������������� ��� �� �� 14 ��������
		for (int i = 0; i < size; i++) {
			// serg
			double dtmp = buf[i] / 730;
			vecMeasure.push_back(buf[i] / 730);
		}



//		unsigned int size_new = GZone[0].size() + packets;
//		if (GZone[0].capacity() < size_new)
//		{
//			size_new *= (1 + LCard502_ADD_PERCENT / 100);
//			for (unsigned int s = 0; s < GZone.size(); s++)
//				GZone[s].reserve(size_new);
//		}
//		for (int p = 0; p < packets; p++)
//		{
//			double* pbuf = buf + p * packet_size;
//			for (unsigned int s = 0; s < GZone.size(); s++)
//				GZone[s].push_back(pbuf[s]*GVoltPercent[s]*10);
//		}


//----- �����: ������� �������� �� ������� �� ������� �����
//		unsigned int size_new = vecMeasuresData[_freqNum].vecSensorsData[0].size()+ kadrs;
//		if (vecMeasuresData[_freqNum].vecSensorsData[0].capacity() < size_new)
//		{
//			size_new *= (1 + LCard502_ADD_PERCENT / 100);
//			for (unsigned int s = 0; s < vecMeasuresData[_freqNum].vecSensorsData.size(); s++)
//				vecMeasuresData[_freqNum].vecSensorsData[s].reserve(size_new);
//		}
//		for (int p = 0; p < kadrs; p++)
//		{
//			double* pbuf = buf + p * kadrSize;
//			for (unsigned int s = 0; s < vecMeasuresData[_freqNum].vecSensorsData.size(); s++)
//				vecMeasuresData[_freqNum].vecSensorsData[s].push_back(pbuf[s] /**GVoltPercent[s]*10*/ ); //todo
//		}
	}
	return (true);
}
// ---------------------------------------------------------------------------
double TLCardData::GetValueAvg5ByChName(AnsiString _ch_name) {
	// ������� �������������� �� 5-��
	int err = -100;
	double ret = 0;
	TLogCh502Params* Ch = dtLcard502->FindChByName(_ch_name);//, err);
	for (int i = 0; i < 5; i++) {
		// ret += lcard502->GetValue(lcard502->FindNumChByName(_ch_name));
		// GetValueByChannel
		ret += dtLcard502->GetValue(Ch->logicalChannel);
	}
	// TPr::pr(Parameters.FindByName(_ch_name)->ToString());
	// TPr::pr(ret);
	return (ret / 5);
}
// ---------------------------------------------------------------------------
int TLCardData::SearchOfZeroTransition(int _sensNum)
{
//	//���������� �������
//	int channelsQuantity = vecMeasuresData[freqNum].vecSensorsData.size();
//	//����� ��������
//	int step = 0;
//	//�� ��������� �� �������������� ������� ����������
//	bool flag = false;
//	//���������� ������
//	int kadrsQuantity = vecMeasure.size() / channelsQuantity;
//	for(int i = 0; i < kadrsQuantity; i++)
//	{
//		if(!flag && vecMeasure[i*channelsQuantity+_sensNum] < 0)
//			flag = true; //����� ������������� �������
//		if(flag && vecMeasure[i*channelsQuantity+_sensNum] >= 0)
//		{
//			step = i*channelsQuantity;
//			break;
//		}
//	}
//	return step;

	//����� ��������
	int step = 0;
	//�� ��������� �� �������������� ������� ����������
	bool flag = false;
	//���������� ������
	int kadrsQuantity = vecZeroTransition.size();
	for(int i = 0; i < kadrsQuantity; i++)
	{
		if(!flag && vecZeroTransition[i] < 0)
			flag = true; //����� ������������� �������
		if(flag && vecZeroTransition[i] >= 0)
		{
			step = i;
			break;
		}
	}
	return step;
}
// ---------------------------------------------------------------------------
int TLCardData::SearchOfZeroTransitionPeriod(int _sensNum)
{
//	//���������� �������
//	int channelsQuantity = vecMeasuresData[freqNum].vecSensorsData.size();
//	//����� ��������
//	int step1 = 0;
//	int step2 = 0;
//	//�� ��������� �� �������������� ������� �������
//	bool flag = false;
//	//���������� ������
//	int kadrsQuantity = vecMeasure.size() / channelsQuantity;
//	for(int i = 0; i < kadrsQuantity; i++)
//	{
//		if(!flag && vecMeasure[i*channelsQuantity+_sensNum] < 0)
//			flag = true; //����� ������������� �������
//		if(flag && (step1 == 0) && vecMeasure[i*channelsQuantity+_sensNum] >= 0)
//		{
//			step1 = i*channelsQuantity;
//			flag = false;
//		}
//		if(flag && (step1 != 0) && vecMeasure[i*channelsQuantity+_sensNum] >= 0)
//		{
//			step2 = i*channelsQuantity - step1;
//			return step2;
//		}
//	}
//	return step2;

	//����� ��������
	int step1 = 0;
	int step2 = 0;
	//�� ��������� �� �������������� ������� �������
	bool flag = false;
	//���������� ������
	int kadrsQuantity = vecZeroTransition.size();
	for(int i = 0; i < kadrsQuantity; i++)
	{
		if(!flag && vecZeroTransition[i] < 0)
			flag = true; //����� ������������� �������
		if(flag && (step1 == 0) && vecZeroTransition[i] >= 0)
		{
			step1 = i;
			flag = false;
		}
		if(flag && (step1 != 0) && vecZeroTransition[i] >= 0)
		{
			step2 = i - step1;
			return step2;
		}
	}
	return step2;
}
// ---------------------------------------------------------------------------
bool TLCardData::CheckMufta(int _sensNum)
{
	//�� ����� ���������� ������
	int count = 0;
	int const maxCounts = 3;
	//���������� ������
	int kadrsQuantity = vecMeasuresData[freqNum].vecSensorsData[0].size();
	//�����������
	SGFilter->toFilter(&(vecMeasuresData[freqNum].vecSensorsData[_sensNum][0])
		,vecMeasuresData[freqNum].vecSensorsData[_sensNum].size());
	double thresVal = dtLcard502->globalSettings->checkMuftaLevel;
	for(int i = 0; i < kadrsQuantity; i++)
	{
		double val = vecMeasuresData[freqNum].vecSensorsData[_sensNum][i];
		if(val > thresVal)
		{
			if(++count > maxCounts)
			{
				return true;
			}
//			AnsiString s="";
//			s = "..\\..\\SavedResult\\";
//			s += FormatDateTime("yyyy_mm_dd_hh_mm_ss_", Now());
//			s += "checkMufta_";
//			s += "FHZ.csv";
//			TLog::SaveTxtVectDoubleFile(ExtractFilePath(Application->ExeName)+s
//				,lCardData->vecMeasuresData[0].vecSensorsData[_sensNum]
//				,lCardData->vecMeasuresData[0].vecSensorsData[_sensNum].size());
		}
	}
	return false;
}
// ---------------------------------------------------------------------------
vector<double> TLCardData::GetBarkValues(vector<int> &_Thresholds)
{   //����� �������, ��� ������ ��������� �����, � ������� ��� ���������� ����
//����� �������� BarkValues ��� ��������, ��� _Thresholds
	vector<double> BarkValues;// = new vector<double>;
	//� ����� �������� ��� �������
	int Period = vecMeasuresData[freqNum].vecSensorsData[0].size() / 3;
	for(int i = 0; i < _Thresholds.size(); i++)
	{
		int x = _Thresholds[i] * Period / 100;
		if (x >= vecMeasuresData[freqNum].vecSensorsData[0].size())
			x = vecMeasuresData[freqNum].vecSensorsData[0].size()-1;
		BarkValues.push_back(vecMeasuresData[freqNum].vecSensorsData[0][x]);
	}
	return BarkValues;
}
// ---------------------------------------------------------------------------
double TLCardData::GetCorrelation(vector<double> &_XCoords,vector<double> &_YCoords)
{
	double XSum = 0;
	double YSum = 0;
	double XYSum = 0;
	double X2Sum = 0;
	double Y2Sum = 0;
	double Corr = 0;

	if (_XCoords.size() != _YCoords.size())
	{
		return 0; //�� ������� ��� �� ���������� ��������
	}
	int vecSize = _XCoords.size();
	for (int i = 0; i < vecSize; i++)
	{
//		TProtocol::ProtocolSave("_XCoords["+IntToStr(i)+"]: "+FloatToStr(_XCoords[i]));
//		TProtocol::ProtocolSave("_YCoords["+IntToStr(i)+"]: "+FloatToStr(_YCoords[i]));
		XSum += _XCoords[i];
		YSum += _YCoords[i];
		XYSum += _XCoords[i] * _YCoords[i];
		X2Sum += _XCoords[i] * _XCoords[i];
		Y2Sum += _YCoords[i] * _YCoords[i];
	}
	Corr = (vecSize * XYSum - XSum * YSum)
		/ (sqrt(vecSize * X2Sum - XSum * XSum)
		* sqrt(vecSize * Y2Sum - YSum * YSum));
	TProtocol::ProtocolSave("Corr = "+FloatToStr(Corr));
	return Corr;
}
// ---------------------------------------------------------------------------
double TLCardData::GetSquareDeviation(vector<double> &_XCoords,vector<double> &_YCoords)
{
	double Corr = 0;
	double Diff = 0;
	if (_XCoords.size() != _YCoords.size())
	{
		return 0; //�� ������� ��� �� ���������� ��������
	}
	int vecSize = _XCoords.size();
	for (int i = 0; i < vecSize; i++)
	{
//		TProtocol::ProtocolSave("_XCoords["+IntToStr(i)+"]: "+FloatToStr(_XCoords[i]));
//		TProtocol::ProtocolSave("_YCoords["+IntToStr(i)+"]: "+FloatToStr(_YCoords[i]));
		Diff = _XCoords[i] - _YCoords[i];
		Corr += Diff * Diff;
	}
	TProtocol::ProtocolSave("Corr = "+FloatToStr(Corr));
	return Corr;  	//���������� ������� ��������, ����� ������ ������ 1.
//	return 1.0/(1+Corr);  	//��� ������ ����������, ��� ��������� ��
}
// ---------------------------------------------------------------------------

int TLCardData::ReCalcMeasuresToChannels()
{
	int arrSize = vecMeasure.size();
	if (arrSize == 0)
	{
		AnsiString msg = "�� ������� ������!!! \n";
		TProtocol::ProtocolSave("ReCalcMeasuresToChannels "+msg);
		TLog::SaveStrMsgLog(msg);
//		TExtFunction::ShowBigModalMessage(msg, clRed);
		return -1;
	}
	int ChanCount = vecMeasuresData[freqNum].vecSensorsData.size();
	vecZeroTransition.clear();
	int zeroTransitionChannel = dtLcard502->globalSettings->zeroTransitionChannel;
	int j = zeroTransitionChannel;
	while(j < arrSize)
	{
		vecZeroTransition.push_back(vecMeasure[j]);
		j += ChanCount;
	}
	if (dtLcard502->globalSettings->isFilter) {
	  SGFilter->toFilter(&(vecZeroTransition[0]),vecZeroTransition.size());
	}else{
	 //
	}

	//������� ����� �������� �� ��������������� ������
	int p = SearchOfZeroTransition(zeroTransitionChannel)*ChanCount;
	int ThreePeriods = SearchOfZeroTransitionPeriod(zeroTransitionChannel) * ChanCount * 3;
	if(ThreePeriods < arrSize - p)
		arrSize = ThreePeriods + p;
	vecZeroTransition.clear();
	if (ThreePeriods == 0)
	{
		AnsiString msg = "������� ������ � �������!!! \n";
		TProtocol::ProtocolSave("ReCalcMeasuresToChannels "+msg);
		TLog::SaveStrMsgLog(msg);
//		TExtFunction::ShowBigModalMessage(msg, clRed);
//		return -2;
	}
	//�������� ��������� �������
	vector< vector<double> > UncuttedData;
//	int ChanCount = vecMeasuresData[freqNum].vecSensorsData.size();
	for (int i = 0; i < ChanCount; i++)
	{
		vector<double> ChanVec;
		UncuttedData.push_back(ChanVec);
	}
	//� ������ �������� ������
	int sens = 0;
	for (int i = 0; i < vecMeasure.size(); i++)
	{
		UncuttedData[sens].push_back(vecMeasure[i]);
		sens++;
		if(sens == ChanCount)
			sens = 0;
	}
	//����������� �����������
	if(dtLcard502->globalSettings->isFilter)
	{
		for (int i = 0;i < UncuttedData.size(); i++)
		{
			SGFilter->toFilter(&(UncuttedData[i][0]),UncuttedData[i].size());
		}
	}
	//������� ������� ������
	for(int i = 0; i < vecMeasuresData[freqNum].vecSensorsData.size(); i++)
		vecMeasuresData[freqNum].vecSensorsData[i].clear();
	//� ������ �������� ������
	sens = 0;
	for (; p/ChanCount < arrSize/ChanCount; p++)
	{
		vecMeasuresData[freqNum].vecSensorsData[sens].push_back(UncuttedData[sens][p/ChanCount]); 
		sens++;
		if(sens == ChanCount)
			sens = 0;
	}
	return 0;
}
// ---------------------------------------------------------------------------
