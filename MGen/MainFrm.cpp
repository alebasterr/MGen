// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MGen.h"
#include "MainFrm.h"
#include "EditParamsDlg.h"
#include "AlgoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER1 1
#define TIMER2 2

// CMainFrame

static UINT WM_GEN_FINISH = RegisterWindowMessage("MGEN_GEN_FINISH_MSG");
static UINT WM_DEBUG_MSG = RegisterWindowMessage("MGEN_DEBUG_MSG");

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_BUTTON_OUTPUTWND, &CMainFrame::ShowOutputWnd)
	ON_COMMAND(ID_BUTTON_PARAMS, &CMainFrame::OnButtonParams)
	ON_COMMAND(ID_BUTTON_GEN, &CMainFrame::OnButtonGen)
	ON_COMMAND(ID_BUTTON_PLAY, &CMainFrame::OnButtonPlay)
	ON_COMMAND(ID_BUTTON_SETTINGS, &CMainFrame::OnButtonSettings)
	ON_COMMAND(ID_BUTTON_ALGO, &CMainFrame::OnButtonAlgo)
	ON_COMMAND(ID_CHECK_OUTPUTWND, &CMainFrame::OnCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_OUTPUTWND, &CMainFrame::OnUpdateCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_COMBO_ALGO, &CMainFrame::OnUpdateComboAlgo)
	ON_COMMAND(ID_COMBO_ALGO, &CMainFrame::OnComboAlgo)
	ON_REGISTERED_MESSAGE(WM_GEN_FINISH, &CMainFrame::OnGenFinish)
	ON_REGISTERED_MESSAGE(WM_DEBUG_MSG, &CMainFrame::OnDebugMsg)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN, &CMainFrame::OnUpdateButtonGen)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_COMBO_MIDIOUT, &CMainFrame::OnUpdateComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EPARAMS, &CMainFrame::OnUpdateButtonEparams)
	ON_COMMAND(ID_BUTTON_EPARAMS, &CMainFrame::OnButtonEparams)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PLAY, &CMainFrame::OnUpdateButtonPlay)
	ON_COMMAND(ID_COMBO_MIDIOUT, &CMainFrame::OnComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ALGO, &CMainFrame::OnUpdateButtonAlgo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PARAMS, &CMainFrame::OnUpdateButtonParams)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SCONFIG, &CMainFrame::OnUpdateButtonSconfig)
	ON_COMMAND(ID_BUTTON_SCONFIG, &CMainFrame::OnButtonSconfig)
	ON_UPDATE_COMMAND_UI(ID_SPIN_PSPEED, &CMainFrame::OnUpdateSpinPspeed)
	ON_COMMAND(ID_SPIN_PSPEED, &CMainFrame::OnSpinPspeed)
	ON_COMMAND(ID_SPIN_ZOOM, &CMainFrame::OnSpinZoom)
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OPENMIDI, &CMainFrame::OnUpdateButtonOpenmidi)
	ON_COMMAND(ID_BUTTON_OPENMIDI, &CMainFrame::OnButtonOpenmidi)
	ON_COMMAND(ID_BUTTON_EDITALGO, &CMainFrame::OnButtonEditalgo)
	ON_COMMAND(ID_BUTTON_EDITINST, &CMainFrame::OnButtonEditinst)
	ON_COMMAND(ID_BUTTON_RELOADALGO, &CMainFrame::OnButtonReloadalgo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RELOADALGO, &CMainFrame::OnUpdateButtonReloadalgo)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
	if (pGen != 0) delete pGen;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2 = "Release";
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	//bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	//ASSERT(bNameValid);

#if defined(_DEBUG)
	strTitlePane2 = "Debug";
#endif

	CMFCRibbonStatusBarPane* pPane = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
	pPane->SetAlmostLargeText(_T("Connecting Connecting Connecting Connecting Connecting Connecting"));
	m_wndStatusBar.AddElement(pPane, strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	LoadAlgo();

	// MIDI port
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,	m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	CString st;
	int default_out = Pm_GetDefaultOutputDeviceID();
	MidiCount = 0;
	for (int i = 0; i < Pm_CountDevices(); i++) {
		const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
		if (info->output) {
			st.Format("%s, %s", info->name, info->interf);
			if (i == default_out) {
				//st += " (default)";
			}
			pCombo->AddItem(st, i);
			MidiName[MidiCount] = st;
			MidiCount++;
		}
	}

	WriteLog(0, "Started MGen version 1.1.5");
	AfxInitRichEdit2();

	LoadSettings();
	//m_wndRibbonBar.GetQuickAccessToolbar()->SetCompactMode(1);

	CGLib::can_send_log = 0;
	CGLib::m_hWnd = m_hWnd;
	CGLib::WM_DEBUG_MSG = WM_DEBUG_MSG;

	return 0;
}

