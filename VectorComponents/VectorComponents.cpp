// WindowsProject1.cpp : Defines the entry point for the application.
//
#include "Framework.h"
#include "GridElement.h"
#include "Resource.h"
#include "VectorComponents.h"
#include "VectorComponentsElement.h"

#define MAX_LOADSTRING 100

namespace VectorComponents {
	HINSTANCE instance;
	WCHAR title[MAX_LOADSTRING];
	WCHAR windowClass[MAX_LOADSTRING];
	VectorComponentsElement* vectorComponentsElement;

	int initInstance(HINSTANCE, int);
}

int APIENTRY wWinMain(_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ LPWSTR    cmdLine,
	_In_ int       cmdShow)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	LoadString(instance, IDS_APP_TITLE, VectorComponents::title, MAX_LOADSTRING);
	LoadString(instance, IDC_VECTORCOMPONENTS, VectorComponents::windowClass, MAX_LOADSTRING);
	VectorComponents::vectorComponentsElement = new VectorComponentsElement(instance);
	VectorComponents::vectorComponentsElement->RegisterWindowClass(instance);

	if (!VectorComponents::initInstance(instance, cmdShow))
	{
		return 0;
	}

	MSG message;

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	delete VectorComponents::vectorComponentsElement;

	return (int)message.wParam;
}

namespace VectorComponents {
	int initInstance(HINSTANCE _instance, int cmdShow)
	{
		instance = _instance;

		const auto window = CreateWindow(windowClass, title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, _instance, VectorComponents::vectorComponentsElement);

		if (!window)
		{
			return 0;
		}

		ShowWindow(window, cmdShow);
		UpdateWindow(window);

		SetTimer(window, 42, 1000 / 25, nullptr);

		return 1;
	}
}
