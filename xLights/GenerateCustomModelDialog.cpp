#include "GenerateCustomModelDialog.h"
#include "effects/VideoPanel.h"

#include <map>
#include <list>

//(*InternalHeaders(GenerateCustomModelDialog)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/msgdlg.h>
#include <wx/file.h>
#include <wx/dcmemory.h>
#include <wx/log.h>

#include "xLightsMain.h"
#include <log4cpp/Category.hh>

#define GCM_DISPLAYIMAGEWIDTH 400
#define GCM_DISPLAYIMAGEHEIGHT 300

#define PAGE_PREPARE 0
#define PAGE_GENERATE 1

#define PAGE_MODELTYPE 0
#define PAGE_CHOOSEVIDEO 1
#define PAGE_STARTFRAME 2
#define PAGE_BULBIDENTIFY 3
#define PAGE_REVIEWMODEL 4

#define STARTSCANSECS 15
#define FRAMEMS 50
#define LEADOFF 3000
#define LEADON 500
#define FLAGON 500
#define FLAGOFF 500
#define NODEON 500
#define NODEOFF 200
//#define DELAYMSUNTILSAMPLE 100
#define DELAYMSUNTILSAMPLE 0

#pragma region Flicker Free Static Bitmap

class MyGenericStaticBitmap : public wxGenericStaticBitmap {
    bool _doerasebackground;
public:
    MyGenericStaticBitmap(wxWindow *parent,
        wxWindowID id,
        const wxBitmap& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticBitmapNameStr)
        : wxGenericStaticBitmap(parent, id, bitmap, pos, size, style, name)
    {
        _doerasebackground = true;
    };
    void OnEraseBackGround(wxEraseEvent& event)
    {
        if (_doerasebackground)
        {
            event.Skip();
        }
    };
    void SetEraseBackground(bool f)
    {
        _doerasebackground = f;
    }
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyGenericStaticBitmap, wxGenericStaticBitmap)
EVT_ERASE_BACKGROUND(MyGenericStaticBitmap::OnEraseBackGround)
END_EVENT_TABLE()

#pragma endregion Flicker Free Static Bitmap

#pragma region Constructor

const long GenerateCustomModelDialog::ID_STATICBITMAP_Preview = wxNewId();

//(*IdInit(GenerateCustomModelDialog)
const long GenerateCustomModelDialog::ID_RADIOBOX1 = wxNewId();
const long GenerateCustomModelDialog::ID_SPINCTRL_NC_Count = wxNewId();
const long GenerateCustomModelDialog::ID_SPINCTRL_StartChannel = wxNewId();
const long GenerateCustomModelDialog::ID_SLIDER_Intensity = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_PCM_Run = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_Prepare = wxNewId();
const long GenerateCustomModelDialog::ID_RADIOBOX2 = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_MT_Next = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL1 = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT10 = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_GCM_Filename = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_GCM_SelectFile = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_CV_Back = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_CV_Next = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_ChooseVideo = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT3 = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Back1Frame = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Forward1Frame = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Back10Frames = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Forward10Frames = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT_StartFrameOk = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT_StartTime = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_SF_Back = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_SF_Next = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_StartFrame = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT5 = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT1 = wxNewId();
const long GenerateCustomModelDialog::ID_SLIDER_AdjustBlur = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_BC_Blur = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT8 = wxNewId();
const long GenerateCustomModelDialog::ID_SLIDER_BI_Sensitivity = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_BI_Sensitivity = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT6 = wxNewId();
const long GenerateCustomModelDialog::ID_SLIDER_BI_MinSeparation = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_BI_MinSeparation = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT2 = wxNewId();
const long GenerateCustomModelDialog::ID_SLIDER_BI_Contrast = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_BI_Contrast = wxNewId();
const long GenerateCustomModelDialog::ID_CHECKBOX_BI_IsSteady = wxNewId();
const long GenerateCustomModelDialog::ID_CHECKBOX_BI_ManualUpdate = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_BI_Update = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_CB_RestoreDefault = wxNewId();
const long GenerateCustomModelDialog::ID_TEXTCTRL_BI_Status = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_BI_Back = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_BI_Next = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_BulbIdentify = wxNewId();
const long GenerateCustomModelDialog::ID_STATICTEXT9 = wxNewId();
const long GenerateCustomModelDialog::ID_GRID_CM_Result = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Shrink = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_Grow = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_CM_Back = wxNewId();
const long GenerateCustomModelDialog::ID_BUTTON_CM_Save = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_CustomModel = wxNewId();
const long GenerateCustomModelDialog::ID_AUINOTEBOOK_ProcessSettings = wxNewId();
const long GenerateCustomModelDialog::ID_PANEL_Generate = wxNewId();
const long GenerateCustomModelDialog::ID_AUINOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(GenerateCustomModelDialog,wxDialog)
	//(*EventTable(GenerateCustomModelDialog)
	//*)
END_EVENT_TABLE()

