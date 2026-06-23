#include "headers.h"
#include <tchar.h>
#include "../shared/ccor.h"
#include "CoreImpl.h"

#include "Core.h"

using namespace ccor;


entid_t gMainEntId = -1;



static const void parseCommandLine(LPSTR cmdLine) {

        IParamPack * ppack = SingleCore::getInstance()->getCoreParamPack();

        std::string name;
        std::string value;
        const char * c = cmdLine;
        while (*c) {
                for (int i=0; i<2 && *c=='-'; ++c);
                if (*c) {
                        if (c[-1]=='-') {
                                const char* d = c;
                                while (*d && !isspace(*d) && *d!='=') 
                                        ++d;
                                if (*d=='=') {
                                        name.assign(c, d-c);
                                        c = ++d;
                                        while (*d && !isspace(*d)) ++d;
                                        value.assign(c, d-c);
                                        ppack->set((std::string("startup.")+name).c_str(), value.c_str());
                                }
                                else {
                                        name.assign(c, d-c);
                                        ppack->set((std::string("startup.")+name).c_str(), 1);
                                }
                                c = d;
                        }
                        else ++c;
                }
        }
}

//int PASCAL WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)

int CoreInitEngine(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
        int rcode = 0;

        try {
                ICore * c = SingleCore::getInstance();
                static_cast<CoreImpl*>(c)->init();
                IParamPack * ppack = c->getCoreParamPack();

                parseCommandLine(cmdLine);
                ppack->set("startup.arguments.instance", (int) instance);
                ppack->set("startup.arguments.prevInstance", (int) prevInstance);
                ppack->set("startup.arguments.cmdShow", (int) cmdShow);

                // Create main entity
                gMainEntId = c->createEntity("Main",-1,NULL);
        }
        catch(const CoreTerminateException& e) {
                SingleCore::getInstance()->logMessage("core: Exiting with code %d", e.code);
                rcode = e.code;
        }
        catch(const Exception& e) {
                SingleCore::getInstance()->logMessage("Exception! %s", e.getMsg());
                MessageBoxA(NULL,_T(e.getMsg()),_T("exception"),MB_OK | MB_ICONSTOP);
                rcode = 1;
#ifdef _DEBUG
#pragma warning(disable:4297)
                throw;
#pragma warning(default:4297)
#endif
        }
#ifndef _DEBUG
        catch(...) {
                SingleCore::getInstance()->logMessage("Unhandled exception!");
                MessageBoxA(NULL,_T(""),_T("unhandled exception"),MB_OK | MB_ICONSTOP);
                rcode = 1;
        }
#endif

        return rcode;
}


int CoreMainLoop()
{
        int rcode = 0;

        try {
                ICore * c = SingleCore::getInstance();

                // Run main entity
                static_cast<CoreImpl*>(c)->act();

                // Never be here...
                assert(false);

        }
        catch(const CoreTerminateException& e) {
                SingleCore::getInstance()->logMessage("core: Exiting with code %d", e.code);
                rcode = e.code;
        }
        catch(const Exception& e) {
                SingleCore::getInstance()->logMessage("Exception! %s", e.getMsg());
                MessageBoxA(NULL,_T(e.getMsg()),_T("exception"),MB_OK | MB_ICONSTOP);
                rcode = 1;
#ifdef _DEBUG
#pragma warning(disable:4297)
                throw;
#pragma warning(default:4297)
#endif
        }
#ifndef _DEBUG
        catch(...) {
                SingleCore::getInstance()->logMessage("Unhandled exception!");
                MessageBoxA(NULL,_T(""),_T("unhandled exception"),MB_OK | MB_ICONSTOP);
                rcode = 1;
        }
#endif

        return rcode;
}


int CoreShutdownEngine(int rcode)
{
        // Finalizing...

#ifndef _DEBUG
        try {
#endif
                SingleCore::getInstance()->destroyEntity(gMainEntId);
                SingleCore::releaseInstance();
#ifndef _DEBUG
        }
        catch(...) {
                MessageBoxA(NULL,_T(""),_T("unhandled exception while finalizing application"),MB_OK | MB_ICONSTOP);
                rcode = 1;
        }
#endif

        return rcode;
}


int CoreRunEngine(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
        // --------------------------------------------
        // MEMORY LEAKING? - use allocation breakpoint!
        //_crtBreakAlloc = 66909;

        int rcode = 0;

        rcode = CoreInitEngine(instance, prevInstance, cmdLine, cmdShow);
        if (rcode == 0) {
                rcode = CoreMainLoop();
        }
        rcode = CoreShutdownEngine(rcode);

        // memory leaks?
        // damn... fails because of a lot of static std templates :-(
        //_CrtDumpMemoryLeaks();

        return rcode;
}
