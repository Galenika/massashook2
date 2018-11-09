#include "Seq.h"
#include <urlmon.h>
#pragma comment (lib, "Urlmon.lib")
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

bool Security::bRunAllChecks()
{
	if (bCheckCommandline() && bCheckDomain())
		return true;
	else
		return false;

	return false;
}

bool Security::bCheckCommandline()
{
	return true;
}

bool Security::bCheckDomain()
{
	IStream* pStream = NULL;
	if (SUCCEEDED(URLOpenBlockingStreamA(0, "http://massas.pw/loader2/cool.txt", &pStream, 0, 0))) {
		pStream->Release();
		return true;
	}
	else {
		return false;
	}
	return false;
}