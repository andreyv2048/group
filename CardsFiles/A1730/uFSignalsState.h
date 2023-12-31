//---------------------------------------------------------------------------

#ifndef uFSignalsStateH
#define uFSignalsStateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//#include <IniFiles.hpp>
#include <vcl.h>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "SignalList.h"
#include "TGlobalSettings.h"
//---------------------------------------------------------------------------
class TFSignalsState : public TForm
{
__published:	// IDE-managed Components
	TTimer *InOutTimer;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall InOutTimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
	void CheckSignals(void);
	//TIniFile *ini;
	SignalList* SL;
	TGlobalSettings* fsGlobalSettings;
public:		// User declarations
	//__fastcall TFSignalsState(TComponent* Owner,TIniFile* _ini,SignalList* _SL);
	__fastcall TFSignalsState(TComponent* Owner,  TGlobalSettings* _mainGlobalSettings,SignalList* _SL);

};
//---------------------------------------------------------------------------
extern PACKAGE TFSignalsState *FSignalsState;
//---------------------------------------------------------------------------
#endif

