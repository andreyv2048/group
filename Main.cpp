// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "TProtocol.h"
#include "ColorSettings.h"
#include "Filters.h"
// ---------------------------------------------------------------------------
#include "unFmMessage.h"
#include "ABOUT_NTC_NK_URAN.h"
#include "unTExtFunction.h"
#include "unDiagnost.h"
#include "unFmEditSettings.h"
#include "uFSignalsState.h"
#include "SignalListDef.h"
#include "ADCSettings.h"
#include "SGSettings.h"
#include "EtalonDatas.h"
#include "unExtSettings.h"
#include "InOutBits.h"
#include "unPasswordForm.h"
#include "Queries.h"
#include "About.h"
// -----------------------------------

#pragma resource "*.dfm"
TMainForm *MainForm;

// ---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner) {

}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender) {

    std::list<UnicodeString> l = GetGost();
	for(std::list<UnicodeString>::iterator it = l.begin(); it != l.end(); ++it)
	{
		cbSGGost->Items->Add(*it);
	}

    options = false;
	lastError = 0;
	AnsiString applPath = ExtractFilePath(Application->ExeName);
	mainGlobalSettings.applPath = applPath;
	applPath += "connectSQL.udl";
	mainGlobalSettings.LoadPRGSettings(applPath);
	mainGlobalSettings.threadMsg = RegisterWindowMessage(L"Thread");
	mainGlobalSettings.mainHandle = this->Handle;
	if (mainGlobalSettings.threadMsg == 0) {
		Application->MessageBoxW(L"TMainForm::FormCreate: �� ���� ���������������� ���������", L"������", MB_ICONERROR | MB_OK);
	}
	else {
		//
	}
	PanelSG->Width = 350;
	l�ard502 = new TLCard502(&mainGlobalSettings, lastError);
	// serg
	if (lastError < 0) {
		return;
	}
	else {
		//
	}
	TSFreqs = new TSFrequencies(mainGlobalSettings.indexCurrentTypeSize);
	int freqCount = TSFreqs->Frequency.size();
	lCardData = new TLCardData(l�ard502, freqCount, l�ard502?l�ard502->countLogCh:0); // todo �������� 1 �������
	lastError = 1;
	SLD = new SignalListDef(&lastError);
	SLD->dev = new InOutBits(l�ard502);
	// serg
	if (lastError < 0) {
		return;
	}
	else {
		//
	}
	SGFilter = new Filters("SG");
	SGFilter->setSettingsFromDB();

	// ����� �����������
	SqlDBModule->FillComboBox("TypeSizes", "TSName", cbTypeSize);
	for (int i = 0; i < cbTypeSize->Items->Count; i++) {
		if ((int)cbTypeSize->Items->Objects[i] == mainGlobalSettings.indexCurrentTypeSize) {
			cbTypeSize->ItemIndex = i;
			break;
		}
		else {
			cbTypeSize->ItemIndex = -1;
		}
	}
	// ����� �����
	cbSGGost->ItemIndex = mainGlobalSettings.indexCurrentSGGost;
	TExtFunction::PrepareChartToTst(SignalChart, 3, 600, 2000);
	SignalChart->Series[0]->Title += " ������";
	SignalChart->Series[1]->Title += " ����������";
	SignalChart->Series[2]->Title += " ���";

	ChangeColor();
	TExtFunction::PrepareChartToTst(EtalonChart, 3, 10, 2000);
	// �������� ������ ��
	// SqlDBModule->FillComboBox("SolidGroups", "SGName", cbxSG);
	SqlDBModule->FillComboBoxFromSql("select rec_id, SGName as F1 from SolidGroups where Gost_id=" +
		IntToStr(mainGlobalSettings.indexCurrentSGGost), cbxSG);
	cbxSG->ItemIndex = -1;
	queryEtalon->Connection = SqlDBModule->ADOConnectionDB;

	// �� ������ ������ ������� ������� ��� ���������
	SLD->oSENSORON->Set(false);
	// ������� ����������
	SLD->oSENSLOWPOW->Set(true);
#ifdef VIRT1730
	SLD->iCC->Set(true);
#endif
	EnableWigits(false);
}

// ------------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject * Sender) {
	if (threadWork) {
		threadWork->Terminate();
		threadWork->WaitFor();
		threadWork->TestExitLoop();
		threadWork = NULL;
	}

	if (lCardData) {
		delete lCardData;
		lCardData = NULL;
	}

	if (SLD) {
		SLD->oSENSORON->Set(false);
		SLD->oSENSLOWPOW->Set(false);
		delete SLD;
        SLD = NULL;
	}