void CMainFrame::WriteLog(int log, CString st)
{
	COutputList* pOL=0;
	if (log == 0) pOL = &m_wndOutput.m_wndOutputDebug;
	if (log == 1) pOL = &m_wndOutput.m_wndOutputWarn;
	if (log == 2) pOL = &m_wndOutput.m_wndOutputPerf;
	if (log == 3) pOL = &m_wndOutput.m_wndOutputAlgo;
	if (log == 4) pOL = &m_wndOutput.m_wndOutputMidi;
	pOL->AddString(CTime::GetCurrentTime().Format("%H:%M:%S") + " " + st);
	if (pOL->GetCount() > 1000) pOL->DeleteString(0);
	pOL->SetTopIndex(pOL->GetCount() - 1);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

void CMainFrame::ShowOutputWnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnButtonParams()
{
	if (m_state_gen != 1 && m_algo > -1 && m_config != "") {
		CString path = "configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl";
		::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
}

void CMainFrame::LoadResults(CString path) {
	if (m_state_gen == 1) {
		AfxMessageBox("Please stop generation before opening saved results");
		return;
	}
	string::size_type pos = string(path).find_last_of("\\/");
	CString dir = string(path).substr(0, pos).c_str();
	string::size_type pos2 = string(path).find_last_of("./");
	CString fname = string(path).substr(pos + 1, pos2 - pos - 1).c_str();
	GetActiveDocument()->SetTitle(fname);
	m_fname = fname;
	m_dir = dir;
	// Stop playback
	if (m_state_play > 0) OnButtonPlay();
	m_state_play = 0;
	// Delete generator
	if (pGen != 0) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
	}
	pGen = 0;
	pGen = new CGenRS1();
	if (pGen != 0) {
		WriteLog(0, _T("Loading file: ") + path);
		// Set pGen variables
		CGLib::can_send_log = 1;
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		// Initialize MIDI
		pGen->StopMIDI();
		pGen->StartMIDI(GetMidiI(), 100, 0);
		pGen->time_started = TIME_PROC(TIME_INFO);
		pGen->InitRandom();
		// Load results
		pGen->LoadResults(dir, fname);
		if (pGen->save_format_version != "1.2") MessageBox("This file was created with a different format version " + pGen->save_format_version + "\nYou can get errors trying to load it.", "Error");
		m_algo_id = pGen->m_algo_id;
		if (GetAlgoById(m_algo_id) > -1) m_algo = GetAlgoById(m_algo_id);
		pGen->m_algo_insts = AlgInsts[m_algo];
		m_config = pGen->m_config;
		// Load configs
		pGen->LoadInstruments();
		pGen->LoadConfig("configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
		pGen->LoadConfig(dir + "\\" + fname + ".pl");
		// Adapt
		pGen->Adapt(0, pGen->t_sent - 1);
		m_state_gen = 2;
		GetActiveView()->SetFocus();
		Invalidate();
		GetActiveView()->Invalidate();
	}
}

bool CMainFrame::NewDocument()
{
	if ((m_state_gen == 2) && (m_state_play == 0)) if (pGen != 0) {
		WriteLog(0, "Removing previous generator");
		delete pGen;
		m_state_gen = 0;
		pGen = 0;
		return true;
	}
	return false;
}

void CMainFrame::OnButtonGen()
{
	if (m_state_gen == 1) {
		if (pGen != 0) {
			pGen->need_exit = 1;
			WriteLog(0, "Sent need_exit to generation thread");
		}
		return;
	}
	if ((m_state_gen == 2) || (pGen != 0)) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
	}
	pGen = 0;
	if (m_algo_id == 101) pGen = new CGenCF1();
	if (m_algo_id == 102) pGen = new CGenCF2();
	if (m_algo_id == 111) pGen = new CGenCA1();
	if (m_algo_id == 121) pGen = new CGenCP1();
	if (m_algo_id == 1001) pGen = new CGenRS1();
	if (m_algo_id == 2001) pGen = new CGenMP1();
	if (pGen != 0) {
		WriteLog(0, _T("Started generator: ") + AlgName[m_algo]);
		// Clear current saved path
		m_fname = "";
		m_dir = "";
		// Set pGen variables
		CGLib::can_send_log = 1;
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		pGen->m_algo_id = m_algo_id;
		pGen->m_algo_insts = AlgInsts[m_algo];
		pGen->m_config = m_config;
		pGen->m_pspeed = m_pspeed;
		// Initialize variables
		pGen->InitRandom();
		pGen->LoadInstruments();
		pGen->LoadConfig("configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
		pGen->InitVectors();
		// Initialize MIDI
		pGen->StopMIDI();
		pGen->StartMIDI(GetMidiI(), 100, 0);
		pGen->time_started = TIME_PROC(TIME_INFO);
		// Start generation
		m_GenThread = AfxBeginThread(CMainFrame::GenThread, pGen);
		m_state_gen = 1;
		m_state_play = 0;
		// Start timer
		SetTimer(TIMER1, m_view_timer, NULL);
		if (pGen->shuffle == 0) SetTimer(TIMER2, 1000, NULL);
	}
}

void CMainFrame::OnButtonSettings()
{
}


void CMainFrame::OnButtonAlgo()
{
	if (m_state_gen != 1) {
		CAlgoDlg dlg;
		dlg.DoModal();
	}
}


void CMainFrame::OnCheckOutputwnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

LRESULT CMainFrame::OnGenFinish(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0) {
		GetActiveView()->Invalidate();
		WriteLog(0, "Generation finished");
		if (m_state_play == 0) ::KillTimer(m_hWnd, TIMER1);
		m_state_gen = 2;
		// Save results
		CString fname = CTime::GetCurrentTime().Format("%Y-%m-%d-auto %H-%M-%S");
		CString dir = "autosaves\\" + AlgFolder[m_algo] + "\\" + fname;
		CreateDirectory("autosaves", NULL);
		CreateDirectory("autosaves\\" + AlgFolder[m_algo], NULL);
		pGen->SaveResults(dir, fname);
		pGen->SaveMidi(dir, fname);
		m_dir = dir;
		m_fname = fname;
		GetActiveDocument()->SetTitle(fname);
		// Copy config
		CGLib::copy_file("configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl", dir + "\\" + fname + ".pl");
		// Append config name
		CGLib::AppendLineToFile(dir + "\\" + fname + ".pl",
			"\n# This config was copied from file " + AlgFolder[m_algo] + "\\" + m_config + ".pl\n");
		CGLib::AppendLineToFile(dir + "\\" + fname + ".pl",
			"# Originally autosaved at " + dir + "\n");
		CGLib::AppendLineToFile(dir + "\\" + fname + ".pl",
			"Instruments = " + pGen->m_algo_insts + " # Original instruments of current algorithm saved");
		//WriteLog(1, "configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
		//WriteLog(1, dir + "\\config.pl");
		// Start playback after shuffle
		if (pGen->shuffle) {
			GetActiveView()->Invalidate();
			if ((m_state_play == 0) && (pGen->t_sent > 0)) OnButtonPlay();
		}
	}
	if (wParam == 1) {
		// This message is sent from generation thread when t_sent is increased
		// Now it is processed OnTimer
	}
	return 0;
}

LRESULT CMainFrame::OnDebugMsg(WPARAM wParam, LPARAM lParam)
{
	CString* pSt = (CString*)lParam;
	WriteLog(wParam, *pSt);
	delete pSt;
	return LRESULT();
}

void CMainFrame::OnUpdateCheckOutputwnd(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_wndOutput.IsVisible();
	pCmdUI->Enable();
	pCmdUI->SetCheck(bEnable);
}


void CMainFrame::OnUpdateComboAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

int CMainFrame::GetMidiI()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	int i = pCombo->GetCurSel();
	if (i == -1) return -1;
	else return pCombo->GetItemData(i);
}

