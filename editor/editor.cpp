
#include "stdafx.h"
#include "editor.h"

SINGLE_ENTITY_COMPONENT(MainWnd2);

/**
 * class implementation
 */

MainWnd2* MainWnd2::instance = NULL;

CWinApp app;

MainWnd2::MainWnd2()
{
    assert( MainWnd2::instance == NULL );
    _font = NULL;
    _mainForm = NULL;
    _renderView = NULL;
    _helpDialog = NULL;
    MainWnd2::instance = this;
}

MainWnd2::~MainWnd2()
{
    assert( MainWnd2::instance != NULL );
    if( _mainForm ) 
    {
        _mainForm->renderView = NULL;
        _mainForm->helpDialog = NULL;
        if( _renderView ) delete _renderView;
        if( _helpDialog ) delete _helpDialog;
        delete _mainForm;
    }
    if( _font )
    {
        delete _font;
    }
    MainWnd2::instance = NULL;
}

/**
 * component support
 */

EntityBase* MainWnd2::creator()
{
    return new MainWnd2;
}

void MainWnd2::entityDestroy()
{
    delete this;
}

/**
 * EntityBase
 */
    
void MainWnd2::entityInit(Object* p)
{
    _font = new CFont;
    _font->CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Lucida Console" );

    _mainForm = new MainForm;
    _mainForm->Create( IDD_MAINFORM_DIALOG, NULL );
    _mainForm->ShowWindow( SW_SHOW );
    _mainForm->ShowWindow( SW_MAXIMIZE );

    // set "Lucida Console" font for workspace boxes
    _mainForm->consoleBox.SetFont( _font );
    _mainForm->commandBox.SetFont( _font );

    _renderView = new RenderView( _mainForm );
    _renderView->Create( IDD_RENDERVIEW_DIALOG, _mainForm );
    _renderView->ShowWindow( SW_SHOW );
    _mainForm->renderView = _renderView;

    _helpDialog = new HelpDialog( _mainForm );	
    _helpDialog->Create( IDD_HELPDIALOG_DIALOG, _mainForm );
    _mainForm->helpDialog = _helpDialog;
    
    RECT rect;
    _mainForm->GetWindowRect( &rect );
    _mainForm->SetWindowPos( NULL, rect.left, rect.top, rect.right, rect.bottom, 0 );

    // core parampack
    IParamPack* corePack = icore->getCoreParamPack();

    // store window handle in core parampack
    corePack->set(
        "MainWnd2.handle",
        getHandle()
    );

    // store viewport properties
    RECT viewRect;
    _renderView->GetClientRect( &viewRect );
    corePack->set( "engine.viewport.width", viewRect.right - viewRect.left );
    corePack->set( "engine.viewport.height", viewRect.bottom - viewRect.top );
}

void MainWnd2::entityAct(float dt)
{
	if( _mainForm->getCurrentBSP() != NULL )
	{
		_renderView->renderScene( _mainForm->getCurrentBSP() );
	}
}

void MainWnd2::entityHandleEvent(evtid_t id, trigid_t trigId, Object* param)
{
}

IBase* MainWnd2::entityAskInterface(iid_t id)
{
    if( id == MainWnd2::IMainWnd::iid ) return this;
    return NULL;
}

/**
 * IMainWnd2
 */

int MainWnd2::getHandle(void)
{
    return (int)( _renderView->m_hWnd );
}