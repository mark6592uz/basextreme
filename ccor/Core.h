#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int CoreInitEngine(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow);
int CoreMainLoop();
int CoreShutdownEngine(int rcode);


int CoreRunEngine(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow);