void CMainFrame::LoadAlgo()
{
	ifstream fs;
	fs.open("algorithms.txt");
	CString st, st2, st3, st4, st5;
	char pch[2550];
	int pos = 0;
	// Load header
	fs.getline(pch, 2550);
	ParamName.clear();
	for (int i = 0; i < MAX_ALGO; i++) ParamCount[i] = 0;
	AlgCount = 0;
	if (AlgGCount > 0) for (int i = 0; i < AlgGCount; i++) AlgGroups[i] = "@#(*&#$%@&*(^@#)(&*@$#)(*&%@#)&*#$";
	AlgGCount = 0;
	while (fs.good()) {
		pos = 0;
		fs.getline(pch, 2550);
		st = pch;
		st.Trim();
		if (st.Find("|") != -1) {
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgID[AlgCount] = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgFolder[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgName[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgGroup[AlgCount] = st2;
			// Load groups
			CString* pst = find(begin(AlgGroups), end(AlgGroups), st2);
			if (*pst != st2) {
				AlgGroups[AlgGCount] = st2;
				AlgGCount++;
			}
			// Load instruments
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgInsts[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgComment[AlgCount] = st2;
			// Check if folder exists
			CString path = "configs\\" + AlgFolder[AlgCount];
			if (!CGLib::dirExists(path)) {
				CreateDirectory(path, NULL);
				ofstream fs;
				fs.open(path + "\\Test.pl");
				fs.close();
			}
			// Get all configs
			DIR *dir;
			struct dirent *ent;
			ParamName.push_back(vector<CString>());
			if ((dir = opendir(path)) != NULL) {
				while ((ent = readdir(dir)) != NULL) {
					CString st = ent->d_name;
					if ((st.Left(1) != ".") && (st.Right(3) == ".pl")) {
						ParamName[AlgCount].push_back(st.Mid(0, st.GetLength()-3));
						ParamCount[AlgCount]++;
					}
				}
				closedir(dir);
			}
			AlgCount++;
		}
	}
	fs.close();
	if (AlgCount == 0) {
		WriteLog(1, "Error loading algorithms.txt");
	}
}

void CMainFrame::LoadSettings()
{
	ifstream fs;
	fs.open("settings.pl");
	CString st, st2, st3;
	char pch[2550];
	int pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			int idata = atoi(st3);
			if (st2 == "algorithm") {
				CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
					m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
				m_algo_id = idata;
				if (GetAlgoById(m_algo_id) > -1) m_algo = GetAlgoById(m_algo_id);
			}
			if (st2 == "midi_out") {
				CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
					m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
				//int id = distance(MidiName, find(begin(MidiName), end(MidiName), st3));
				pCombo->SelectItem(st3);
			}
			if (st2 == "playback_speed") {
				CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_PSPEED));
				pRibbonSpin->SetEditText(st3);
				m_pspeed = atoi(st3);
			}
			if (st2 == "horizontal_zoom") {
				CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_ZOOM));
				pRibbonSpin->SetEditText(st3);
				zoom_x = atoi(st3);
			}
			CGLib::CheckVar(&st2, &st3, "view_timer", &m_view_timer, MIN_VIEW_TIMER, MAX_VIEW_TIMER);
			CGLib::CheckVar(&st2, &st3, "step_dyn", &m_step_dyn);
			CGLib::LoadVar(&st2, &st3, "config", &m_config);
			//CGLib::LoadVar(&st2, &st3, "midi_program", &midi_program);
		}
	}
	fs.close();
}