if (l�ard502) {
  	delete l�ard502;
	l�ard502 = NULL;
}
if (gen) {
	delete gen;
	gen =  NULL;
}
	if (TSFreqs) {
		delete TSFreqs;
		TSFreqs = NULL;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::MenuProtocolClick(TObject * Sender) {
	TProtocol::Show();
}

// ---------------------------------------------------------------------------
void TMainForm::LoadFromFile(UnicodeString FileName) {
	try {
		TLog::LoadTxtChDoubleFile(AnsiString(FileName).c_str(), lCardData, 0,3); //serg
		TSG* solidGroup = new TSG(&mainGlobalSettings, lCardData);
		csg = solidGroup->Get1FrecSG();
		delete solidGroup;
		Redraw();
	}
	catch (Exception *ex) {
		TLog::ErrFullSaveLog(ex);
		MessageDlg(ex->Message, mtError, TMsgDlgButtons() << mbOK, NULL);
	}
}

// ---------------------------------------------------------------------------
void TMainForm::SaveToFile(UnicodeString FileName) {
	try {
		// ���������� � ����
		int arrSize = lCardData->vecMeasure.size();
		TLog::SaveChTxtDoubleFile(FileName, &(lCardData->vecMeasure[0]), arrSize, 3); //serg
	}
	catch (Exception *ex) {
		TLog::ErrFullSaveLog(ex);
		MessageDlg(ex->Message, mtError, TMsgDlgButtons() << mbOK, NULL);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::menuSaveClick(TObject * Sender) {
	int arrS = SignalChart->Series[0]->Count();
	if (arrS > 0) {
		//
	}
	else {
		MessageDlg("�������� ������!!!", mtError, TMsgDlgButtons() << mbOK, NULL);
		return;
	}
	if (SaveDialog->Execute())
		SaveToFile(SaveDialog->FileName);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::menuOpenClick(TObject * Sender) {
	if (OpenDialog->Execute())
		LoadFromFile(OpenDialog->FileName);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::WmDropFiles(TWMDropFiles & Message) {
	// HDROP drop_handle = (HDROP)Message.Drop;
	// wchar_t fName[MAXPATH];
	// int filenum = DragQueryFile(drop_handle, -1, NULL, NULL);
	// for (int i = 0; i < filenum; i++)
	// {
	// DragQueryFile(drop_handle, i, fName, MAXPATH);
	// ReadFile(fName);
	// }
	// DragFinish(drop_handle);
}

// ---------------------------------------------------------------------------------
// ����� ��������������, ��������� ���������� � ��������� ���� -----
void __fastcall TMainForm::ReadFile(AnsiString FileName) {
	// AnsiString str = ExtractFileExt(FileName);
	// if (str == ".sg")
	// MainForm->LoadFromFile(FileName);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::SignalChartClick(TObject * Sender) {
	// SignalChart->SetFocus();
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::SignalChartScroll(TObject * Sender) {
	lMinScrollbar->Caption = "" + IntToStr((int) SignalChart->BottomAxis->Minimum);
	lMaxScrollbar->Caption = "" + IntToStr((int) SignalChart->BottomAxis->Maximum);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::scbChartDraggerChange(TObject * Sender) {
	double length = 200;
	for (int i = 0; i < SignalChart->SeriesCount(); ++i) {
		if (length < SignalChart->Series[i]->MaxXValue()) {
			length = SignalChart->Series[i]->MaxXValue();
		}
	}
	double max = SignalChart->BottomAxis->Maximum;
	double min = SignalChart->BottomAxis->Minimum;
	double step = (double)(max - min) / 100.0;
	if (length > 0)
		scbChartDragger->Max = (int)ceil((double)length * 100.0 / (double)(max - min));
	else
		scbChartDragger->Max = (int)ceil((double)100.0 * 100.0 / (double)(max - min));
	// curPos = max/step;
	long change = scbChartDragger->Position - curPos;
	if (change > 0) {
		SignalChart->BottomAxis->Maximum = change * step + max;
		SignalChart->BottomAxis->Minimum = change * step + min;
	}
	else {
		SignalChart->BottomAxis->Minimum = change * step + min;
		SignalChart->BottomAxis->Maximum = change * step + max;
	}

	SignalChart->Refresh();
	curPos = scbChartDragger->Position;
	SignalChartScroll(Sender);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationEventsMessage(tagMSG & Msg, bool &Handled) {
	if (Msg.message == WM_HOTKEY) { // ��������� ����
		if (Msg.wParam == 0x00E) // ������������� ���
		{
			WinExec("hh ..\\..\\help\\Help.chm", SW_RESTORE);
			Handled = true;
		}
	}
	if (Msg.message == mainGlobalSettings.threadMsg) { // Handled = true;
		switch (Msg.wParam) {
		case ThreadWork::REDRAW: {
				if(NULL == threadWork) break;
				TProtocol::ProtocolSave("���������: ������������");
				csg = threadWork->GetSG();
				Redraw();
				break;
			}
		case ThreadWork::COMPUTE: {
				if(NULL == threadWork) break;
				AnsiString a = "���������: ������ �����, ���: ";
				a += Msg.lParam;
				TProtocol::ProtocolSave(a);
				if (threadWork != NULL)
					threadWork->SetCalc();
				break;
			}
		case ThreadWork::COMPLETE: { {
		        if(NULL == threadWork) break;
					AnsiString a = "���������: ���������� �����, ���: ";
					a += Msg.lParam;
					TProtocol::ProtocolSave(a);
				} threadWork->Terminate();
				SLD->LatchesTerminate();
				SLD->SetAlarm(false);
				threadWork->WaitFor();
				threadWork->TestExitLoop();
				delete threadWork;
				threadWork = NULL;
				SetAbleButtons(true);

				if (Msg.lParam == 1 && !SLD->iCC->WasConst(false, 100)) {
					// ���� ���� ��������� ������ ��������� - ��������
					Start();
				}
				else {
					if (SLD->iCC->WasConst(false, 100))
						TProtocol::ProtocolSave("������� ���� ����������");
					// ���� ���� ��������� ������ � ������� - ���������
					TProtocol::ProtocolSave("_return_code==false");
					// bWork->Caption = "������";
					// bCancelWork->Enabled = false;
				}
				break;
			}
		case ThreadWork::UPDATE_STATUS: {
				if (threadWork != NULL)
					UpdateStatus(threadWork->GetStext1(), threadWork->GetStext2());
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void TMainForm::UpdateStatus(AnsiString _text1, AnsiString _text2) {
	StatusBar->Panels->Items[0]->Text = _text1;
	StatusBar->Panels->Items[1]->Text = _text2;
	StatusBar->Font->Size = 20;
	// StatusBar->Font->Color=clRed;
	StatusBar->Refresh();
}

// ---------------------------------------------------------------------------
void TMainForm::ClearCharts(void) {
	 int count = SignalChart->SeriesCount();
	 for(int i = 0; i < count; ++i)
	 {
		  SignalChart->Series[i]->Clear();
	 }
	 PanelSG->Caption = "����";
	 PanelSG->Color = clBtnFace;
}

// ---------------------------------------------------------------------------
void TMainForm::DrawOnChart() {
	// ClearCharts();
	// if (lSGSignal.size() < 100)
	// return;
	// SignalChart->BottomAxis->Automatic = true;
	// SignalChart->BottomAxis->SetMinMax(0, 1000);
	// SignalChart->LeftAxis->Automatic = true;
	// SignalChart->Refresh();
	// bool DrawNetU = ini->ReadBool("SignalView","NetU",true);
	// bool DrawSignalSG = ini->ReadBool("SignalView","SignalSG",true);
	// bool DrawSynchroDetector = ini->ReadBool("SignalView","SynchroDetector",true);
	// bool DrawInductionSignal = ini->ReadBool("SignalView","InductionSignal",true);
	// bool DrawSynchroSignal = ini->ReadBool("SignalView","SynchroSignal",true);
	// for (unsigned int i = 0; i < lSGSignal.size(); i++)
	// {
	////		Series->AddXY(i, lSGSignal[i].I, "", clBlue);
	////		Series1->AddXY(i, lSGSignal[i].U, "", clRed);
	// if(DrawNetU)SeriesNetU->AddXY(i, lSGSignal[i].NetU, "", SeriesNetU->SeriesColor);
	// if(DrawSignalSG)SeriesSignalSG->AddXY(i, lSGSignal[i].SignalSG, "", SeriesSignalSG->SeriesColor);
	// if(DrawSynchroDetector)SeriesSynchroDetector->AddXY(i, lSGSignal[i].SynchroDetector, "", SeriesSynchroDetector->SeriesColor);
	// if(DrawInductionSignal)SeriesInductionSignal->AddXY(i, lSGSignal[i].InductionSignal, "", SeriesInductionSignal->SeriesColor);
	// if(DrawSynchroSignal)SeriesSynchroSignal->AddXY(i, lSGSignal[i].SynchroSignal, "", SeriesSynchroSignal->SeriesColor);
	// }
	// vector<vector<SGPoint> >sgPoints = sgc->GetPoints(lSGSignal);
	// for (unsigned int t = 0; t < sgPoints.size(); t++)
	// {
	// for (unsigned int i = 0; i < sgPoints[t].size(); i++)
	// {
	////			Series2->AddXY(sgPoints[t][i].index, sgPoints[t][i].I, "", clBlue);
	////			Series3->AddXY(sgPoints[t][i].index, sgPoints[t][i].U, "", clRed);
	// SeriesDotsSignalSG->AddXY(sgPoints[t][i].index, sgPoints[t][i].SignalSG, "", SeriesDotsSignalSG->SeriesColor);
	// SeriesDotsNetU->AddXY(sgPoints[t][i].index, sgPoints[t][i].NetU, "", SeriesDotsNetU->SeriesColor);
	// }
	// }
	// vector<double>Coords = sgc->GetCoords(sgPoints);
	// String str = "";
	// for (unsigned i = 0; i < Coords.size(); i++)
	// str += FloatToStrF(Coords[i], ffFixed, 3, 3) + "   ";
	// StatusBar->Panels->Items[1]->Text = str;
	// StatusBar->Refresh();
}

// ---------------------------------------------------------------------------
void TMainForm::DrawThresholds() {
	// for (int i = 0; i < (int)local_th.size(); i++)
	// {
	// lsThresholds[i]->AddXY(0, local_th[i], "", clBlack);
	// lsThresholds[i]->AddXY(SignalChart->Series[0]->MaxXValue(), local_th[i], "", clBlack);
	// SignalChart->AddSeries(lsThresholds[i]);
	// }
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::Edit1Exit(TObject * Sender) {
	// TEdit* p = (TEdit*)Sender;
	// double val;
	// try
	// {
	// val = p->Text.ToDouble();
	// local_th[p->Tag] = val;
	// }
	// catch (...)
	// {
	// p->SetFocus();
	// }
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::bStartClick(TObject * Sender) {
	mainGlobalSettings.isWork=true;
	if (threadWork == NULL)
		Start();
	else
		ReStart();
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::bCancelClick(TObject * Sender) {
    mainGlobalSettings.isWork=false;
	Stop();
}

// ---------------------------------------------------------------------------
void TMainForm::Start() {
	int err = 0;
	try {
		bool isControl = SLD->iCC->Get(); // ��������� ���� ����������
		if (isControl) {
			SetAbleButtons(false);
			if (threadWork != NULL) {
				TProtocol::ProtocolSave("TMainForm::Start: ������ ��� ��������");
				return;
			}
			inWork = true;
			SLD->oSENSLOWPOW->Set(true);
			// SLD->oSENSORON->Set(true);//��� 12�

			// TProtocol::Clear();
			// bCancel->Enabled = true; //����� ������ ����� ��������
			if (threadWork != NULL) {
				threadWork->Terminate();
				threadWork->WaitFor();
				threadWork->TestExitLoop();
				ThreadWork *x = threadWork;
				threadWork = NULL;
				delete x;
			}
			if (!gen) {
			   //	gen = new TGSPF052(&mainGlobalSettings, err);
			   gen = new TGSPF052();
			   if(NULL == gen->hDLL)
			   {
					delete gen;
					gen = NULL;
                    SetAbleButtons(true);
					inWork = false;
					SLD->oSENSORON->Set(false);
					SLD->LatchesTerminate();
					return;
			   }
			}
			//gen->SetSampleFreq(mainGlobalSettings.discrFrecGSPF);
			TSFrequencies TSFreqs = TSFrequencies(mainGlobalSettings.indexCurrentTypeSize);
			if (TSFreqs.Frequency.size() <= 0) {
				TProtocol::ProtocolSave("������: ������ ��� ����� ����������� ��� � ���� ������");
				TExtFunction::ShowBigModalMessage("������: ������ ��� ����� ����������� ��� � ���� ������", clBlue);
				return;
			}
		   //	gen->SetSampleFreq(mainGlobalSettings.discrFrecGSPF);
			int f = TSFreqs.Frequency[0];
			double a = TSFreqs.Amplitude[0];
			gen->FormSignal(f, a);
			threadWork = new ThreadWork(true, lCardData, &mainGlobalSettings, gen);
			SLD->SetAlarm(true);
		}
		else {
			AnsiString msg = "��� ������� ����� ����������!!! \n";
			TExtFunction::ShowBigModalMessage(msg, clRed);
			SetAbleButtons(true);
			return;
		}

	}
	catch (Exception *ex) {
		TLog::ErrFullSaveLog(ex);
		MessageDlg(ex->Message, mtError, TMsgDlgButtons() << mbOK, NULL);
	}
}

// ---------------------------------------------------------------------------
void TMainForm::ReStart() {
	// ������������ ������ ��� �������� ��� ���������, ������� � ��� ���
	if (threadWork == NULL) {
		TProtocol::ProtocolSave("TMainForm::ReStart: ������ �� ��������");
		return;
	}
	threadWork->SetCalc();
}

// ---------------------------------------------------------------------------
void TMainForm::Stop() {
	if (threadWork == NULL) {
		TProtocol::ProtocolSave("TMainForm::Stop: ������ �� ��������");
		return;
	}
	// ������ �����
	threadWork->Terminate();
   //	threadWork->SetCalc();
	threadWork->WaitFor();
	threadWork->TestExitLoop();
	ThreadWork *x = threadWork;
	threadWork = NULL;
	Sleep(1000);
	delete x;

	// �������� ���������
  //	if (gen) {
  //		delete gen;
   //		gen = NULL;
   //	}
	// MainMenu->Items->Visible = true;
	// GroupBoxNGr->Visible = true;
	// bStart->Enabled = true;
	SetAbleButtons(true);
	inWork = false;
	SLD->oSENSORON->Set(false);
	SLD->LatchesTerminate();
	UpdateStatus("�������� ������", "������ \"������\"");
	TProtocol::ProtocolSave("User: �������� ������ ������");
}

// ---------------------------------------------------------------------------
void TMainForm::Redraw() {
	// ������� ��� ���������� ������
	// int MeasCount = lCardData->vecMeasuresData.size();
	// TExtFunction::PrepareFastLineChart
	// (MeasCount * l�ard502->countLogCh, 2500, SignalChart);
	// for (int k = 0; k < MeasCount; k++) {
	// for (int i = 0; i < l�ard502->countLogCh; i++) {
	// int sz =
	// lCardData->vecMeasuresData[k].vecSensorsData[0].size();
	// double* arr = new double[sz];
	// for (int j = 0; j < sz; j++) {
	// arr[j] =
	// lCardData->vecMeasuresData[k].vecSensorsData
	// [i][j] - k * 10.0; // ������ �����
	// }
	// SignalChart->Series[k * l�ard502->countLogCh + i]
	// ->AddArray(arr, sz);
	// }
	// }

	// ��������� ����� ��������
	int chCount = lCardData->vecMeasuresData[0].vecSensorsData.size();
	TExtFunction::PrepareChartToTst(SignalChart, chCount, 0, 0);
	ChangeColor();
	SignalChart->Series[0]->Title += " ������";
	SignalChart->Series[1]->Title += " ����������";
	SignalChart->Series[2]->Title += " ���";
	// ������� �� ����� �������
	int arrSize;
	for (int i = 0; i < chCount; i++) {
		arrSize = lCardData->vecMeasuresData[0].vecSensorsData[i].size();
		double* tmpArray = new double[arrSize];
		for (int j = 0; j < arrSize; j++) {
			tmpArray[j] = lCardData->vecMeasuresData[0].vecSensorsData[i][j];
		}
		// SignalChart->Series[i]->Color=colorSer[i+1];
		SignalChart->Series[i]->AddArray(tmpArray, arrSize - 1);
	}
	// ��������� �������� �� �������
	vector<int>Thresholds;
	int res;
	// ��������� ������ �� ����������  �����������
	queryEtalon->Close();
	AnsiString strSQL = "select threshold_value from TypeSizeThresholds where TS_id=:pTS_id order by threshold_value";
	queryEtalon->SQL->Text = strSQL;
	queryEtalon->Parameters->ParamByName("pTS_id")->Value = mainGlobalSettings.indexCurrentTypeSize;
	queryEtalon->Open();
	queryEtalon->First();
	int tst = queryEtalon->RecordCount;
	// serg
	if (tst == 0) {
		MessageDlg("�� ������� �������� �������! ��������� ������� ��������.", mtWarning, TMsgDlgButtons() << mbOK, NULL);
		return;
	}
	else {
		//
	}
	while (!queryEtalon->Eof) {
		res = queryEtalon->FieldByName("threshold_value")->AsInteger;
		Thresholds.push_back(res);
		queryEtalon->Next();
	}
	queryEtalon->Close();
	// ��������� ���� �������
	vector<double>BarkValues;
	BarkValues = lCardData->GetBarkValues(Thresholds);
	// ������� ����������� �����
	TFastLineSeries* series = new TFastLineSeries(SignalChart);
	// series->ColorEachPoint = true;
	series->Marks->Visible = false;
	series->LinePen->Width = 3;
	series->Color = clBlack;
	series->ShowInLegend = false;
	SignalChart->AddSeries(series);
	double PeriodProc = arrSize / 300.0;
	for (int i = 0; i < Thresholds.size(); i++) {
		res = (int)(Thresholds[i] * PeriodProc);
		SignalChart->Series[SignalChart->SeriesCount() - 1]->AddXY(res, BarkValues[i], "", clBlack);
	}
	// //������ ����� ��� ��������
	// TColor colorSer[7];
	// colorSer[0] = clBlack;
	// colorSer[1] = clBlue;
	// colorSer[2] = clRed;
	// colorSer[3] = clGreen;
	// colorSer[4] = clFuchsia;
	// colorSer[5] = clYellow;
	// colorSer[6] = clWhite;
	// ������ � ��
	PanelSG->Width = 180;
	PanelSG->Caption = csg.group; // + " " + IntToStr((int)(csg.probability * 100)) + "%";
	PanelSG->Width *= 2;
	// if(csg.group_id < 7)
	PanelSG->Color = csg.color; // colorSer[csg.group_id]; //todo ���������!
	PanelSG->Font->Color = 16777215 - (int)csg.color;
	// else PanelSG->Color = clLime;
	// �������� ���� ������������������� ������� ��������
	EtalonDatas EDatas = EtalonDatas(mainGlobalSettings.indexCurrentTypeSize, mainGlobalSettings.indexCurrentSGGost);
	TExtFunction::PrepareChartToTst(EtalonChart, EDatas.Etalons.size(), 0, 0);

	// ������� �� ����� �������
	arrSize = EDatas.Etalons[0].BarkValues.size();
	for (int i = 0; i < EDatas.Etalons.size(); i++) {
		// arrSize = lCardData->vecMeasuresData[0].vecSensorsData[i].size();
		double* tmpArray = new double[arrSize];
		for (int j = 0; j < arrSize; j++) {
			tmpArray[j] = EDatas.Etalons[i].BarkValues[j];
		}
		// int sg_id = EDatas.Etalons[i].SG_id;
		// if(sg_id < 0)
		// sg_id = 0;
		EtalonChart->Series[i]->Title = EDatas.Etalons[i].SG_name;
		// if(sg_id < 7)
		EtalonChart->Series[i]->Color = EDatas.Etalons[i].Color; // colorSer[sg_id];
		// else EtalonChart->Series[i]->Color = clLime;
		EtalonChart->Series[i]->AddArray(tmpArray, arrSize - 1);
	}

	// �� ���� ��������� - ��� � ��� �� ���� ������ ��� ����
	// series = new TFastLineSeries(EtalonChart);
	////	series->ColorEachPoint = true;
	// series->Marks->Visible = false;
	// series->LinePen->Width = 3;
	// series->Color = clBlack;
	// series->ShowInLegend = true;
	// EtalonChart->AddSeries(series);
	EtalonChart->Series[EtalonChart->SeriesCount() - 1]->ShowInLegend = true;
	EtalonChart->Series[EtalonChart->SeriesCount() - 1]->Title = "����� ������";
	// EtalonChart->Series[SignalChart->SeriesCount()-1]->LinePen->Width = 3;
	for (int i = 0; i < Thresholds.size(); i++) {
		EtalonChart->Series[EtalonChart->SeriesCount() - 1]->AddY(BarkValues[i], "", clBlack);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::menuColorSettClick(TObject * Sender) {
	FormColor = new TFormColor(this);
	FormColor->Show();
	// delete FormColor;
	// FormColor->ShowModal();
	// ChangeColor();
}

// ---------------------------------------------------------------------------
void TMainForm::ChangeColor() {
	// ����� �����
	SignalChart->Series[0]->Color = clBlue; // ����������
	SignalChart->Series[1]->Color = clRed; // ����������
	SignalChart->Series[2]->Color = clGreen; // ���
	SignalChart->Series[3]->Color = clBlack; // DotsMeasure2
}

// ---------------------------------------------------------------------------

void __fastcall TMainForm::menuSettingsClick(TObject * Sender) {
	ADCSettForm = new TADCSettForm(this, l�ard502);
	ADCSettForm->ShowModal();
	delete ADCSettForm;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::menuTypeSizeClick(TObject * Sender) {
	// SettingsForm->ShowModal();
	fmTypeSizes = new TfmTypeSizes(this);
	fmTypeSizes->ShowModal();
	delete fmTypeSizes;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::menuAboutClick(TObject * Sender) {
	fmAboutBox = new TfmAboutBox(this);
	fmAboutBox->ShowModal();
	fmAboutBox->Close();
	fmAboutBox = NULL;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::menuSGSettClick(TObject * Sender) {

	SGSettForm = new TSGSettForm(this, &mainGlobalSettings);
	SGSettForm->ShowModal();
	SGSettForm->Close();
	SGSettForm = NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::PanelSQClick(TObject * Sender) {
	// PanelTop->FullRepaint();
	// ToolBarTop->Refresh();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::PanelONClick(TObject * Sender) {
	// PanelTop->FullRepaint();
	// ToolBarTop->Refresh();
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::mnuCheckGenClick(TObject * Sender) {
	fmDiagnost = new TfmDiagnost(this, &mainGlobalSettings, l�ard502, lCardData);
	// fmDiagnost->Show();
	fmDiagnost->ShowModal();
	fmDiagnost->Close();
	fmDiagnost = NULL;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mnuCheck1730Click(TObject * Sender) {
		FSignalsState = new TFSignalsState(this, &mainGlobalSettings, SLD);
		FSignalsState->Show();
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::menuEtalonsClick(TObject *Sender) {
	fmEditEtalon = new TfmEditEtalon(this, &mainGlobalSettings, lCardData);
	fmEditEtalon->ShowModal();
	fmEditEtalon = NULL;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::cbTypeSizeSelect(TObject *Sender) {
	int ind = cbTypeSize->ItemIndex;
	mainGlobalSettings.indexCurrentTypeSize = (int)cbTypeSize->Items->Objects[ind];
	SqlDBModule->UpdIntSql("SettingsGlobal", "indexCurrentTypeSize", mainGlobalSettings.indexCurrentTypeSize, NULL);
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::cbSGGostSelect(TObject *Sender) {
	int ind = cbSGGost->ItemIndex;
	mainGlobalSettings.indexCurrentSGGost = ind;
	SqlDBModule->UpdIntSql("SettingsGlobal", "indexCurrentSGGost", mainGlobalSettings.indexCurrentSGGost, NULL);
	// serg
	// SqlDBModule->FillComboBox("SolidGroups", "SGName", cbxSG);
	SqlDBModule->FillComboBoxFromSql("select rec_id, SGName as F1 from SolidGroups where Gost_id=" +
		IntToStr(mainGlobalSettings.indexCurrentSGGost), cbxSG);
	cbxSG->ItemIndex = -1;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::bbtCreateEtalonClick(TObject *Sender) {
	AnsiString strSQL = "";
	AnsiString strCmdSql = "";
	int ind = 0;
	int err;
	int freq = 0;
	double res = 0;
	try {
		if (cbxSG->ItemIndex == -1) {
			MessageDlg("�������� ������ ���������", mtError, TMsgDlgButtons() << mbOK, NULL);
			return;
		}
		else {
			int freqN = lCardData->getFreqNum();
			int arrS = SignalChart->Series[0]->Count();
			if (arrS > 0) {

			}
			else {
				MessageDlg("�������� ������!!!", mtError, TMsgDlgButtons() << mbOK, NULL);
				return;
			}
			AnsiString whereStr = "TS_id = " + IntToStr(mainGlobalSettings.indexCurrentTypeSize) + " ORDER BY frequency_value ASC";
			freq = SqlDBModule->GetIntFieldSQL("TypeSizeFrequencies", "frequency_value", whereStr, 50000, err);
			// ������� � ����, ��� ������ ��� ������ ������
			AnsiString fileName = "";
			// fileName = "..\\..\\SavedEtalons\\�_";
			// fileName = ExtractFilePath(Application->ExeName);
			// fileName+="\SavedEtalons\\E_";
		}
		AnsiString msg = "����������� ������� ������ ��������� ";
		// int ss=cbxSG->ItemIndex;
		msg += cbxSG->Text;
		// msg += cbxSG->Items[cbxSG->ItemIndex]-> Text;
		msg += "\n �������?";
		if (MessageDlg(msg, mtConfirmation, TMsgDlgButtons() << mbOK << mbCancel, NULL) == mrOk) {

		}
		else {
			MessageDlg("������!", mtConfirmation, TMsgDlgButtons() << mbOK, NULL);
			return;
		}
		// AnsiString fileName = "d:\\SavedEtalons\\E_";
		AnsiString fileName = mainGlobalSettings.SaveEtalonPath;
		fileName += "\\E_";
		fileName += FormatDateTime("yyyy_mm_dd_hh_mm_ss_", Now());
		fileName += IntToStr(freq);
		fileName += "_TS_";
		// fileName += IntToStr(mainGlobalSettings.indexCurrentTypeSize);
		fileName += SqlDBModule->GetStrFieldSQL("TypeSizes", "TSName", "rec_id = " + IntToStr(mainGlobalSettings.indexCurrentTypeSize),
			0, err);
		fileName += "_SG_";
		fileName += cbxSG->Text;
		fileName += "_FHZ.csv";
		// �������� �������� � ����
		ind = cbxSG->ItemIndex;
		strSQL = "INSERT INTO Etalons(ts_id,fenable,frequency,address_file,sg_id) VALUES( ";
		strSQL += IntToStr(mainGlobalSettings.indexCurrentTypeSize);
		strSQL += ",";
		strSQL += IntToStr(1);
		strSQL += ",";
		strSQL += IntToStr(freq);
		strSQL += ",'";
		strSQL += fileName;
		strSQL += "',";
		strSQL += IntToStr((int)cbxSG->Items->Objects[ind]);
		strSQL += ");\n";
		strSQL += "SELECT @@IDENTITY AS 'LAST_ID';";
		queryEtalon->Close();
		queryEtalon->SQL->Text = strSQL;
		queryEtalon->Open();
		int etId = queryEtalon->FieldByName("LAST_ID")->AsInteger;
		queryEtalon->Close();
		strSQL = "INSERT INTO EtalonValues Select ";
		strSQL += IntToStr(etId);
		strSQL += ",threshold_value, 123 from TypeSizeThresholds where TS_id=";
		strSQL += IntToStr(mainGlobalSettings.indexCurrentTypeSize);
		strSQL += " order by threshold_value";
		queryEtalon->SQL->Text = strSQL;
		queryEtalon->ExecSQL();
		queryEtalon->Close();
		// fmEditEtalon = new TfmEditEtalon(this,pGlobalSettings,lCardData);
		// fmEditEtalon->ShowModal();
		// fmEditEtalon=NULL;
		// cbxSG->ItemIndex=-1;

		// ��������� �������� �� �������
		vector<int>Thresholds;

		// ��������� ������ �� ����������  �����������
		queryEtalon->Close();
		strSQL = "select threshold_value from TypeSizeThresholds where TS_id=:pTS_id order by threshold_value";
		queryEtalon->SQL->Text = strSQL;
		queryEtalon->Parameters->ParamByName("pTS_id")->Value = mainGlobalSettings.indexCurrentTypeSize;
		queryEtalon->Open();
		queryEtalon->First();
		int tst = queryEtalon->RecordCount;
		while (!queryEtalon->Eof) {
			res = queryEtalon->FieldByName("threshold_value")->AsInteger;
			Thresholds.push_back(res);
			queryEtalon->Next();
		}
		queryEtalon->Close();
		strSQL =
			"SELECT rec_id as vrec_id, etalon_id,thres_val,barkgausen_val FROM EtalonValues where etalon_id = :petalon_id order by thres_val";
		queryEtalon->SQL->Text = strSQL;
		queryEtalon->Parameters->ParamByName("petalon_id")->Value = etId;
		// ��������� ���� �������
		vector<double>BarkValues;
		BarkValues = lCardData->GetBarkValues(Thresholds);
		// ������� � ����
		queryEtalon->Open();
		queryEtalon->First();
		int ii = 0;
		for (int i = 0; i < Thresholds.size(); i++) {
			strCmdSql = "UPDATE EtalonValues SET barkgausen_val = :pbarkgausen_val";
			// cmdEtalonVal->Parameters->ParamByName("pbarkgausen_val")->Value = 222;
			// double tst = BarkValues[i];
			cmdEtalonVal->Parameters->ParamByName("pbarkgausen_val")->Value = BarkValues[i];
			// strCmdSql+= FloatToStr(BarkValues[i]);
			strCmdSql += " WHERE rec_id=";
			strCmdSql += IntToStr(queryEtalon->FieldByName("vrec_id")->AsInteger);
			cmdEtalonVal->CommandText = strCmdSql;
			// cmdEtalonVal->Parameters->ParamByName("prec_id")->Value = queryEtalonVal->FieldByName("vrec_id")->AsInteger;
			// cmdEtalonVal->Parameters->ParamByName("pbarkgausen_val")->Value = &BarkValues[i];
			cmdEtalonVal->Execute();
			queryEtalon->Next();
		}
		queryEtalon->Close();
		int freqNum = lCardData->getFreqNum();
		int arrSize = lCardData->vecMeasuresData[freqNum].vecSensorsData[0].size()*3;
		//int arrSize = lCardData->vecMeasure.size();
		vector<double> forSave;
		for(int i = 0; i<lCardData->vecMeasuresData[0].vecSensorsData[0].size();i++)
		{
		   forSave.push_back(lCardData->vecMeasuresData[freqNum].vecSensorsData[0][i]);
		   forSave.push_back(lCardData->vecMeasuresData[freqNum].vecSensorsData[1][i]);
		   forSave.push_back(lCardData->vecMeasuresData[freqNum].vecSensorsData[2][i]);
		}
		TLog::SaveChTxtDoubleFile(fileName, &(forSave[0]) , arrSize,3); //serg
		MessageDlg("������� ��������", mtConfirmation, TMsgDlgButtons() << mbOK, NULL);
		err = 0;
		//
		// queryTSz->Close();
		// queryTSz->Parameters->ParamByName("pTypeSize")->Value = (int)cbTypeSize->Items->Objects[ind];
		// queryTSz->Open();

	}
	catch (Exception *ex) {
		TLog::ErrFullSaveLog(ex);
		MessageDlg(ex->Message, mtError, TMsgDlgButtons() << mbOK, NULL);
	}
}

// ---------------------------------------------------------------------------

void __fastcall TMainForm::FormResize(TObject *Sender) {
	// Panel1->Height=Panel2->Top-Panel1->Top;
	// SignalChart->Height=Panel1->Height-SignalChart->MarginBottom-SignalChart->MarginTop;
}

// ---------------------------------------------------------------------------
// ��������� ������
void TMainForm::SetAbleButtons(bool _enable) {
	MainMenu->Items->Find("���������")->Enabled = _enable;
	MainMenu->Items->Find("�����������")->Enabled = _enable;
	MainMenu->Items->Find("���������")->Enabled = _enable;
	MainMenu->Items->Find("������")->Enabled = _enable;
 //	GroupBoxNGr->Visible = _enable;
	bStart->Enabled = _enable;
	cbTypeSize->Enabled = _enable;
	cbSGGost->Enabled = _enable;
}

void __fastcall TMainForm::SplitterMidMoved(TObject *Sender) {
	PanelChartTop->Height = SplitterMid->Top;
	PanelChartBottom->Top = SplitterMid->Top + SplitterMid->Height;
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::FormActivate(TObject *Sender) {

	// if(lastError<0){
	// this->Close();
	// return;
	// }else{
	// //
	// }
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::SplitterResMoved(TObject *Sender) {
	// PanelSG->Width=SplitterRes->Left+SplitterRes->Width;
	PanelSG->Left = SplitterRes->Left + SplitterRes->Width;
}
// ---------------------------------------------------------------------------
void __fastcall TMainForm::cbTypeSizeChange(TObject *Sender)
{
   	int ind = cbTypeSize->ItemIndex;
	mainGlobalSettings.indexCurrentTypeSize = (int)cbTypeSize->Items->Objects[ind];
	SqlDBModule->UpdIntSql("SettingsGlobal", "indexCurrentTypeSize", mainGlobalSettings.indexCurrentTypeSize, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::menuExtSetClick(TObject *Sender)
{
 fmExtSettings=new TfmExtSettings(this,&mainGlobalSettings);
 fmExtSettings->ShowModal();
 delete fmExtSettings;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
 	if (mainGlobalSettings.isWork) {
		lastMsg = "������� ������� ������!!!";
		TExtFunction::ShowBigModalMessage(lastMsg, clRed);
        CanClose=false;
		return;
	}
	else {
		if (threadWork != NULL) {
				threadWork->Terminate();
				threadWork->WaitFor();
				threadWork->TestExitLoop();
				ThreadWork *x = threadWork;
				threadWork = NULL;
				delete x;
		}
		if(NULL != gen )
		{
			gen->Stop();
			TGSPF052* x = gen;
			gen = NULL;
			Sleep(1000);
			delete x;
			Sleep(1000);
		}
	}
}
void TMainForm::EnableWigits(bool b)
{
	GroupBoxNGr->Visible = b;
	PanelChartBottom->Visible = b;
	menuSettings->Visible = b;
	menuSGSett->Visible = b;
	menuColorSett->Visible = b;
	menuTypeSize->Visible = b;
	menuEtalons->Visible = b;
	menuExtSet->Visible = b;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N1Click(TObject *Sender)
{
	  if(!GroupBoxNGr->Visible)
	  {
		TPasswordForm *f = new TPasswordForm(this);
		f->ShowModal();
		if(f->result)
		{
		   EnableWigits(true);
		}
	  }
	  else
	  {
		   EnableWigits(false);
	  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnExitClick(TObject *Sender)
{
	 Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnAbautClick(TObject *Sender)
{
   TAboutBox *a = new TAboutBox((TComponent *)Sender);
   a->ShowModal();
   a->Free();
}
//---------------------------------------------------------------------------

