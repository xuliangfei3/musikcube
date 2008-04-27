//////////////////////////////////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2007, Daniel �nnerby
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
//////////////////////////////////////////////////////////////////////////////
#include "pch.hpp"

#include "ApplicationThread.hpp"

//////////////////////////////////////////////////////////////////////////////

using namespace win32cpp;

//////////////////////////////////////////////////////////////////////////////

ApplicationThread::ApplicationThread(void)
: helperWindow(NULL)
{
    this->applicationThreadId   = GetCurrentThreadId();
}

ApplicationThread::~ApplicationThread(void){
    delete this->helperWindow;
}

bool ApplicationThread::InMainThread(){
    ApplicationThread *thread=Application::Instance().thread;
    if(thread){
        DWORD theThread = GetCurrentThreadId();
        return thread->applicationThreadId==theThread;
    }
    return false;
}


void ApplicationThread::AddCall(CallClassBase *callClass){
    this->calls.push_back(CallClassPtr(callClass));
    this->NotifyMainThread();
}

void ApplicationThread::MainThreadCallback(){
    boost::mutex::scoped_lock lock(this->mutex);
    while(!this->calls.empty()){
        CallClassPtr currentCall    = this->calls.front();
        currentCall->Call();
        this->calls.pop_front();
    }
}

void ApplicationThread::NotifyMainThread(){
    if(this->helperWindow){
        ::PostMessage(this->helperWindow->Handle(),WM_USER+1,NULL,NULL);
    }
}

void ApplicationThread::Initialize(){
    // Create helper window
    this->helperWindow  = new ApplicationThread::HelperWindow();

    this->helperWindow->Initialize();
}


///\brief
///Constructor for HelperWindow
ApplicationThread::HelperWindow::HelperWindow(){
}

///\brief
///Create HelperWindow
HWND ApplicationThread::HelperWindow::Create(Window* parent){
    HINSTANCE hInstance = Application::Instance();

    // create the window
    DWORD style = WS_CHILD;
    //
    HWND hwnd = CreateWindowEx(
        NULL,                   // ExStyle
        _T("Message"),          // Class name
        _T("ThreadHelperWindow"),     // Window name
        style,                  // Style
        0,                      // X
        0,                      // Y
        0,                      // Width
        0,                      // Height
        HWND_MESSAGE,           // Parent
        NULL,                   // Menu
        hInstance,              // Instance
        NULL);                  // lParam

    return hwnd;
}

///\brief
///HelperWindow message handler.
LRESULT ApplicationThread::HelperWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam){
    switch (message)
    {
    case WM_USER+1:
        // This is a ApplicationTread message
        ApplicationThread *thread    = Application::Instance().thread;
        if(thread){
            thread->MainThreadCallback();
        }
        return 0;   // 0 = processed
    }

    return this->DefaultWindowProc(message, wParam, lParam);
}

void ApplicationThread::HelperWindow::OnCreated(){
    ApplicationThread *thread    = Application::Instance().thread;
    if(thread){
        thread->MainThreadCallback();
    }
}