int CMainFrame::GetAlgoById(int id) {
	int* found = find(begin(AlgID), end(AlgID), id);
	if (*found == id) {
		return distance(AlgID, found);
	}
	return -1;
}

void CMainFrame::SaveSettings()
{
	ofstream fs;
	fs.open("settings.pl");
	CString st;
	fs << "# Settings of MGen\n";
	fs << "# This file is loaded on MGen startup and automatically saved on every setting change\n";
	fs << "\n";
	st.Format("Algorithm = %d # Id of the currently selected algorithm\n", m_algo_id);
	fs << st;
	st.Format("Config = %s # Name of configuration file for selected algorithm\n", m_config);
	fs << st;
	int i = GetMidiI();
	//st.Format("MIDI_OUT_ID = %d\n", i);
	//fs << st;
	if (i > -1) {
		const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
		st.Format("MIDI_OUT = %s, %s # Name of MIDI device used for playing notes\n", info->name, info->interf);
		fs << st;
	}
	st.Format("Horizontal_zoom = %d # Zoom of the piano roll. Can be from 80 to 500\n", zoom_x);
	fs << st;
	st.Format("View_timer = %d # ms between each screen update during generation and playback. 100 ms is recommended. Increase for slower computers\n", m_view_timer);
	fs << st;
	st.Format("playback_speed = %d # Playback speed in percent\n", m_pspeed);
	fs << st;
	st.Format("Step_dyn = %d # Show dynamics with note opacity for each step of note.Disable for slower computers.\n", m_step_dyn);
	fs << st;
	//st.Format("Midi_program = %s # Path to program to use to open MIDI file. Leave blank to use default OS file association.\n", midi_program);
	//fs << st;
	fs.close();
}