GenerateCustomModelDialog::GenerateCustomModelDialog(wxWindow* parent, wxXmlDocument* network, wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    _network = network;
    _busy = false;

	//(*Initialize(GenerateCustomModelDialog)
	wxFlexGridSizer* FlexGridSizer16;
	wxFlexGridSizer* FlexGridSizer24;
	wxFlexGridSizer* FlexGridSizer23;
	wxStaticText* StaticText2;
	wxFlexGridSizer* FlexGridSizer10;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticText* StaticText6;
	wxFlexGridSizer* FlexGridSizer27;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer25;
	wxFlexGridSizer* FlexGridSizer22;
	wxStaticText* StaticText8;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText1;
	wxStaticText* StaticText3;
	wxFlexGridSizer* FlexGridSizer7;
	wxStaticText* StaticText5;
	wxFlexGridSizer* FlexGridSizer15;
	wxStaticText* StaticText7;
	wxFlexGridSizer* FlexGridSizer18;
	wxFlexGridSizer* FlexGridSizer21;
	wxFlexGridSizer* FlexGridSizer14;
	wxFlexGridSizer* FlexGridSizer20;
	wxFlexGridSizer* FlexGridSizer12;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer11;
	wxFlexGridSizer* FlexGridSizer17;
	wxStaticText* StaticText4;
	wxFlexGridSizer* FlexGridSizer26;

	Create(parent, id, _("Generate Custom Models"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(1, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	AuiNotebook1 = new wxAuiNotebook(this, ID_AUINOTEBOOK1, wxDefaultPosition, wxSize(1000,500), wxTAB_TRAVERSAL);
	Panel_Prepare = new wxPanel(AuiNotebook1, ID_PANEL_Prepare, wxPoint(63,54), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_Prepare"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	StaticText1 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Use this page to generate the lights that you need to video before you can proceed to the processing tab to create your custom model."), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL, 2);
	FlexGridSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Using a steady video camera to record a clear video showing all the lights running this test pattern."), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText7 = new wxStaticText(Panel_Prepare, wxID_ANY, _("The test pattern must start within the first 30 seconds of the video."), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer2->Add(StaticText7, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer6->AddGrowableCol(1);
	StaticText4 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Model details:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	wxFont StaticText4Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText4->SetFont(StaticText4Font);
	FlexGridSizer6->Add(StaticText4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer6->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Nodes"),
		_("Single Channels")
	};
	RadioBox1 = new wxRadioBox(Panel_Prepare, ID_RADIOBOX1, _("Type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	FlexGridSizer6->Add(RadioBox1, 1, wxALL|wxEXPAND, 2);
	StaticText5 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Node/Channel Count"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer6->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	SpinCtrl_NC_Count = new wxSpinCtrl(Panel_Prepare, ID_SPINCTRL_NC_Count, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 99999, 1, _T("ID_SPINCTRL_NC_Count"));
	SpinCtrl_NC_Count->SetValue(_T("1"));
	FlexGridSizer6->Add(SpinCtrl_NC_Count, 1, wxALL|wxEXPAND, 2);
	StaticText6 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Start Channel"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer6->Add(StaticText6, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	SpinCtrl_StartChannel = new wxSpinCtrl(Panel_Prepare, ID_SPINCTRL_StartChannel, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 99999, 1, _T("ID_SPINCTRL_StartChannel"));
	SpinCtrl_StartChannel->SetValue(_T("1"));
	FlexGridSizer6->Add(SpinCtrl_StartChannel, 1, wxALL|wxEXPAND, 2);
	StaticText8 = new wxStaticText(Panel_Prepare, wxID_ANY, _("Intensity"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer6->Add(StaticText8, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Intensity = new wxSlider(Panel_Prepare, ID_SLIDER_Intensity, 255, 30, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Intensity"));
	FlexGridSizer6->Add(Slider_Intensity, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer2->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_PCM_Run = new wxButton(Panel_Prepare, ID_BUTTON_PCM_Run, _("Run Capture Pattern"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_PCM_Run"));
	FlexGridSizer2->Add(Button_PCM_Run, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel_Prepare->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(Panel_Prepare);
	FlexGridSizer2->SetSizeHints(Panel_Prepare);
	Panel_Generate = new wxPanel(AuiNotebook1, ID_PANEL_Generate, wxPoint(59,17), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_Generate"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(0);
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	FlexGridSizer5->AddGrowableRow(0);
	FlexGridSizer14 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer14->AddGrowableCol(0);
	FlexGridSizer14->AddGrowableRow(0);
	FlexGridSizer5->Add(FlexGridSizer14, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer7 = new wxFlexGridSizer(1, 1, 0, 0);
	FlexGridSizer7->AddGrowableCol(0);
	FlexGridSizer7->AddGrowableRow(0);
	AuiNotebook_ProcessSettings = new wxAuiNotebook(Panel_Generate, ID_AUINOTEBOOK_ProcessSettings, wxDefaultPosition, wxDefaultSize, 0);
	Panel1 = new wxPanel(AuiNotebook_ProcessSettings, ID_PANEL1, wxPoint(73,8), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer24 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer24->AddGrowableCol(0);
	FlexGridSizer24->AddGrowableRow(1);
	FlexGridSizer25 = new wxFlexGridSizer(0, 3, 0, 0);
	wxString __wxRadioBoxChoices_2[3] =
	{
		_("Nodes"),
		_("Single Channels"),
		_("Static Lights")
	};
	RadioBox2 = new wxRadioBox(Panel1, ID_RADIOBOX2, _("Type"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_2, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
	FlexGridSizer25->Add(RadioBox2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer24->Add(FlexGridSizer25, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer24->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer26 = new wxFlexGridSizer(0, 3, 0, 0);
	Button_MT_Next = new wxButton(Panel1, ID_BUTTON_MT_Next, _("Next"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_MT_Next"));
	FlexGridSizer26->Add(Button_MT_Next, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer24->Add(FlexGridSizer26, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(FlexGridSizer24);
	FlexGridSizer24->Fit(Panel1);
	FlexGridSizer24->SetSizeHints(Panel1);
	Panel_ChooseVideo = new wxPanel(AuiNotebook_ProcessSettings, ID_PANEL_ChooseVideo, wxPoint(18,15), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_ChooseVideo"));
	FlexGridSizer21 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer21->AddGrowableCol(0);
	FlexGridSizer21->AddGrowableRow(2);
	StaticText_CM_Request = new wxStaticText(Panel_ChooseVideo, ID_STATICTEXT10, _("Insert text here"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer21->Add(StaticText_CM_Request, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer22 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer22->AddGrowableCol(1);
	StaticText3 = new wxStaticText(Panel_ChooseVideo, wxID_ANY, _("Media File"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer22->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	TextCtrl_GCM_Filename = new wxTextCtrl(Panel_ChooseVideo, ID_TEXTCTRL_GCM_Filename, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_GCM_Filename"));
	FlexGridSizer22->Add(TextCtrl_GCM_Filename, 1, wxALL|wxEXPAND, 2);
	Button_GCM_SelectFile = new wxButton(Panel_ChooseVideo, ID_BUTTON_GCM_SelectFile, _("..."), wxDefaultPosition, wxSize(29,28), 0, wxDefaultValidator, _T("ID_BUTTON_GCM_SelectFile"));
	FlexGridSizer22->Add(Button_GCM_SelectFile, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer21->Add(FlexGridSizer22, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer21->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer23 = new wxFlexGridSizer(0, 3, 0, 0);
	Button_CV_Back = new wxButton(Panel_ChooseVideo, ID_BUTTON_CV_Back, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CV_Back"));
	FlexGridSizer23->Add(Button_CV_Back, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	Button_CV_Next = new wxButton(Panel_ChooseVideo, ID_BUTTON_CV_Next, _("Next"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CV_Next"));
	FlexGridSizer23->Add(Button_CV_Next, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer21->Add(FlexGridSizer23, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel_ChooseVideo->SetSizer(FlexGridSizer21);
	FlexGridSizer21->Fit(Panel_ChooseVideo);
	FlexGridSizer21->SetSizeHints(Panel_ChooseVideo);
	Panel_StartFrame = new wxPanel(AuiNotebook_ProcessSettings, ID_PANEL_StartFrame, wxPoint(43,126), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_StartFrame"));
	FlexGridSizer10 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer10->AddGrowableCol(0);
	FlexGridSizer10->AddGrowableRow(4);
	StaticText11 = new wxStaticText(Panel_StartFrame, ID_STATICTEXT3, _("This is the frame the scan has identified as being the most likely to show all your bulbs. It should be a frame showing all the bulbs on and it should be near the start of the first flash of all the bulbs before each bulb was lit in turn.\nYou can move it forward or backwards with the buttons below.\n\nClick next when you are happy with it."), wxDefaultPosition, wxSize(658,99), 0, _T("ID_STATICTEXT3"));
	FlexGridSizer10->Add(StaticText11, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer11 = new wxFlexGridSizer(0, 2, 0, 0);
	Button_Back1Frame = new wxButton(Panel_StartFrame, ID_BUTTON_Back1Frame, _("Back 1 Frame"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_Back1Frame"));
	FlexGridSizer11->Add(Button_Back1Frame, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Forward1Frame = new wxButton(Panel_StartFrame, ID_BUTTON_Forward1Frame, _("Forward 1 Frame"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_Forward1Frame"));
	FlexGridSizer11->Add(Button_Forward1Frame, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Back10Frames = new wxButton(Panel_StartFrame, ID_BUTTON_Back10Frames, _("Back 10 Frames"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_Back10Frames"));
	FlexGridSizer11->Add(Button_Back10Frames, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Forward10Frames = new wxButton(Panel_StartFrame, ID_BUTTON_Forward10Frames, _("Forward 10 Frames"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_Forward10Frames"));
	FlexGridSizer11->Add(Button_Forward10Frames, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer10->Add(FlexGridSizer11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	StaticText_StartFrameOk = new wxStaticText(Panel_StartFrame, ID_STATICTEXT_StartFrameOk, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT_StartFrameOk"));
	wxFont StaticText_StartFrameOkFont(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText_StartFrameOk->SetFont(StaticText_StartFrameOkFont);
	FlexGridSizer10->Add(StaticText_StartFrameOk, 1, wxALL|wxEXPAND, 2);
	StaticText_StartTime = new wxStaticText(Panel_StartFrame, ID_STATICTEXT_StartTime, wxEmptyString, wxDefaultPosition, wxSize(95,16), 0, _T("ID_STATICTEXT_StartTime"));
	FlexGridSizer10->Add(StaticText_StartTime, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer10->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer12 = new wxFlexGridSizer(0, 3, 0, 0);
	Button_SF_Back = new wxButton(Panel_StartFrame, ID_BUTTON_SF_Back, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SF_Back"));
	FlexGridSizer12->Add(Button_SF_Back, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_SF_Next = new wxButton(Panel_StartFrame, ID_BUTTON_SF_Next, _("Next"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SF_Next"));
	FlexGridSizer12->Add(Button_SF_Next, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer10->Add(FlexGridSizer12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel_StartFrame->SetSizer(FlexGridSizer10);
	FlexGridSizer10->Fit(Panel_StartFrame);
	FlexGridSizer10->SetSizeHints(Panel_StartFrame);
	Panel_BulbIdentify = new wxPanel(AuiNotebook_ProcessSettings, ID_PANEL_BulbIdentify, wxPoint(176,18), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_BulbIdentify"));
	FlexGridSizer15 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer15->AddGrowableCol(0);
	FlexGridSizer15->AddGrowableRow(5);
	StaticText_BI = new wxStaticText(Panel_BulbIdentify, ID_STATICTEXT5, _("The red circles on the image show the bulbs we have identify. Adjust the sensitivity if there are bulbs missing or phantom bulbs identified.\n\nClick next when you are happy that all bulbs have been detected."), wxDefaultPosition, wxSize(652,75), 0, _T("ID_STATICTEXT5"));
	FlexGridSizer15->Add(StaticText_BI, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer16 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer16->AddGrowableCol(1);
	StaticText9 = new wxStaticText(Panel_BulbIdentify, ID_STATICTEXT1, _("Blur"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer16->Add(StaticText9, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_AdjustBlur = new wxSlider(Panel_BulbIdentify, ID_SLIDER_AdjustBlur, 1, 1, 30, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_AdjustBlur"));
	FlexGridSizer16->Add(Slider_AdjustBlur, 1, wxALL|wxEXPAND, 2);
	TextCtrl_BC_Blur = new wxTextCtrl(Panel_BulbIdentify, ID_TEXTCTRL_BC_Blur, _("1"), wxDefaultPosition, wxSize(40,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_BC_Blur"));
	FlexGridSizer16->Add(TextCtrl_BC_Blur, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText_BI_Slider = new wxStaticText(Panel_BulbIdentify, ID_STATICTEXT8, _("Sensitivity"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer16->Add(StaticText_BI_Slider, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_BI_Sensitivity = new wxSlider(Panel_BulbIdentify, ID_SLIDER_BI_Sensitivity, 127, 0, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_BI_Sensitivity"));
	FlexGridSizer16->Add(Slider_BI_Sensitivity, 1, wxALL|wxEXPAND, 2);
	TextCtrl_BI_Sensitivity = new wxTextCtrl(Panel_BulbIdentify, ID_TEXTCTRL_BI_Sensitivity, _("127"), wxDefaultPosition, wxSize(40,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_BI_Sensitivity"));
	FlexGridSizer16->Add(TextCtrl_BI_Sensitivity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText13 = new wxStaticText(Panel_BulbIdentify, ID_STATICTEXT6, _("Minimum Separation"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer16->Add(StaticText13, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_BI_MinSeparation = new wxSlider(Panel_BulbIdentify, ID_SLIDER_BI_MinSeparation, 10, 1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_BI_MinSeparation"));
	FlexGridSizer16->Add(Slider_BI_MinSeparation, 1, wxALL|wxEXPAND, 2);
	TextCtrl_BI_MinSeparation = new wxTextCtrl(Panel_BulbIdentify, ID_TEXTCTRL_BI_MinSeparation, _("10"), wxDefaultPosition, wxSize(40,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_BI_MinSeparation"));
	FlexGridSizer16->Add(TextCtrl_BI_MinSeparation, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText10 = new wxStaticText(Panel_BulbIdentify, ID_STATICTEXT2, _("Contrast"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer16->Add(StaticText10, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_BI_Contrast = new wxSlider(Panel_BulbIdentify, ID_SLIDER_BI_Contrast, 0, -255, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_BI_Contrast"));
	FlexGridSizer16->Add(Slider_BI_Contrast, 1, wxALL|wxEXPAND, 2);
	TextCtrl_BI_Contrast = new wxTextCtrl(Panel_BulbIdentify, ID_TEXTCTRL_BI_Contrast, _("0"), wxDefaultPosition, wxSize(40,24), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_BI_Contrast"));
	FlexGridSizer16->Add(TextCtrl_BI_Contrast, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer15->Add(FlexGridSizer16, 1, wxALL|wxEXPAND, 2);
	CheckBox_BI_IsSteady = new wxCheckBox(Panel_BulbIdentify, ID_CHECKBOX_BI_IsSteady, _("Video is steady"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_BI_IsSteady"));
	CheckBox_BI_IsSteady->SetValue(true);
	FlexGridSizer15->Add(CheckBox_BI_IsSteady, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	CheckBox_BI_ManualUpdate = new wxCheckBox(Panel_BulbIdentify, ID_CHECKBOX_BI_ManualUpdate, _("Manual Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_BI_ManualUpdate"));
	CheckBox_BI_ManualUpdate->SetValue(true);
	FlexGridSizer15->Add(CheckBox_BI_ManualUpdate, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer27 = new wxFlexGridSizer(0, 2, 0, 0);
	Button_BI_Update = new wxButton(Panel_BulbIdentify, ID_BUTTON_BI_Update, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BI_Update"));
	FlexGridSizer27->Add(Button_BI_Update, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_CB_RestoreDefault = new wxButton(Panel_BulbIdentify, ID_BUTTON_CB_RestoreDefault, _("Restore Default"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CB_RestoreDefault"));
	FlexGridSizer27->Add(Button_CB_RestoreDefault, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer15->Add(FlexGridSizer27, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl_BI_Status = new wxTextCtrl(Panel_BulbIdentify, ID_TEXTCTRL_BI_Status, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL_BI_Status"));
	FlexGridSizer15->Add(TextCtrl_BI_Status, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer17 = new wxFlexGridSizer(0, 2, 0, 0);
	Button_BI_Back = new wxButton(Panel_BulbIdentify, ID_BUTTON_BI_Back, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BI_Back"));
	FlexGridSizer17->Add(Button_BI_Back, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	Button_BI_Next = new wxButton(Panel_BulbIdentify, ID_BUTTON_BI_Next, _("Next"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BI_Next"));
	FlexGridSizer17->Add(Button_BI_Next, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer15->Add(FlexGridSizer17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel_BulbIdentify->SetSizer(FlexGridSizer15);
	FlexGridSizer15->Fit(Panel_BulbIdentify);
	FlexGridSizer15->SetSizeHints(Panel_BulbIdentify);
	Panel_CustomModel = new wxPanel(AuiNotebook_ProcessSettings, ID_PANEL_CustomModel, wxPoint(259,19), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL_CustomModel"));
	FlexGridSizer18 = new wxFlexGridSizer(4, 1, 0, 0);
	FlexGridSizer18->AddGrowableCol(0);
	FlexGridSizer18->AddGrowableRow(1);
	StaticText17 = new wxStaticText(Panel_CustomModel, ID_STATICTEXT9, _("This is the new custom model. Click save to create a model file that you can then import into your layout."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer18->Add(StaticText17, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer19 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer19->AddGrowableCol(0);
	FlexGridSizer19->AddGrowableRow(0);
	Grid_CM_Result = new wxGrid(Panel_CustomModel, ID_GRID_CM_Result, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_GRID_CM_Result"));
	FlexGridSizer19->Add(Grid_CM_Result, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer18->Add(FlexGridSizer19, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer18->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer20 = new wxFlexGridSizer(0, 5, 0, 0);
	Button_Shrink = new wxButton(Panel_CustomModel, ID_BUTTON_Shrink, _("-"), wxDefaultPosition, wxSize(26,28), 0, wxDefaultValidator, _T("ID_BUTTON_Shrink"));
	FlexGridSizer20->Add(Button_Shrink, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Grow = new wxButton(Panel_CustomModel, ID_BUTTON_Grow, _("+"), wxDefaultPosition, wxSize(26,28), 0, wxDefaultValidator, _T("ID_BUTTON_Grow"));
	FlexGridSizer20->Add(Button_Grow, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer20->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_CM_Back = new wxButton(Panel_CustomModel, ID_BUTTON_CM_Back, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CM_Back"));
	FlexGridSizer20->Add(Button_CM_Back, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_CM_Save = new wxButton(Panel_CustomModel, ID_BUTTON_CM_Save, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CM_Save"));
	FlexGridSizer20->Add(Button_CM_Save, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer18->Add(FlexGridSizer20, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel_CustomModel->SetSizer(FlexGridSizer18);
	FlexGridSizer18->Fit(Panel_CustomModel);
	FlexGridSizer18->SetSizeHints(Panel_CustomModel);
	AuiNotebook_ProcessSettings->AddPage(Panel1, _("Model Type"));
	AuiNotebook_ProcessSettings->AddPage(Panel_ChooseVideo, _("Choose Media"));
	AuiNotebook_ProcessSettings->AddPage(Panel_StartFrame, _("Start Frame"));
	AuiNotebook_ProcessSettings->AddPage(Panel_BulbIdentify, _("Bulb Identify"));
	AuiNotebook_ProcessSettings->AddPage(Panel_CustomModel, _("Custom Model"));
	FlexGridSizer7->Add(AuiNotebook_ProcessSettings, 1, wxALL|wxEXPAND, 2);
	FlexGridSizer5->Add(FlexGridSizer7, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3->Add(FlexGridSizer5, 1, wxALL|wxEXPAND, 2);
	Panel_Generate->SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(Panel_Generate);
	FlexGridSizer3->SetSizeHints(Panel_Generate);
	AuiNotebook1->AddPage(Panel_Prepare, _("Prepare"), true);
	AuiNotebook1->AddPage(Panel_Generate, _("Process"));
	FlexGridSizer1->Add(AuiNotebook1, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE, 2);
	SetSizer(FlexGridSizer1);
	FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_PCM_Run,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_PCM_RunClick);
	Connect(ID_BUTTON_MT_Next,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_MT_NextClick);
	Connect(ID_TEXTCTRL_GCM_Filename,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnTextCtrl_GCM_FilenameText);
	Connect(ID_BUTTON_GCM_SelectFile,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_GCM_SelectFileClick);
	Connect(ID_BUTTON_CV_Back,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_CV_BackClick);
	Connect(ID_BUTTON_CV_Next,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_CV_NextClick);
	Connect(ID_BUTTON_Back1Frame,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_Back1FrameClick);
	Connect(ID_BUTTON_Forward1Frame,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_Forward1FrameClick);
	Connect(ID_BUTTON_Back10Frames,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_Back10FramesClick);
	Connect(ID_BUTTON_Forward10Frames,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_Forward10FramesClick);
	Connect(ID_BUTTON_SF_Back,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_SF_BackClick);
	Connect(ID_BUTTON_SF_Next,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_SF_NextClick);
	Connect(ID_SLIDER_AdjustBlur,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_AdjustBlurCmdScrollChanged);
	Connect(ID_SLIDER_AdjustBlur,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_AdjustBlurCmdScroll);
	Connect(ID_SLIDER_BI_Sensitivity,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_SensitivityCmdScrollChanged);
	Connect(ID_SLIDER_BI_Sensitivity,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_SensitivityCmdSliderUpdated);
	Connect(ID_SLIDER_BI_MinSeparation,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_MinSeparationCmdScrollChanged);
	Connect(ID_SLIDER_BI_MinSeparation,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_MinSeparationCmdSliderUpdated);
	Connect(ID_SLIDER_BI_Contrast,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_ContrastCmdScrollChanged);
	Connect(ID_SLIDER_BI_Contrast,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnSlider_BI_ContrastCmdSliderUpdated);
	Connect(ID_CHECKBOX_BI_IsSteady,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnCheckBox_BI_IsSteadyClick);
	Connect(ID_CHECKBOX_BI_ManualUpdate,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnCheckBox_BI_ManualUpdateClick);
	Connect(ID_BUTTON_BI_Update,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_BI_UpdateClick);
	Connect(ID_BUTTON_CB_RestoreDefault,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_BI_RestoreDefaultClick);
	Connect(ID_BUTTON_BI_Back,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_BI_BackClick);
	Connect(ID_BUTTON_BI_Next,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_BI_NextClick);
	Connect(ID_BUTTON_Shrink,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_ShrinkClick);
	Connect(ID_BUTTON_Grow,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_GrowClick);
	Connect(ID_BUTTON_CM_Back,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_CM_BackClick);
	Connect(ID_BUTTON_CM_Save,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&GenerateCustomModelDialog::OnButton_CM_SaveClick);
	Connect(ID_AUINOTEBOOK_ProcessSettings,wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING,(wxObjectEventFunction)&GenerateCustomModelDialog::OnAuiNotebook_ProcessSettingsPageChanging);
	Connect(wxEVT_SIZE,(wxObjectEventFunction)&GenerateCustomModelDialog::OnResize);
	//*)

    _displaybmp = wxImage(GCM_DISPLAYIMAGEWIDTH, GCM_DISPLAYIMAGEHEIGHT, true);

    StaticBitmap_Preview = new MyGenericStaticBitmap(Panel_Generate, ID_STATICBITMAP_Preview, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP_Preview"));
    StaticBitmap_Preview->SetScaleMode(wxStaticBitmapBase::ScaleMode::Scale_AspectFit);
    StaticBitmap_Preview->SetSizeHints(wxSize(400, 300), wxSize(400, 300));
    StaticBitmap_Preview->SetBitmap(_displaybmp);
    FlexGridSizer14->Insert(0, StaticBitmap_Preview, 1, wxALL | wxEXPAND, 2);
    FlexGridSizer14->Layout();
    FlexGridSizer5->Layout();

    FlexGridSizer19->Fit(Grid_CM_Result);
    FlexGridSizer19->SetSizeHints(Grid_CM_Result);
    FlexGridSizer19->Layout();

    StaticBitmap_Preview->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&GenerateCustomModelDialog::OnStaticBitmapLeftDown, 0, this);
    StaticBitmap_Preview->Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&GenerateCustomModelDialog::OnStaticBitmapLeftUp, 0, this);
    StaticBitmap_Preview->Connect(wxEVT_MOTION, (wxObjectEventFunction)&GenerateCustomModelDialog::OnStaticBitmapMouseMove, 0, this);
    StaticBitmap_Preview->Connect(wxEVT_LEAVE_WINDOW, (wxObjectEventFunction)&GenerateCustomModelDialog::OnStaticBitmapMouseLeave, 0, this);

    _vr = NULL;
    _draggingedge = -1;

    MTTabEntry();

    ValidateWindow();
}

GenerateCustomModelDialog::~GenerateCustomModelDialog()
{
	//(*Destroy(GenerateCustomModelDialog)
	//*)

    if (_vr != NULL)
    {
        delete _vr;
    }
}
#pragma endregion Constructor

#pragma region Global Functions

void GenerateCustomModelDialog::OnResize(wxSizeEvent& event)
{
    event.Skip();
}

void GenerateCustomModelDialog::ValidateWindow()
{
    if (_state == VideoProcessingStates::CHOOSE_VIDEO)
    {
        wxString file = TextCtrl_GCM_Filename->GetValue();
        if (wxFile::Exists(file))
        {
            TextCtrl_GCM_Filename->SetBackgroundColour(*wxWHITE);
            Button_CV_Next->Enable();
        }
        else
        {
            TextCtrl_GCM_Filename->SetBackgroundColour(*wxRED);
            Button_CV_Next->Disable();
        }
        Button_GCM_SelectFile->Enable();
    }
    else if (_state == VideoProcessingStates::CHOOSE_MODELTYPE)
    {
    }
    else if (_state == VideoProcessingStates::FINDING_START_FRAME)
    {
    }
    else if (_state == VideoProcessingStates::CIRCLING_BULBS)
    {
    }
    else if (_state == VideoProcessingStates::IDENTIFYING_BULBS)
    {
    }
    else if (_state == VideoProcessingStates::REVIEW_CUSTOM_MODEL)
    {
    }
}

#pragma endregion Global Functions

#pragma region Image Functions

inline unsigned char GetPixel(int x, int y, int w3, unsigned char* data) {
    return *(data + y*w3 + x * 3);
}

inline void SetPixel(int x, int y, int w3, unsigned char* data, unsigned char c) {
    *(data + y*w3 + x * 3) = c;
}

wxImage GenerateCustomModelDialog::CreateImageFromFrame(AVFrame* frame)
{
    if (frame != NULL)
    {
        wxImage img(frame->width, frame->height, (unsigned char *)frame->data[0], true);
        img.SetType(wxBitmapType::wxBITMAP_TYPE_BMP);

        return img;
    }
    else
    {
        return wxImage(_startFrame.GetWidth(), _startFrame.GetHeight(), true);
    }
}

void GenerateCustomModelDialog::ShowImage(const wxImage& image)
{
    if (image.IsOk())
    {
        _displaybmp = image.Scale(GCM_DISPLAYIMAGEWIDTH, GCM_DISPLAYIMAGEHEIGHT, wxImageResizeQuality::wxIMAGE_QUALITY_HIGH);
    }
    StaticBitmap_Preview->SetBitmap(_displaybmp);
    wxYield();
}

void GenerateCustomModelDialog::ShowFrame(int time)
{
    wxImage image = CreateImageFromFrame(_vr->GetNextFrame(time));
    ShowImage(image);
}
#pragma endregion Image Functions

// ***********************************************************
//
// Prepare tab methods
//
// ***********************************************************

#pragma region Prepare

void GenerateCustomModelDialog::SetBulbs(bool nodes, int count, int startch, int node, int ms, int intensity, xOutput* xout)
{
    // node is out of range ... odd
    if (node > count)
    {
        return;
    }

    wxTimeSpan ts = wxDateTime::UNow() - _starttime;
    long curtime = ts.GetMilliseconds().ToLong();
    xout->TimerStart(curtime);

    if (node != -1)
    {
        if (nodes)
        {
            for (int j = 0; j < count; j++)
            {
                if (node == j)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        xout->SetIntensity(startch + j * 3 + i - 1, intensity);
                    }
                }
                else
                {
                    for (int i = 0; i < 3; i++)
                    {
                        xout->SetIntensity(startch + j * 3 + i - 1, 0);
                    }
                }
            }
        }
        else
        {
            for (int j = 0; j < count; j++)
            {
                if (j == node)
                {
                    xout->SetIntensity(startch + j - 1, intensity);
                }
                else
                {
                    xout->SetIntensity(startch + j - 1, 0);
                }
            }
        }
    }
    else
    {
        if (nodes)
        {
            for (int j = 0; j < count; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    xout->SetIntensity(startch + j * 3 + i - 1, intensity);
                }
            }
        }
        else
        {
            for (int j = 0; j < count; j++)
            {
                xout->SetIntensity(startch + j - 1, intensity);
            }
        }
    }

    xout->TimerEnd();

    wxTimeSpan tsx = wxDateTime::UNow() - _starttime;
    long curtimex = tsx.GetMilliseconds().ToLong();
    while (curtimex - curtime < ms)
    {
        wxYield();
        wxMicroSleep(5000);
        tsx = wxDateTime::UNow() - _starttime;
        curtimex = tsx.GetMilliseconds().ToLong();
    }
}

bool GenerateCustomModelDialog::InitialiseOutputs(xOutput* xout)
{
    long MaxChan;
    bool ok = true;

    for (wxXmlNode* e = _network->GetRoot()->GetChildren(); e != NULL && ok; e = e->GetNext())
    {
        wxString tagname = e->GetName();
        if (tagname == "network")
        {
            wxString tempstr = e->GetAttribute("MaxChannels", "0");
            tempstr.ToLong(&MaxChan);
            wxString NetworkType = e->GetAttribute("NetworkType", "");
            wxString ComPort = e->GetAttribute("ComPort", "");
            wxString BaudRate = e->GetAttribute("BaudRate", "");
            int baud = (BaudRate == _("n/a")) ? 115200 : wxAtoi(BaudRate);
            bool enabled = e->GetAttribute("Enabled", "Yes") == "Yes";
            wxString Description = e->GetAttribute("Description", "");
            static wxString choices;

            int numU = wxAtoi(e->GetAttribute("NumUniverses", "1"));

#ifdef __WXMSW__ //TODO: enumerate comm ports on all platforms -DJ
            TCHAR valname[32];
            /*byte*/TCHAR portname[32];
            DWORD vallen = sizeof(valname);
            DWORD portlen = sizeof(portname);
            HKEY hkey = NULL;
            DWORD err = 0;

            //enum serial comm ports (more user friendly, especially if USB-to-serial ports change):
            //logic based on http://www.cplusplus.com/forum/windows/73821/
            if (choices.empty()) //should this be cached?  it's not really that expensive
            {
                if (!(err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hkey)))
                    for (DWORD inx = 0; !(err = RegEnumValue(hkey, inx, (LPTSTR)valname, &vallen, NULL, NULL, (LPBYTE)portname, &portlen)) || (err == ERROR_MORE_DATA); ++inx)
                    {
                        if (err == ERROR_MORE_DATA) portname[sizeof(portname) / sizeof(portname[0]) - 1] = '\0'; //need to enlarge read buf if this happens; just truncate string for now
                                                                                                                 //                            debug(3, "found port[%d] %d:'%s' = %d:'%s', err 0x%x", inx, vallen, valname, portlen, portname, err);
                        choices += _(", ") + portname;
                        vallen = sizeof(valname);
                        portlen = sizeof(portname);
                    }
                if (err && (err != /*ERROR_FILE_NOT_FOUND*/ ERROR_NO_MORE_ITEMS)) choices = wxString::Format(", error %d (can't get serial comm ports from registry)", err);
                if (hkey) RegCloseKey(hkey);
                //                    if (err) SetLastError(err); //tell caller about last real error
                if (!choices.empty()) choices = "\n(available ports: " + choices.substr(2) + ")";
                else choices = "\n(no available ports)";
            }
#endif // __WXMSW__
            wxString msg = _("Error occurred while connecting to ") + NetworkType + _(" network on ") + ComPort +
                choices +
                _("\n\nThings to check:\n1. Are all required cables plugged in?") +
                _("\n2. Is there another program running that is accessing the port (like the LOR Control Panel)? If so, then you must close the other program and then restart xLights.") +
                _("\n3. If this is a USB dongle, are the FTDI Virtual COM Port drivers loaded?\n\n");

            try
            {
                xout->addnetwork(NetworkType, MaxChan, ComPort, baud, numU, enabled);
            }
            catch (const char *str)
            {
                wxString errmsg(str, wxConvUTF8);
                if (wxMessageBox(msg + errmsg + _("\nProceed anyway?"), _("Communication Error"), wxYES_NO | wxNO_DEFAULT) != wxYES)
                    ok = false;
            }
        }
    }
    return ok;
}

void GenerateCustomModelDialog::OnButton_PCM_RunClick(wxCommandEvent& event)
{
    wxMessageBox("Please prepare to video the model ... press ok when ready to start.", "", 5L, this);

    log4cpp::Category &logger_pcm = log4cpp::Category::getInstance(std::string("log_prepcustommodel"));
    logger_pcm.info("Running lights to be videod.");

    wxProgressDialog pd("Running light patterns", "", 100, this);

    int count = SpinCtrl_NC_Count->GetValue();
    int startch = SpinCtrl_StartChannel->GetValue();
    int intensity = Slider_Intensity->GetValue();
    bool nodes = RadioBox1->GetSelection() == 0;

    logger_pcm.info("   Count: %d.", count);
    logger_pcm.info("   Start Channel: %d.", startch);
    logger_pcm.info("   Intensity: %d.", intensity);
    if (nodes)
    {
        logger_pcm.info("   Nodes.");
    }
    else
    {
        logger_pcm.info("   Channels.");
    }


    DisableSleepModes();

    _starttime = wxDateTime::UNow();
    xOutput* xout = new xOutput();
    InitialiseOutputs(xout);

    int totaltime = LEADOFF + LEADON + FLAGOFF + FLAGON + FLAGOFF + count * (NODEON + NODEOFF) + FLAGOFF + FLAGON;

    // 3.0 seconds off 0.5 seconds on ... 0.5 seconds off ... 0.5 second on ... 0.5 seconds off
    SetBulbs(nodes, count, startch, -1, LEADOFF, 0, xout);
    UpdateProgress(pd, totaltime);
    SetBulbs(nodes, count, startch, -1, LEADON, intensity, xout);
    UpdateProgress(pd, totaltime);
    SetBulbs(nodes, count, startch, -1, FLAGOFF, 0, xout);
    UpdateProgress(pd, totaltime);
    SetBulbs(nodes, count, startch, -1, FLAGON, intensity, xout);
    UpdateProgress(pd, totaltime);
    SetBulbs(nodes, count, startch, -1, FLAGOFF, 0, xout);
    UpdateProgress(pd, totaltime);

    // then in turn each node on for 0.5 seconds ... all off for 0.2 seconds
    for (int i = 0; i < count; i++)
    {
        SetBulbs(nodes, count, startch, i, NODEON, intensity, xout);
        UpdateProgress(pd, totaltime);
        SetBulbs(nodes, count, startch, i, NODEOFF, 0, xout);
        UpdateProgress(pd, totaltime);
    }

    pd.Update(100);

    // then turn off for 0.5 seconds ... then on for 0.5 seconds then off
    SetBulbs(nodes, count, startch, -1, FLAGOFF, 0, xout);
    UpdateProgress(pd, totaltime);
    SetBulbs(nodes, count, startch, -1, FLAGON, intensity, xout);
    SetBulbs(nodes, count, startch, -1, 0, 0, xout);

    xout->alloff();
    delete xout;

    pd.Update(100);
    pd.Close();
    SetFocus();

    EnableSleepModes();

    logger_pcm.info("   Done.");

    wxMessageBox("Please stop the video.", "", 5L, this);
    ValidateWindow();
}

#pragma endregion Prepare

// ***********************************************************
//
// Generate tab methods
//
// ***********************************************************

#pragma region Generate

#pragma region Generate Tab General Methods

void GenerateCustomModelDialog::UpdateProgress(wxProgressDialog& pd, int totaltime)
{
    wxTimeSpan ts = wxDateTime::UNow() - _starttime;
    int progress = ts.GetMilliseconds().ToLong() * 100 / totaltime;
    if (progress > 100) progress = 100;
    pd.Update(progress);
}

void GenerateCustomModelDialog::OnAuiNotebook_ProcessSettingsPageChanging(wxAuiNotebookEvent& event)
{
    int page = event.GetSelection();
    if (_state == VideoProcessingStates::CHOOSE_MODELTYPE && page != PAGE_MODELTYPE)
    {
        event.Veto();
    }
    else if (_state == VideoProcessingStates::CHOOSE_VIDEO && page != PAGE_CHOOSEVIDEO)
    {
        event.Veto();
    }
    else if (_state == VideoProcessingStates::FINDING_START_FRAME && page != PAGE_STARTFRAME)
    {
        event.Veto();
    }
    else if (_state == VideoProcessingStates::IDENTIFYING_BULBS && page != PAGE_BULBIDENTIFY)
    {
        event.Veto();
    }
    else if (_state == VideoProcessingStates::REVIEW_CUSTOM_MODEL && page != PAGE_REVIEWMODEL)
    {
        event.Veto();
    }
}

void GenerateCustomModelDialog::SwapPage(int oldp, const int newp)
{
    AuiNotebook_ProcessSettings->ChangeSelection(newp);
}

#pragma endregion Generate Tab General Methods

// ***********************************************************
// Model Type tab methods
// ***********************************************************

#pragma region Model Type

void GenerateCustomModelDialog::MTTabEntry()
{
    _state = VideoProcessingStates::CHOOSE_MODELTYPE;
    _displaybmp = wxImage(GCM_DISPLAYIMAGEWIDTH, GCM_DISPLAYIMAGEHEIGHT, true);
    StaticBitmap_Preview->SetBitmap(_displaybmp);
    if (_vr != NULL)
    {
        delete _vr;
        _vr = NULL;
    }
    _draggingedge = -1;
}

void GenerateCustomModelDialog::OnButton_MT_NextClick(wxCommandEvent& event)
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    logger_gcm.info("Generating custom model.");
    switch (RadioBox2->GetSelection())
    {
    case 0:
        logger_gcm.info("   Nodes.");
        break;
    case 1:
        logger_gcm.info("   Channels.");
        break;
    case 2:
        logger_gcm.info("   Static Bulbs.");
        break;
    }
    TextCtrl_GCM_Filename->SetValue(""); // clear the filename in case the type has changed
    CVTabEntry();
    SwapPage(PAGE_MODELTYPE, PAGE_CHOOSEVIDEO);
    ValidateWindow();
}

#pragma endregion Model Type

// ***********************************************************
// Choose Video tab methods
// ***********************************************************

#pragma region Choose Video

void GenerateCustomModelDialog::CVTabEntry()
{
    Button_CV_Next->Enable();
    Button_CV_Back->Enable();
    Button_GCM_SelectFile->Enable();
    TextCtrl_GCM_Filename->Enable();

    if (RadioBox2->GetSelection() == 2)
    {
        StaticText_CM_Request->SetLabel("Select a picture of your static lights model.");
    }
    else
    {
        StaticText_CM_Request->SetLabel("Select the video you recorded of your model using the prepare tab.");
    }
    _state = VideoProcessingStates::CHOOSE_VIDEO;
    if (wxFile::Exists(std::string(TextCtrl_GCM_Filename->GetValue().c_str())))
    {
        if (RadioBox2->GetSelection() == 2)
        {
            _startFrame = wxImage(TextCtrl_GCM_Filename->GetValue());
            _clip = wxRect(0, 0, _startFrame.GetWidth()-1, _startFrame.GetHeight()-1);
            ShowImage(_startFrame);
        }
        else
        {
            VideoReader vr(std::string(TextCtrl_GCM_Filename->GetValue().c_str()), 800, 600, true);
            ShowImage(CreateImageFromFrame(vr.GetNextFrame(0)));
        }
    }
}

void GenerateCustomModelDialog::OnButton_GCM_SelectFileClick(wxCommandEvent& event)
{
    if (RadioBox2->GetSelection() == 2)
    {
        FileDialog1->SetWildcard(wxImage::GetImageExtWildcard());
    }
    else
    {
        FileDialog1->SetWildcard(VIDEOWILDCARD);
    }
    if (FileDialog1->ShowModal() == wxID_OK)
    {
        TextCtrl_GCM_Filename->SetValue(FileDialog1->GetDirectory() + "/" + FileDialog1->GetFilename());
        CVTabEntry();
    }

    ValidateWindow();
}

void GenerateCustomModelDialog::OnTextCtrl_GCM_FilenameText(wxCommandEvent& event)
{
    ValidateWindow();
}

void GenerateCustomModelDialog::SetStartFrame(int time)
{
    _startframetime = time;
    StaticText_StartTime->SetLabel(wxString::Format("%dms", time));
    _startFrame = CreateImageFromFrame(_vr->GetNextFrame(time));
    _clip = wxRect(0, 0, _startFrame.GetWidth()-1, _startFrame.GetHeight()-1);
    _startframebrightness = CalcFrameBrightness(_startFrame);
    _darkFrame = CreateImageFromFrame(_vr->GetNextFrame(time + LEADON)).ConvertToGreyscale();
    ShowImage(_startFrame);
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    logger_gcm.info("Start frame set to time %dms brightness %f.", time, _startframebrightness);
}

void GenerateCustomModelDialog::OnButton_CV_NextClick(wxCommandEvent& event)
{
    Button_CV_Next->Disable();
    Button_GCM_SelectFile->Disable();
    Button_CV_Back->Disable();
    TextCtrl_GCM_Filename->Disable();

    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    logger_gcm.info("File: %s.", std::string(TextCtrl_GCM_Filename->GetValue().c_str()).c_str());

    if (RadioBox2->GetSelection() == 2)
    {
        CheckBox_BI_ManualUpdate->SetValue(false);
        Button_BI_Update->Hide();
        CheckBox_BI_IsSteady->Hide();
        CheckBox_BI_ManualUpdate->Hide();
        DoBulbIdentify();
        BITabEntry();
        SwapPage(PAGE_CHOOSEVIDEO, PAGE_BULBIDENTIFY);
    }
    else
    {
        DoStartFrameIdentify();
        SFTabEntry();
        SwapPage(PAGE_CHOOSEVIDEO, PAGE_STARTFRAME);
    }
    ValidateWindow();
}

void GenerateCustomModelDialog::OnButton_CV_BackClick(wxCommandEvent& event)
{
    MTTabEntry();
    SwapPage(PAGE_CHOOSEVIDEO, PAGE_MODELTYPE);
    ValidateWindow();
}

#pragma endregion Choose Video

// ***********************************************************
// Start Frame tab methods
// ***********************************************************

#pragma region Start Frame

void GenerateCustomModelDialog::DoStartFrameIdentify()
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    if (_vr != NULL)
    {
        delete _vr;
        _vr = NULL;
    }

    _vr = new VideoReader(std::string(TextCtrl_GCM_Filename->GetValue().c_str()), 800, 600, true);

    if (_vr == NULL)
    {
        logger_gcm.info("Error starting video reader.");
        wxMessageBox("Unable to process video.");
        ValidateWindow();
        return;
    }

    SetCursor(wxCURSOR_WAIT);

    FindStartFrame(_vr);

    SetCursor(wxCURSOR_ARROW);
}

void GenerateCustomModelDialog::SFTabEntry()
{
    _state = VideoProcessingStates::FINDING_START_FRAME;
    ShowImage(_startFrame);
    Button_Back10Frames->Enable();
    Button_Back1Frame->Enable();
    Button_Forward10Frames->Enable();
    Button_Forward1Frame->Enable();
    Button_SF_Next->Enable();
    Button_SF_Back->Enable();
    ValidateWindow();
}

#define ALLOWABLEDIFFERENCE 0.2
// A frame looks like a valid start frame if another frame LEADON + FLAGOFF MS in the future is about as bright
bool GenerateCustomModelDialog::LooksLikeStartFrame(int candidateframe)
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    wxImage image = CreateImageFromFrame(_vr->GetNextFrame(candidateframe));
    float fimage = CalcFrameBrightness(image);

    if (fimage < 0.75 * _overallmaxbrightness)
    {
        logger_gcm.info("       Frame %d (%f) NOT close enough to maximum brightness %f to be considered start frame.", candidateframe, fimage, _overallmaxbrightness);
        return false;
    }

    int testframe = candidateframe + LEADON + FLAGOFF;
    wxImage nextimage = CreateImageFromFrame(_vr->GetNextFrame(testframe));
    float fnextimage = CalcFrameBrightness(nextimage);

    if (fnextimage > fimage * (1.0 - ALLOWABLEDIFFERENCE) && fnextimage < fimage * (1.0 + ALLOWABLEDIFFERENCE))
    {
        logger_gcm.info("       Frame %d (%f) and %d (%f) close enough to look like start frame.", candidateframe, fimage, testframe, fnextimage);
        // within +/-20% close enough
        return true;
    }

    logger_gcm.info("       Frame %d (%f) and %d (%f) NOT close enough to look like start frame.", candidateframe, fimage, testframe, fnextimage);
    return false;
}

float GenerateCustomModelDialog::CalcFrameBrightness(const wxImage& image)
{
    wxImage grey = image.ConvertToGreyscale();
    int w = grey.GetWidth();
    int h = grey.GetHeight();
    int w3 = w * 3;
    unsigned char * data = grey.GetData();
    int64_t total = 0;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            total += GetPixel(x,y,w3,data);
        }
    }

    return (float)((double)total /
        ((double)w * (double)h) / 255.0);
}

//#define PIXELBRIGHTNESSTHRESHOLD 235
//float GenerateCustomModelDialog::CalcFrameBrightness(wxImage& image)
//{
//    wxImage grey = image.ConvertToGreyscale();
//    int w = grey.GetWidth();
//    int h = grey.GetHeight();
//    int w3 = w * 3;
//    unsigned char * data = grey.GetData();
//    int64_t total = 0;
//    for (int y = 0; y < h; y++)
//    {
//        for (int x = 0; x < w; x++)
//        {
//            if (GetPixel(x, y, w3, data) > PIXELBRIGHTNESSTHRESHOLD)
//            {
//                total++;
//            }
//        }
//    }
//
//    return (float)total;
//}

// returns the MS of the best start frame - 0.1 MS into what looks like a bright section of the video that lasts about LEADON seconds
#define EXTRABRIGHTTHRESHOLD 0.2
int GenerateCustomModelDialog::FindStartFrame(VideoReader* vr)
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    std::list<float> framebrightness;

    StaticBitmap_Preview->SetEraseBackground(false);

    // skip over any leading blankness
    int start = 0;
    while (CalcFrameBrightness(CreateImageFromFrame(vr->GetNextFrame(start))) == 0.0)
    {
        start += FRAMEMS;
    }
    logger_gcm.info("Skipped first %dms due to black lead in.", start);
    logger_gcm.info("Finding start frame in first %dms.", (STARTSCANSECS * 1000) - start);

    // scan first STARTSCANSECS seconds of video build a list of average frame brightnesses
    for (int ms = start; ms < STARTSCANSECS * 1000; ms+=FRAMEMS)
    {
        wxImage img = CreateImageFromFrame(vr->GetNextFrame(ms));
        ShowImage(img);
        float b = CalcFrameBrightness(img);
        framebrightness.push_back(b);
        logger_gcm.info("   Frame %d brightness %f.", ms, b);
    }

    StaticBitmap_Preview->SetEraseBackground(true);

    // find the maximum number of frames in the video that it is about a set of brightness thresholds
    _overallmaxbrightness = 0.0;
    std::map<int, int> levelmaxlen;
    std::map<int, int> levelmaxstart;
    float level = 0.1;
    for (int i = 0; i < 9; i++)
    {
        int maxrunlength = 0;
        int currunlength = 0;
        int maxrunstart = 0;
        int currunstart = start / FRAMEMS;
        float maxrunbrightness = 0.0;
        float curmaxbrightness = 0.0;

        auto it = framebrightness.begin();
        for (int j = 0; j < framebrightness.size(); j++)
        {
            if (*it > level)
            {
                if (currunlength == 0)
                {
                    currunstart = j;
                }
                currunlength++;
                if (*it > curmaxbrightness)
                {
                    curmaxbrightness = *it;
                }
            }
            else
            {
                // take this run if it is closer to the right length
                //if (currunlength > maxrunlength || curmaxbrightness > maxrunbrightness * (1.0 + EXTRABRIGHTTHRESHOLD))
                if (abs(LEADON/FRAMEMS - currunlength) < abs(LEADON/FRAMEMS - maxrunlength))
                {
                    maxrunlength = currunlength;
                    maxrunstart = currunstart;
                    maxrunbrightness = curmaxbrightness;
                    if (maxrunbrightness > _overallmaxbrightness)
                    {
                        _overallmaxbrightness = maxrunbrightness;
                    }
                }
                currunlength = 0;
                curmaxbrightness = 0.0;
            }
            it++;
        }
        // take this run if it is closer to the right length
        //if (currunlength > maxrunlength || curmaxbrightness > maxrunbrightness * (1.0 + EXTRABRIGHTTHRESHOLD))
        if (abs(LEADON / FRAMEMS - currunlength) < abs(LEADON / FRAMEMS - maxrunlength))
        {
            maxrunlength = currunlength;
            maxrunstart = currunstart;
        }
        levelmaxlen[(int)(level*10.0)] = maxrunlength;
        levelmaxstart[(int)(level*10.0)] = maxrunstart;
        logger_gcm.info("   For level %f maxrunstarts at %dms and goes for %dms with max brightness %f.", level, maxrunstart * FRAMEMS, maxrunlength * FRAMEMS, maxrunbrightness);
        level += 0.1;
    }

    // look for thresholds that are close to LEADON long
    std::map<int, bool> suitable;
    for (int l = 1; l < 10; l++)
    {
        if (levelmaxlen[l] > LEADON / FRAMEMS - 2 && levelmaxlen[l] < LEADON / FRAMEMS + 2)
        {
            logger_gcm.info("   Level %f looks suitable from a length perspective.", (float)l/10.0);
            if (LooksLikeStartFrame(levelmaxstart[l] * FRAMEMS))
            {
                logger_gcm.info("       And looking forward the second flash also seems to be there.");
                suitable[l] = true;
            }
            else
            {
                // check just one more frame
                if (LooksLikeStartFrame((levelmaxstart[l] + 1) * FRAMEMS))
                {
                    logger_gcm.info("       And looking forward the second flash also seems to be there ... but only once I looked forward one frame.");
                    suitable[l] = true;
                }
                else
                {
                    suitable[l] = false;
                }
            }
        }
        else
        {
            suitable[l] = false;
        }
    }

    // choose the best threshold to use
    int first = 0;
    int last = 0;
    int curfirst = 0;
    int curlast = 0;
    for (int l = 1; l < 10; l++)
    {
        if (suitable[l])
        {
            curlast = l;
            if (curfirst == 0)
            {
                curfirst = l;
            }
        }
        else
        {
            if (curlast - curfirst > last - first)
            {
                last = curlast;
                first = curfirst;
            }
            curlast = 0;
            curfirst = 0;
        }
    }
    if (curlast - curfirst > last - first)
    {
        last = curlast;
        first = curfirst;
    }

    int bestlevel;
    if (first == 0)
    {
        logger_gcm.info("    No great match found.");
        bestlevel = 7;
        while (bestlevel > 1 && levelmaxstart[bestlevel] == 0)
        {
            bestlevel--;
        }
    }
    else
    {
        bestlevel = ((((float)last + (float)first + 1.0) * 10.0) / 2.0) / 10;
        logger_gcm.info("    Level chosen: halfway between %f and %f ... %f.", (float)first/10.0, (float)last/10.0, (float)bestlevel / 10.0);
    }

    // pick a point 0.1 secs into the high period as our start frame
    int candidateframe = levelmaxstart[bestlevel] * FRAMEMS + start;
    logger_gcm.info("    Selected start frame %d.", candidateframe);
    candidateframe += DELAYMSUNTILSAMPLE;
    logger_gcm.info("    After adding delay Selected start frame %d.", candidateframe);

    // check the second all on event is there ... if not move up to 10 frames forward looking for it
    for (int i = 0; i < 10; i++)
    {
        if (LooksLikeStartFrame(candidateframe))
        {
            break;
        }
        else
        {
            candidateframe+=FRAMEMS;
        }
    }

    logger_gcm.info("    After scanning forward the best start frame is %d.", candidateframe);

    SetStartFrame(candidateframe);
    ValidateStartFrame();

    return candidateframe;
}

void GenerateCustomModelDialog::ValidateStartFrame()
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    if (LooksLikeStartFrame(_startframetime))
    {
        logger_gcm.info("Start frame look ok.");
        StaticText_StartFrameOk->SetLabel("Looks ok.");
        StaticText_StartFrameOk->SetForegroundColour(*wxGREEN);
    }
    else
    {
        logger_gcm.info("Start frame does NOT look ok.");
        StaticText_StartFrameOk->SetLabel("Looks wrong.");
        StaticText_StartFrameOk->SetForegroundColour(*wxRED);
    }
}

void GenerateCustomModelDialog::MoveStartFrame(int by)
{
    _startframetime += by * FRAMEMS;

    if (_startframetime < 0)
    {
        _startframetime = 0;
    }

    if (_startframetime > _vr->GetLengthMS())
    {
        _startframetime = _vr->GetLengthMS();
    }

    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    logger_gcm.info("Start frame moved manually to %d.", _startframetime);
}

void GenerateCustomModelDialog::OnButton_Back1FrameClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        _busy = true;
        SetCursor(wxCURSOR_WAIT);
        MoveStartFrame(-1);
        SetStartFrame(_startframetime);
        ValidateStartFrame();
        ValidateWindow();
        SetCursor(wxCURSOR_ARROW);
        _busy = false;
    }
}

void GenerateCustomModelDialog::OnButton_Forward1FrameClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        _busy = true;
        SetCursor(wxCURSOR_WAIT);
        MoveStartFrame(1);
        SetStartFrame(_startframetime);
        ValidateStartFrame();
        ValidateWindow();
        SetCursor(wxCURSOR_ARROW);
        _busy = false;
    }
}

void GenerateCustomModelDialog::OnButton_Back10FramesClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        _busy = true;
        SetCursor(wxCURSOR_WAIT);
        MoveStartFrame(-10);
        SetStartFrame(_startframetime);
        ValidateStartFrame();
        ValidateWindow();
        SetCursor(wxCURSOR_ARROW);
        _busy = false;
    }
}

void GenerateCustomModelDialog::OnButton_Forward10FramesClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        _busy = true;
        SetCursor(wxCURSOR_WAIT);
        MoveStartFrame(10);
        SetStartFrame(_startframetime);
        ValidateStartFrame();
        ValidateWindow();
        SetCursor(wxCURSOR_ARROW);
        _busy = false;
    }
}

void GenerateCustomModelDialog::OnButton_SF_NextClick(wxCommandEvent& event)
{
    Button_Back10Frames->Disable();
    Button_Back1Frame->Disable();
    Button_Forward10Frames->Disable();
    Button_Forward1Frame->Disable();
    Button_SF_Next->Disable();
    Button_SF_Back->Disable();

    CheckBox_BI_ManualUpdate->SetValue(true);
    Button_BI_Update->Show();
    CheckBox_BI_IsSteady->Show();
    CheckBox_BI_ManualUpdate->Show();
    _biFrame = _startFrame;

    BITabEntry();
    SwapPage(PAGE_STARTFRAME, PAGE_BULBIDENTIFY);
    ValidateWindow();
}

void GenerateCustomModelDialog::OnButton_SF_BackClick(wxCommandEvent& event)
{
    SwapPage(PAGE_STARTFRAME, PAGE_CHOOSEVIDEO);
    CVTabEntry();
    ValidateWindow();
}

#pragma endregion Start Frame

// ***********************************************************
// Bulb Identify tab methods
// ***********************************************************

#pragma region Bulb Identify

void GenerateCustomModelDialog::ApplyThreshold(wxImage& image, int threshold)
{
    for (int i = 0; i < image.GetWidth() * image.GetHeight() * 3; i++)
    {
        if (*(image.GetData() + i) > threshold)
        {
            *(image.GetData() + i) = 255;
        }
        else
        {
            *(image.GetData() + i) = 0;
        }
    }
}

void GenerateCustomModelDialog::SubtractImage(wxImage& from, wxImage& tosubtract)
{
    int b = from.GetWidth() * 3 * from.GetHeight();
    unsigned char * datafrom = from.GetData();
    unsigned char * datatosubtract = tosubtract.GetData();
    for (int i = 0; i < b; i++)
    {
        if (*(datatosubtract + i) > *(datafrom + i))
        {
            *(datafrom + i) = 0;
        }
        else
        {
            *(datafrom + i) = *(datafrom + i) - *(datatosubtract + i);
        }
    }
}

int GenerateCustomModelDialog::CountWhite(wxImage& image)
{
    int res = 0;
    int b = image.GetWidth() * 3 * image.GetHeight();
    unsigned char * data = image.GetData();
    for (int i = 0; i < b; i+=3)
    {
        if (*(data + i) == 255)
        {
            res++;
        }
    }

    return res;
}

void GenerateCustomModelDialog::ApplyContrast(wxImage& grey, int contrast)
{
    // Dont need to do anything if zero
    if (contrast == 0)
    {
        return;
    }

    float factor = (259.0 * ((float)contrast + 255.0)) / (255.0 * (259.0 - (float)contrast));

    int w = grey.GetWidth();
    int h = grey.GetHeight();
    int w3 = w * 3;
    unsigned char* data = grey.GetData();
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            SetPixel(x, y, w3, data, (unsigned char)(factor * (GetPixel(x,y,w3,data) - 128) + 128));
        }
    }
}

void GenerateCustomModelDialog::DoBulbIdentify()
{
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));

    if (!_busy)
    {
        _busy = true;

        Slider_AdjustBlur->Disable();
        Slider_BI_Sensitivity->Disable();
        Slider_BI_MinSeparation->Disable();
        Slider_BI_Contrast->Disable();
        CheckBox_BI_ManualUpdate->Disable();
        CheckBox_BI_IsSteady->Disable();
        Button_BI_Update->Disable();
        Button_CB_RestoreDefault->Disable();
        Button_BI_Next->Disable();
        Button_BI_Back->Disable();
        SetCursor(wxCURSOR_WAIT);

        wxYield(); // let them update

        logger_gcm.info("Executing bulb identify.");
        logger_gcm.info("   Image Size: %dx%d.", _startFrame.GetWidth(), _startFrame.GetHeight());
        logger_gcm.info("   Blur: %d.", Slider_AdjustBlur->GetValue());
        logger_gcm.info("   Sensitivity: %d.", Slider_BI_Sensitivity->GetValue());
        logger_gcm.info("   Contrast: %d.", Slider_BI_Contrast->GetValue());
        logger_gcm.info("   Minimum Separation: %d.", Slider_BI_MinSeparation->GetValue());
        logger_gcm.info("   Clip Rectangle: (%d,%d)-(%d,%d).", _clip.GetLeft(), _clip.GetTop(), _clip.GetRight(), _clip.GetBottom());
        if (CheckBox_BI_IsSteady->GetValue())
        {
            logger_gcm.info("   Is Steady: TRUE.");
        }
        else
        {
            logger_gcm.info("   Is Steady: FALSE.");
        }
        _warned = false;
        _lights.clear();
        if (RadioBox2->GetSelection() == 2)
        {
            _startFrame.LoadFile(TextCtrl_GCM_Filename->GetValue());
            wxImage bwFrame;
            wxImage grey = _startFrame.ConvertToGreyscale();
            ApplyContrast(grey, Slider_BI_Contrast->GetValue());
            wxImage imgblur = grey.Blur(Slider_AdjustBlur->GetValue());
            bwFrame = imgblur;
            ApplyThreshold(bwFrame, Slider_BI_Sensitivity->GetValue());
            FindLights(bwFrame, 1, grey);
            _biFrame = CreateDetectMask(bwFrame, true, _clip);
        }
        else
        {
            // handle videos here
            int currentTime = _startframetime + LEADON + FLAGOFF + FLAGON + (0.9 * (float)FLAGOFF);
            int n = 1;

            int sincefound = 0;
            while (currentTime < _vr->GetLengthMS() && !_warned && sincefound < 100)
            {
                logger_gcm.info("   Looking for frame at %d.", currentTime);
                wxImage bwFrame;
                wxImage grey;

                while ((!bwFrame.IsOk() || CountWhite(bwFrame) < 50) && currentTime < _vr->GetLengthMS() && sincefound < 100)
                {
                    sincefound++;
                    if (bwFrame.IsOk())
                    {
                        currentTime += FRAMEMS;
                        logger_gcm.info("   No frame found so now trying %d.", currentTime);
                    }

                    wxImage frame = CreateImageFromFrame(_vr->GetNextFrame(currentTime));
                    float brightness = CalcFrameBrightness(frame);

                    if (brightness > _startframebrightness * .8)
                    {
                        logger_gcm.info("   End found at time %d as brightness (%f) > 80%% of start frame brightness (%f).", currentTime, brightness, _startframebrightness);
                        // this is our end frame ...
                        _busy = false;
                        _biFrame = CreateDetectMask(_startFrame, true, _clip);
                        ShowImage(_biFrame);
                        Slider_AdjustBlur->Enable();
                        Slider_BI_Sensitivity->Enable();
                        Slider_BI_MinSeparation->Enable();
                        Slider_BI_Contrast->Enable();
                        Button_CB_RestoreDefault->Enable();
                        CheckBox_BI_ManualUpdate->Enable();
                        CheckBox_BI_IsSteady->Enable();
                        Button_BI_Update->Enable();
                        Button_BI_Next->Enable();
                        Button_BI_Back->Enable();
                        SetCursor(wxCURSOR_ARROW);
                        logger_gcm.info("Result: %s.", std::string(TextCtrl_BI_Status->GetValue().c_str()).c_str());
                        return;
                    }

                    grey = frame.ConvertToGreyscale();
                    if (CheckBox_BI_IsSteady->GetValue())
                    {
                        SubtractImage(grey, _darkFrame);
                    }
                    ApplyContrast(grey, Slider_BI_Contrast->GetValue());
                    wxImage imgblur = grey.Blur(Slider_AdjustBlur->GetValue());
                    bwFrame = imgblur;
                    ApplyThreshold(bwFrame, Slider_BI_Sensitivity->GetValue());
                }

                if (sincefound < 100)
                {
                    sincefound = 0;
                    FindLights(bwFrame, n++, grey);
                    currentTime = currentTime + (0.9 * (float)(NODEON + NODEOFF));
                }
            }

            if (sincefound >= 100)
            {
                logger_gcm.info("   Too many frames with no lights spotted. Aborting scan. 100 frames no bulb.");
                wxMessageBox("Too many frames with no lights spotted. Aborting scan.");
            }

            _biFrame = CreateDetectMask(_startFrame, true, _clip);
        }
        ShowImage(_biFrame);
        Slider_AdjustBlur->Enable();
        Slider_BI_Sensitivity->Enable();
        Slider_BI_MinSeparation->Enable();
        Slider_BI_Contrast->Enable();
        CheckBox_BI_ManualUpdate->Enable();
        CheckBox_BI_IsSteady->Enable();
        Button_BI_Update->Enable();
        Button_CB_RestoreDefault->Enable();
        Button_BI_Next->Enable();
        Button_BI_Back->Enable();
        SetCursor(wxCURSOR_ARROW);
        logger_gcm.info("Result: %s.", std::string(TextCtrl_BI_Status->GetValue().c_str()).c_str());
        _busy = false;
    }
}

void GenerateCustomModelDialog::BITabEntry()
{
    _state = VideoProcessingStates::IDENTIFYING_BULBS;
    ShowImage(_biFrame);
    StaticText_BI->SetLabel("The red circles on the image show the bulbs we have identified. Adjust the sensitivity if there are bulbs missing or phantom bulbs identified.\n\nClick next when you are happy that all bulbs have been detected.");
    Slider_BI_Sensitivity->Enable();
    Slider_AdjustBlur->Enable();
    Slider_BI_MinSeparation->Enable();
    Slider_BI_Contrast->Enable();
    Button_CB_RestoreDefault->Enable();
    Button_BI_Next->Enable();
    Button_BI_Back->Enable();
}

int GenerateCustomModelDialog::GetMaxNum()
{
    int max = -1;
    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed() && it->GetNum() > max)
        {
            max = it->GetNum();
        }
    }

    return max;
}

int GenerateCustomModelDialog::GetBulbCount()
{
    int count = 0;
    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            count++;
        }
    }

    return count;
}

// Assumes nodes are in order
wxString GenerateCustomModelDialog::GetMissingNodes()
{
    wxString res;
    int current = 0;
    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            if (it->GetNum() == current)
            {
                // this is ok ... a second bulb for this node
            }
            else if (it->GetNum() == current + 1)
            {
                // this is ok ... we have moved on to next node
                current++;
            }
            else
            {
                // this is a problem
                for (int i = current + 1; i < it->GetNum(); i++)
                {
                    if (res != "")
                    {
                        res += ", ";
                    }
                    res += wxString::Format("%d", i);
                }
                current = it->GetNum();
            }
        }
    }

    return res;
}

// Assumes nodes are in order
wxString GenerateCustomModelDialog::GetMultiBulbNodes()
{
    wxString res;
    int current = -1;
    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            if (it->GetNum() == current)
            {
                // this is ok ... a second bulb for this node
                if (res != "")
                {
                    res += ", ";
                }
                res += wxString::Format("%d", it->GetNum());
                current++;
            }
            else if (it->GetNum() > current)
            {
                // this is ok ... we have moved on to next node
                current = it->GetNum();
            }
        }
    }

    return res;
}

wxString GenerateCustomModelDialog::GenerateStats(int minseparation)
{
    wxString res;

    int n = GetMaxNum();
    if (n < 1)
    {
        res += "Nodes: None\n";
    }
    else
    {
        res += wxString::Format("Nodes: %d\n", n);
    }
    res += wxString::Format("Bulbs: %d\n", GetBulbCount());
    if (minseparation == 9999999)
    {
        res += "Minimum Bulb Separation: N/A\n";
    }
    else
    {
        res += wxString::Format("Minimum Bulb Separation: %d\n", minseparation);
    }
    wxString mn = GetMissingNodes();
    if (mn == "")
    {
        res += "Missing Nodes: N/A\n";
    }
    else
    {
        res += wxString::Format("Missing Nodes: %s\n", mn);
    }
    wxString mbn = GetMultiBulbNodes();
    if (mbn == "")
    {
        res += "Nodes with more than 1 bulb: N/A\n";
    }
    else
    {
        res += wxString::Format("Nodes with more than 1 bulb: %s\n", mbn);
    }
    wxSize p = CalcSize();
    res += wxString::Format("Model size: %dx%d\n", p.x, p.y);

    return res;
}

wxImage GenerateCustomModelDialog::CreateDetectMask(wxImage ref, bool includeimage, wxRect clip)
{
    for (auto it = _lights.begin(); it != _lights.end(); ++it)
    {
        it->Reset();
    }

    RemoveClippedLights(_lights, _clip);
    int min = ApplyMinimumSeparation(_lights, Slider_BI_MinSeparation->GetValue());

    TextCtrl_BI_Status->SetValue(GenerateStats(min));

    wxBitmap bmp(ref.GetWidth(), ref.GetHeight());
    wxMemoryDC dc(bmp);

    if (includeimage)
    {
        dc.DrawBitmap(ref, wxPoint(0, 0), false);
    }

    wxSize displaysize = StaticBitmap_Preview->GetSize();
    float factor = std::max((float)_startFrame.GetWidth() / (float)displaysize.GetWidth(),
        (float)_startFrame.GetHeight() / (float)displaysize.GetHeight());

    // draw grey out clipped area
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxBrush shade(*wxLIGHT_GREY, wxBRUSHSTYLE_BDIAGONAL_HATCH);
    dc.SetBrush(shade);
    if (_clip.GetLeft() > 0)
    {
        dc.DrawRectangle(wxRect(0, 0, _clip.GetLeft(), _startFrame.GetHeight()));
    }
    if (_clip.GetRight() < _startFrame.GetWidth())
    {
        dc.DrawRectangle(wxRect(_clip.GetRight(), 0, _startFrame.GetWidth() - _clip.GetRight(), _startFrame.GetHeight()));
    }
    if (_clip.GetTop() > 0)
    {
        dc.DrawRectangle(wxRect(0, 0, _startFrame.GetWidth(), _clip.GetTop()));
    }
    if (_clip.GetBottom() < _startFrame.GetHeight())
    {
        dc.DrawRectangle(wxRect(0, _clip.GetBottom(), _startFrame.GetWidth(), _startFrame.GetHeight() - _clip.GetBottom()));
    }

    // Draw clip rectangle
    int penw = 2 * factor;
    wxPen p2(*wxGREEN, penw, wxPENSTYLE_LONG_DASH);
    dc.SetPen(p2);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(_clip);

    // draw blue first
    for (auto c = _lights.begin(); c != _lights.end(); c++)
    {
        if (c->isSupressedButDraw())
        {
            c->Draw(dc, factor);
        }
    }

    // now red so they are easy to see
    for (auto c = _lights.begin(); c != _lights.end(); c++)
    {
        if (!c->isSupressed())
        {
            c->Draw(dc, factor);
        }
    }

    return bmp.ConvertToImage();
}

void GenerateCustomModelDialog::WalkPixels(int x, int y, int w, int h, int w3, unsigned char *data, int& totalX, int& totalY, int& pixelCount)
{
    std::list<wxPoint> pixels;
    pixels.push_back(wxPoint(x, y));

    while(pixels.size() != 0 && pixels.size() < 1000)
    {
        std::list<wxPoint>::iterator it = pixels.begin();

        if (GetPixel(it->x, it->y, w3, data) > 0)
        {
            SetPixel(it->x, it->y, w3, data, 0);
            pixelCount++;
            totalX += it->x;
            totalY += it->y;

            if (GetPixel(it->x - 1, it->y - 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x - 1, it->y - 1));
            }
            if (GetPixel(it->x - 1, it->y + 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x - 1, it->y + 1));
            }
            if (GetPixel(it->x + 1, it->y - 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x + 1, it->y - 1));
            }
            if (GetPixel(it->x + 1, it->y + 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x + 1, it->y + 1));
            }
            if (GetPixel(it->x, it->y - 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x, it->y - 1));
            }
            if (GetPixel(it->x, it->y + 1, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x, it->y + 1));
            }
            if (GetPixel(it->x - 1, it->y, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x - 1, it->y));
            }
            if (GetPixel(it->x + 1, it->y, w3, data) > 0) {
                pixels.push_back(wxPoint(it->x + 1, it->y));
            }
        }
        pixels.pop_front();
    }

    if (pixels.size() != 0)
    {
        if (!_warned)
        {
            _warned = true;
            log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
            logger_gcm.info("Too many pixels are looking like bulbs ... this could take forever ... you need to change your settings ... maybe increase sensitivity.");
            wxMessageBox("Too many pixels are looking like bulbs ... this could take forever ... you need to change your settings ... maybe increase sensitivity.");
        }
    }
}

GCMBulb GenerateCustomModelDialog::FindCenter(int x, int y, int w, int h, int w3, unsigned char *data, int num, wxImage& grey)
{
    int totalX = 0;
    int totalY = 0;
    int pixelCount = 0;
    WalkPixels(x, y, w, h, w3, data, totalX, totalY, pixelCount);

    return GCMBulb(wxPoint(totalX / pixelCount, totalY / pixelCount), num, GetPixel(totalX / pixelCount, totalY / pixelCount, w3, grey.GetData()));
}

void GenerateCustomModelDialog::FindLights(wxImage& bwimage, int num, wxImage& greyimage)
{
    wxImage temp = bwimage;
    temp.UnShare(); // we are going to change the data so get out own copy
    int w = temp.GetWidth();
    int w3 = w * 3;
    int h = temp.GetHeight();
    unsigned char * data = temp.GetData();

    for (int y = 0; y < temp.GetHeight(); y++)
    {
        for (int x = 0; x < temp.GetWidth(); x++)
        {
            if (GetPixel(x, y, w3, data) > 0)
            {
                _lights.push_back(FindCenter(x, y, w, h, w3, data, num, greyimage));
            }
        }
    }

    _biFrame = CreateDetectMask(_startFrame, true, _clip);
    ShowImage(_biFrame);
}

void GenerateCustomModelDialog::OnSlider_BI_SensitivityCmdSliderUpdated(wxScrollEvent& event)
{
    TextCtrl_BI_Sensitivity->SetValue(wxString::Format("%d", Slider_BI_Sensitivity->GetValue()));
}

void GenerateCustomModelDialog::OnSlider_AdjustBlurCmdScroll(wxScrollEvent& event)
{
    TextCtrl_BC_Blur->SetValue(wxString::Format("%d", Slider_AdjustBlur->GetValue()));
}

void GenerateCustomModelDialog::OnSlider_BI_MinSeparationCmdSliderUpdated(wxScrollEvent& event)
{
    TextCtrl_BI_MinSeparation->SetValue(wxString::Format("%d", Slider_BI_MinSeparation->GetValue()));
}

void GenerateCustomModelDialog::OnButton_BI_RestoreDefaultClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        Slider_BI_MinSeparation->SetValue(10);
        TextCtrl_BI_MinSeparation->SetValue("10");
        Slider_AdjustBlur->SetValue(1);
        TextCtrl_BC_Blur->SetValue("1");
        Slider_BI_Sensitivity->SetValue(127);
        TextCtrl_BI_Sensitivity->SetValue("127");
        Slider_BI_Contrast->SetValue(0);
        TextCtrl_BI_Contrast->SetValue("0");
        if (!CheckBox_BI_ManualUpdate->GetValue())
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnSlider_AdjustBlurCmdScrollChanged(wxScrollEvent& event)
{
    if (!CheckBox_BI_ManualUpdate->GetValue())
    {
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnSlider_BI_SensitivityCmdScrollChanged(wxScrollEvent& event)
{
    if (!CheckBox_BI_ManualUpdate->GetValue())
    {
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnSlider_BI_MinSeparationCmdScrollChanged(wxScrollEvent& event)
{
    if (!CheckBox_BI_ManualUpdate->GetValue())
    {
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnSlider_BI_ContrastCmdScrollChanged(wxScrollEvent& event)
{
    if (!CheckBox_BI_ManualUpdate->GetValue())
    {
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnCheckBox_BI_IsSteadyClick(wxCommandEvent& event)
{
    if (!CheckBox_BI_ManualUpdate->GetValue())
    {
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnCheckBox_BI_ManualUpdateClick(wxCommandEvent& event)
{
    if (CheckBox_BI_ManualUpdate->GetValue())
    {
        Button_BI_Update->Show();
    }
    else
    {
        Button_BI_Update->Hide();
        if (!_busy)
        {
            DoBulbIdentify();
        }
    }
}

void GenerateCustomModelDialog::OnButton_BI_UpdateClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        DoBulbIdentify();
    }
}

void GenerateCustomModelDialog::OnSlider_BI_ContrastCmdSliderUpdated(wxScrollEvent& event)
{
    TextCtrl_BI_Contrast->SetValue(wxString::Format("%d", Slider_BI_Contrast->GetValue()));
}

void GenerateCustomModelDialog::OnButton_BI_NextClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        DoGenerateCustomModel();
        CMTabEntry();
        SwapPage(PAGE_BULBIDENTIFY, PAGE_REVIEWMODEL);
    }
}

void GenerateCustomModelDialog::OnButton_BI_BackClick(wxCommandEvent& event)
{
    if (!_busy)
    {
        if (RadioBox2->GetSelection() == 2)
        {
            CVTabEntry();
            SwapPage(PAGE_BULBIDENTIFY, PAGE_CHOOSEVIDEO);
        }
        else
        {
            SFTabEntry();
            SwapPage(PAGE_BULBIDENTIFY, PAGE_STARTFRAME);
        }
    }
}

#pragma endregion Bulb Identify

// ***********************************************************
// Custom Model tab methods
// ***********************************************************

#pragma region Custom Model

void GenerateCustomModelDialog::CMTabEntry()
{
    _state = VideoProcessingStates::REVIEW_CUSTOM_MODEL;
    ShowImage(_biFrame);
}

bool GenerateCustomModelDialog::TestScale(std::list<GCMBulb>& lights, std::list<GCMBulb>::iterator it, float scale, wxPoint trim)
{
    GCMBulb b = *it;
    ++it;
    if (it != lights.end() && !it->isSupressed())
    {
        if (!TestScale(lights, it, scale, trim))
        {
            return false;
        }
        while (it != lights.end())
        {
            if (!it->isSupressed())
            {
                if (b.IsSameLocation(*it, scale, trim))
                {
                    return false;
                }
            }
            ++it;
        }
    }
    return true;
}

wxPoint GenerateCustomModelDialog::CalcTrim(std::list<GCMBulb>& lights)
{
    int x = 999999;
    int y = 999999;

    for (auto it = lights.begin(); it != lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            wxPoint loc = it->GetLocation();
            if (loc.x < x)
            {
                x = loc.x;
            }
            if (loc.y < y)
            {
                y = loc.y;
            }
        }
    }

    return wxPoint(x, y);
}

wxSize GenerateCustomModelDialog::CalcSize()
{
    if (_lights.size() == 0)
    {
        return wxSize(0, 0);
    }

    _trim = CalcTrim(_lights);

    float best = 1.0;
    float curr = 0.9;

    while (curr > 0 && TestScale(_lights, _lights.begin(), curr, _trim))
    {
        best = curr;
        curr = curr - 0.1;
    }
    float start = curr;
    curr = curr - 0.01;
    while (curr > start && TestScale(_lights, _lights.begin(), curr, _trim))
    {
        best = curr;
        curr = curr - 0.01;
    }
    _scale = best;

    int x = 0;
    int y = 0;

    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            wxPoint loc = it->GetLocation(_scale, _trim);
            if (loc.x > x)
            {
                x = loc.x;
            }
            if (loc.y > y)
            {
                y = loc.y;
            }
        }
    }

    return wxSize(x+1, y+1);
}

void GenerateCustomModelDialog::RemoveClippedLights(std::list<GCMBulb>& lights, wxRect& clip)
{
    for (auto it = lights.begin(); it != lights.end(); ++it)
    {
        if (!it->isSupressed())
        {
            int x = it->GetLocation().x;
            int y = it->GetLocation().y;
            if (x < _clip.GetLeft() || x > _clip.GetRight() ||
                y < _clip.GetTop() || y > _clip.GetBottom())
            {
                it->OutsideClip();
            }
        }
    }
}

inline int GetSeparation(int x1, int y1, int x2, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
inline bool IsWithin(int x1, int y1, int x2, int y2, int d)
{
    return d > GetSeparation(x1, y1, x2, y2);
}

// return minimum actual separation
int GenerateCustomModelDialog::ApplyMinimumSeparation(std::list<GCMBulb>& lights, int minseparation)
{
    int min = 9999999;
    for (auto it = lights.begin(); it != lights.end(); ++it)
    {
        if (!it->isSupressed())
        {
            for (auto it2 = it; it2 != lights.end(); ++it2)
            {
                if (it != it2 && !it2->isSupressed())
                {
                    if (IsWithin(it->GetLocation().x, it->GetLocation().y, it2->GetLocation().x, it2->GetLocation().y, minseparation))
                    {
                        if (it->GetBrightness() >= it2->GetBrightness())
                        {
                            it2->TooClose();
                        }
                        else
                        {
                            it->TooClose();
                        }
                    }
                    else
                    {
                        int d = GetSeparation(it->GetLocation().x, it->GetLocation().y, it2->GetLocation().x, it2->GetLocation().y);
                        if (d < min)
                        {
                            min = d;
                        }
                    }
                }
            }
        }
    }

    return min;
}

// this will find the best scale to 1/100th of the imput size
void GenerateCustomModelDialog::DoGenerateCustomModel()
{
    if (_lights.size() == 0)
    {
        return;
    }

    for (auto it = _lights.begin(); it != _lights.end(); ++it)
    {
        it->Reset();
    }

    RemoveClippedLights(_lights, _clip);
    ApplyMinimumSeparation(_lights, Slider_BI_MinSeparation->GetValue());

    _size = CalcSize();

    Grid_CM_Result->ClearGrid();
    if (Grid_CM_Result->GetNumberCols() > 0)
    {
        Grid_CM_Result->DeleteCols(0, Grid_CM_Result->GetNumberCols());
        if (Grid_CM_Result->GetNumberRows() > 0)
        {
            Grid_CM_Result->DeleteRows(0, Grid_CM_Result->GetNumberRows());
        }
        Grid_CM_Result->AppendCols(_size.x);
        Grid_CM_Result->AppendRows(_size.y);
    }
    else
    {
        Grid_CM_Result->CreateGrid(_size.y, _size.x);
    }

    for (auto it = _lights.begin(); it != _lights.end(); it++)
    {
        if (!it->isSupressed())
        {
            wxPoint p = it->GetLocation(_scale, _trim);
            Grid_CM_Result->SetCellValue(p.y, p.x, wxString::Format("%d", it->GetNum()));
            Grid_CM_Result->SetCellBackgroundColour(p.y, p.x, *wxGREEN);
        }
    }

    wxFont font = Grid_CM_Result->GetDefaultCellFont();
    Grid_CM_Result->SetRowMinimalAcceptableHeight(5); //don't need to read text, just see the shape
    Grid_CM_Result->SetColMinimalAcceptableWidth(5); //don't need to read text, just see the shape
    for (int c = 0; c < Grid_CM_Result->GetNumberCols(); ++c)
        Grid_CM_Result->SetColSize(c, 2 * font.GetPixelSize().y); //Grid_CM_Result->GetColSize(c) * 4/5);
    for (int r = 0; r < Grid_CM_Result->GetNumberRows(); ++r)
        Grid_CM_Result->SetRowSize(r, int(1.5 * (float)font.GetPixelSize().y)); //Grid_CM_Result->GetRowSize(r) * 4/5);
    font = Grid_CM_Result->GetLabelFont();
    Grid_CM_Result->SetColLabelSize(int(1.5 * (float)font.GetPixelSize().y));
    FlexGridSizer19->Layout();
    Layout();
}

void GenerateCustomModelDialog::OnButton_CM_BackClick(wxCommandEvent& event)
{
    BITabEntry();
    SwapPage(PAGE_REVIEWMODEL, PAGE_BULBIDENTIFY);
}

wxString GenerateCustomModelDialog::CreateCustomModelData()
{
    wxString res = "";
    for (int y = 0; y < Grid_CM_Result->GetNumberRows(); y++)
    {
        for (int x = 0; x < Grid_CM_Result->GetNumberCols(); x++)
        {
            res += Grid_CM_Result->GetCellValue(y, x);

            if (x + 1 != Grid_CM_Result->GetNumberCols())
            {
                res += ",";
            }
        }

        if (y + 1 != Grid_CM_Result->GetNumberRows())
        {
            res += ";";
        }
    }

    return res;
}

void GenerateCustomModelDialog::OnButton_CM_SaveClick(wxCommandEvent& event)
{
    wxLogNull logNo; //kludge: avoid "error 0" message from wxWidgets after new file is written
    wxString filename = wxFileSelector(_("Choose output file"), wxEmptyString, "NewCustomModel", wxEmptyString, "Custom Model files (*.xmodel)|*.xmodel", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (filename.IsEmpty()) return;
    wxFile f(filename);
    log4cpp::Category &logger_gcm = log4cpp::Category::getInstance(std::string("log_generatecustommodel"));
    logger_gcm.info("Saving to xmodel file %s.", std::string(filename.c_str()).c_str());
    if (!f.Create(filename, true) || !f.IsOpened())
    {
        logger_gcm.info("Unable to create file %s. Error %d\n", std::string(filename.c_str()).c_str(), f.GetLastError());
        wxMessageBox(wxString::Format("Unable to create file %s. Error %d\n", filename, f.GetLastError()));
        return;
    }
    wxString name = wxFileName(filename).GetName();
    wxString cm = CreateCustomModelData();
    wxString p1 = wxString::Format("%d",Grid_CM_Result->GetNumberCols());
    wxString p2 = wxString::Format("%d", Grid_CM_Result->GetNumberRows());
    wxString st;
    if (RadioBox2->GetSelection() == 2)
    {
        st = "Single Color White";
    }
    else if (RadioBox2->GetSelection() == 1)
    {
        st = "Single Color White";
    }
    else
    {
        st = "RGB Nodes";
    }
    wxString ps = "2";
    wxString t = "0";
    wxString mb = "";
    wxString a = "1";
    wxString sn = "";
    wxString nn = "";
    wxString v = xlights_version_string;
    f.Write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<custommodel \n");
    f.Write(wxString::Format("name=\"%s\" ", name));
    f.Write(wxString::Format("parm1=\"%s\" ", p1));
    f.Write(wxString::Format("parm2=\"%s\" ", p2));
    f.Write(wxString::Format("StringType=\"%s\" ", st));
    f.Write(wxString::Format("Transparency=\"%s\" ", t));
    f.Write(wxString::Format("PixelSize=\"%s\" ", ps));
    f.Write(wxString::Format("ModelBrightness=\"%s\" ", mb));
    f.Write(wxString::Format("Antialias=\"%s\" ", a));
    f.Write(wxString::Format("StrandNames=\"%s\" ", sn));
    f.Write(wxString::Format("NodeNames=\"%s\" ", nn));
    f.Write(wxString::Format("CustomModel=\"%s\" ", cm));
    f.Write(wxString::Format("SourceVersion=\"%s\" ", v));
    f.Write(" />\n");
    f.Close();
}

void GenerateCustomModelDialog::OnButton_GrowClick(wxCommandEvent& event)
{
    Grid_CM_Result->BeginBatch();
    wxFont font = Grid_CM_Result->GetLabelFont();
    font.MakeLarger();
    Grid_CM_Result->SetLabelFont(font);
    font = Grid_CM_Result->GetDefaultCellFont();
    font.MakeLarger();
    Grid_CM_Result->SetDefaultCellFont(font);
    for (int c = 0; c < Grid_CM_Result->GetNumberCols(); ++c)
        Grid_CM_Result->SetColSize(c, 2 * font.GetPixelSize().y); //Grid_CM_Result->GetColSize(c) * 5/4);
    for (int r = 0; r < Grid_CM_Result->GetNumberRows(); ++r)
        Grid_CM_Result->SetRowSize(r, int(1.5 * (float)font.GetPixelSize().y)); //Grid_CM_Result->GetRowSize(r) * 5/4);
    Grid_CM_Result->EndBatch();
}

void GenerateCustomModelDialog::OnButton_ShrinkClick(wxCommandEvent& event)
{
    Grid_CM_Result->BeginBatch();
    wxFont font = Grid_CM_Result->GetLabelFont();
    font.MakeSmaller();
    Grid_CM_Result->SetLabelFont(font);
    font = Grid_CM_Result->GetDefaultCellFont();
    font.MakeSmaller();
    Grid_CM_Result->SetDefaultCellFont(font);
    Grid_CM_Result->SetRowMinimalAcceptableHeight(5); //don't need to read text, just see the shape
    Grid_CM_Result->SetColMinimalAcceptableWidth(5); //don't need to read text, just see the shape
    for (int c = 0; c < Grid_CM_Result->GetNumberCols(); ++c)
        Grid_CM_Result->SetColSize(c, 2 * font.GetPixelSize().y); //Grid_CM_Result->GetColSize(c) * 4/5);
    for (int r = 0; r < Grid_CM_Result->GetNumberRows(); ++r)
        Grid_CM_Result->SetRowSize(r, int(1.5 * (float)font.GetPixelSize().y)); //Grid_CM_Result->GetRowSize(r) * 4/5);
    Grid_CM_Result->EndBatch();
}
#pragma endregion Custom Model

#pragma endregion Generate

int GenerateCustomModelDialog::GetEdge(int x, int y)
{
    wxSize displaysize = StaticBitmap_Preview->GetSize();
    float xf = (float)_startFrame.GetWidth() / (float)displaysize.GetWidth();
    float yf = (float)_startFrame.GetHeight() / (float)displaysize.GetHeight();
    int edge = -1;

    if (std::abs(xf*x - _clip.GetLeft()) < 3 * xf)
    {
        if (yf*y >= _clip.GetTop() && yf*y <= _clip.GetBottom())
        {
            edge = 3;
        }
    }
    else if (std::abs(xf*x - _clip.GetRight()) < 5 * xf)
    {
        if (yf*y >= _clip.GetTop() && yf*y <= _clip.GetBottom())
        {
            edge = 1;
        }
    }
    else if (std::abs(yf*y - _clip.GetTop()) < 5*yf)
    {
        if (xf*x >= _clip.GetLeft() && xf*x <= _clip.GetRight())
        {
            edge = 0;
        }
    }
    else if (std::abs(yf*y - _clip.GetBottom()) < 5*yf)
    {
        if (xf*x >= _clip.GetLeft() && xf*x <= _clip.GetRight())
        {
            edge = 2;
        }
    }

    return edge;
}

void GenerateCustomModelDialog::ResizeClip(int x, int y)
{
    wxSize displaysize = StaticBitmap_Preview->GetSize();
    float xf = (float)_startFrame.GetWidth() / (float)displaysize.GetWidth() * (float)x;
    float yf = (float)_startFrame.GetHeight() / (float)displaysize.GetHeight() * (float)y;
    if (_draggingedge == -1)
    {
        return;
    }
    else if (_draggingedge == 0)
    {
        int b = _clip.GetBottom();
        _clip.SetTop(yf);
        _clip.SetBottom(b);
    }
    else if (_draggingedge == 1)
    {
        _clip.SetRight(xf);
    }
    else if (_draggingedge == 2)
    {
        _clip.SetBottom(yf);
    }
    else if (_draggingedge == 3)
    {
        int r = _clip.GetRight();
        _clip.SetLeft(xf);
        _clip.SetRight(r);
    }
    StaticBitmap_Preview->SetEraseBackground(false);
    _biFrame = CreateDetectMask(_startFrame, true, _clip);
    ShowImage(_biFrame);
    StaticBitmap_Preview->SetEraseBackground(true);
}

void GenerateCustomModelDialog::OnStaticBitmapLeftDown(wxMouseEvent& event)
{
    // only doing dragging on one page
    if (_state == VideoProcessingStates::IDENTIFYING_BULBS)
    {
        _draggingedge = GetEdge(event.GetX(), event.GetY());
        if (_draggingedge >= 0)
        {
            ResizeClip(event.GetX(), event.GetY());
            if (_draggingedge % 2 == 0)
            {
                SetCursor(wxCURSOR_SIZENS);
            }
            else
            {
                SetCursor(wxCURSOR_SIZEWE);
            }
        }
    }
}

void GenerateCustomModelDialog::OnStaticBitmapLeftUp(wxMouseEvent& event)
{
    if (_state == VideoProcessingStates::IDENTIFYING_BULBS)
    {
        if (_draggingedge >= 0)
        {
            ResizeClip(event.GetX(), event.GetY());
        }
        _draggingedge = -1;
        SetCursor(wxCURSOR_ARROW);
    }
}


void GenerateCustomModelDialog::OnStaticBitmapMouseLeave(wxMouseEvent& event)
{
    if (_state == VideoProcessingStates::IDENTIFYING_BULBS)
    {
        if (_draggingedge >= 0)
        {
            ResizeClip(std::min(event.GetX(),_startFrame.GetWidth() - 1), std::min(event.GetY(), _startFrame.GetHeight() - 1));
        }
        _draggingedge = -1;
        SetCursor(wxCURSOR_ARROW);
    }
}

void GenerateCustomModelDialog::OnStaticBitmapMouseMove(wxMouseEvent& event)
{
    if (_state == VideoProcessingStates::IDENTIFYING_BULBS)
    {
        if (_draggingedge >= 0)
        {
            ResizeClip(event.GetX(), event.GetY());
        }
        else
        {
            int e = GetEdge(event.GetX(), event.GetY());
            if (e == -1)
            {
                SetCursor(wxCURSOR_ARROW);
            }
            else if (e % 2 == 0)
            {
                SetCursor(wxCURSOR_SIZENS);
            }
            else
            {
                SetCursor(wxCURSOR_SIZEWE);
            }
        }
    }
}