void CMainFrame::OnComboAlgo()
{
	SaveSettings();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CFrameWndEx::OnTimer(nIDEvent);
	if (nIDEvent == TIMER1)
	{
		GetActiveView()->Invalidate();
		//int play_time = TIME_PROC(TIME_INFO) - pGen->midi_start_time;
		if ((m_state_gen != 1) && (m_state_play == 2) && (TIME_PROC(TIME_INFO) > pGen->midi_sent_t)) {
			OnButtonPlay();
		}
	}
	if ((nIDEvent == TIMER2) && (pGen != 0))
	{
		if (pGen->t_sent < 2) return;
		// Decide if we can play
		if (m_state_play == 0) {
			if (m_state_gen == 2) {
				m_state_play = 1;
				SetTimer(TIMER1, m_view_timer, NULL);
			}
			if (m_state_gen == 1) {
				if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(1000))) {
					WriteLog(4, "OnGenFinish mutex timed out: playback not started");
					return;
				}
				// Check generation speed
				double gtime = pGen->stime[pGen->t_sent - 1]*100/pGen->m_pspeed;
				double ptime = TIME_PROC(TIME_INFO) - pGen->time_started;
				if ((gtime / ptime > 2) || (gtime > 30000) || ((gtime / ptime > 1.2) && (gtime > 5000))) {
					m_state_play = 1;
					SetTimer(TIMER1, m_view_timer, NULL);
				}
				else {
					CString st;
					st.Format("Only %.1f s generated after %.1f s. Playback is postponed to avoid buffer underruns", gtime / 1000, ptime / 1000);
					WriteLog(4, st);
				}
				pGen->mutex_output.unlock();
			}
		}
		if (m_state_play == 1) {
			if ((pGen->t_sent > pGen->midi_sent)) {
				//CString st;
				//st.Format("Chunk received up to %d", pGen->t_sent);
				//WriteLog(0, st);
				pGen->m_pspeed = m_pspeed;
				// If generation finished set last run
				if (m_state_gen == 2) pGen->midi_last_run = 1;
				pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
			}
			if ((pGen->buf_underrun) || ((pGen->t_sent == pGen->midi_sent) && (m_state_gen == 2))) {
				::KillTimer(m_hWnd, TIMER2);
				m_state_play = 2;
				//GetActiveView()->Invalidate();
			}
		}
	}
}

UINT CMainFrame::GenThread(LPVOID pParam)
{
	CGMidi* pGen = (CGMidi*)pParam;

	if (pGen == NULL) return 1;   // if Object is not valid  

	//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread started"));
	pGen->Generate();
	//Sleep(2000);
	::PostMessage(pGen->m_hWnd, WM_GEN_FINISH, 0, 0);
	//pGen->time_stopped = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	pGen->time_stopped = TIME_PROC(TIME_INFO);

	//if (pGen->can_send_log)
		//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread stopped"));
	return 0;   // thread completed successfully 
}

void CMainFrame::OnClose()
{
	if (m_state_gen == 1) {
		CGLib::can_send_log = 0;
		OnButtonGen();
		WaitForSingleObject(m_GenThread->m_hThread, 10000);
		delete pGen;
		pGen = 0;
	}
	if (pGen != 0) {
		CGLib::can_send_log = 0;
		delete pGen;
		pGen = 0;
	}
	Pm_Terminate();

	CFrameWndEx::OnClose();
}



void CMainFrame::OnUpdateComboMidiout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnButtonEparams()
{
	//CEditParamsDlg dlg;
	//dlg.DoModal();
}


void CMainFrame::OnUpdateButtonPlay(CCmdUI *pCmdUI)
{
	if ((m_state_gen == 1) && (m_state_play == 0)) pCmdUI->Enable(0);
	else if (m_state_gen != 0) pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
	if (m_state_play > 0) pCmdUI->SetText("Stop Playback");
	else pCmdUI->SetText("Play");
}

void CMainFrame::OnUpdateButtonGen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	if (m_state_gen == 1) pCmdUI->SetText("Stop Generator");
	else pCmdUI->SetText("Generate");
}

void CMainFrame::OnButtonPlay()
{
	if (m_state_play > 0) {
		m_state_play = 0;
		if (pGen != 0) {
			::KillTimer(m_hWnd, TIMER2);
			if (m_state_gen == 2) ::KillTimer(m_hWnd, TIMER1);
			pGen->StopMIDI();
		}
		WriteLog(4, "Stopped playback");
	}
	else if ((m_state_gen == 2) && (GetMidiI() > -1)) {
		StartPlay(0);
	}
}

void CMainFrame::StartPlay(int from)
{
	if (pGen->m_pspeed != pGen->adapt_pspeed) pGen->Adapt(0, pGen->t_generated - 1);
	pGen->StopMIDI();
	pGen->StartMIDI(GetMidiI(), 100, from);
	m_state_play = 1;
	// Start timer
	pGen->m_pspeed = m_pspeed;
	// If generation finished set last run
	if (m_state_gen == 2) pGen->midi_last_run = 1;
	pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
	SetTimer(TIMER1, m_view_timer, NULL);
	SetTimer(TIMER2, 1000, NULL);
}

void CMainFrame::OnComboMidiout()
{
	SaveSettings();
}


void CMainFrame::OnUpdateButtonAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1);
	if (m_algo != -1) pCmdUI->SetText("Algorithm: " + AlgName[m_algo]);
	else pCmdUI->SetText("Algorithm: click to select");
}


void CMainFrame::OnUpdateButtonParams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1 && m_algo > -1 && m_config != "");
	if (m_config != "") pCmdUI->SetText("Config: " + m_config);
	else pCmdUI->SetText("Config: ");
}

void CMainFrame::OnUpdateButtonEparams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1 && m_algo > -1 && m_config != "");
}


BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam >= ID_FILE_MRU_FILE1 && wParam <= ID_FILE_MRU_FILE16)
	{
		int nIndex = wParam - ID_FILE_MRU_FILE1;
		CString path = ((CMGenApp*)::AfxGetApp())->getRecentFile(nIndex);
		LoadResults(path);
	}

	return CFrameWndEx::OnCommand(wParam, lParam);
}


void CMainFrame::OnUpdateButtonSconfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_fname != "") && (m_dir != ""));
}


void CMainFrame::OnButtonSconfig()
{
	if ((m_dir != "") && (m_fname != "")) {
		CString path = m_dir + "\\" + m_fname + ".pl";
		::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
}


void CMainFrame::OnUpdateSpinPspeed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_state_play == 0) && (m_state_gen != 1));
}


void CMainFrame::OnSpinPspeed()
{
	CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_PSPEED));
	CString st = pRibbonSpin->GetEditText();
	if (m_pspeed != atoi(st)) {
		m_pspeed = atoi(st);
		if (m_pspeed < MIN_PSPEED) m_pspeed = MIN_HZOOM;
		if (m_pspeed > MAX_PSPEED) m_pspeed = MAX_HZOOM;
		st.Format("%d", m_pspeed);
		pRibbonSpin->SetEditText(st);
		if (pGen != 0) pGen->m_pspeed = m_pspeed;
		GetActiveView()->Invalidate();
		SaveSettings();
	}
}


void CMainFrame::OnSpinZoom()
{
	CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_ZOOM));
	CString st = pRibbonSpin->GetEditText();
	if (zoom_x != atoi(st)) {
		zoom_x = atoi(st);
		if (zoom_x < MIN_HZOOM) zoom_x = MIN_HZOOM;
		if (zoom_x > MAX_HZOOM) zoom_x = MAX_HZOOM;
		st.Format("%d", zoom_x);
		pRibbonSpin->SetEditText(st);
		GetActiveView()->Invalidate();
		SaveSettings();
	}
}


void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMainFrame::OnUpdateButtonOpenmidi(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fname != "");
}


void CMainFrame::OnButtonOpenmidi()
{
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", m_dir + "\\" + m_fname + ".mid", NULL, NULL, SW_SHOWNORMAL);
}


void CMainFrame::OnButtonEditalgo()
{
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "algorithms.txt", NULL, NULL, SW_SHOWNORMAL);
}


void CMainFrame::OnButtonEditinst()
{
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "instruments.pl", NULL, NULL, SW_SHOWNORMAL);
}


void CMainFrame::OnButtonReloadalgo()
{
	LoadAlgo();
}


void CMainFrame::OnUpdateButtonReloadalgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1);
}
