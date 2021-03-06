// WinsockPipesSimulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
// This program is designed to run pipes using threads in place of Winsock sockets to simulate network communications.

#include "pch.h"
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

//For sleep function for testing.
#include <synchapi.h>

//For threads.
#include <process.h>

//"server" thread.
unsigned int __stdcall myServer(void* data)
{
	printf("Thread %d: Creating named pipe...\n", GetCurrentThreadId());
	HANDLE pipe = CreateNamedPipe(L"\\\\.\\pipe\\test_pipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE, 1, 0, 0, 0, NULL);
	if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
	{
		printf("Thread %d: Failed to create pipe instance. Exiting...\n", GetCurrentThreadId());
		return 1;
	}
	printf("Thread %d: Pipe successfully created.\n", GetCurrentThreadId());

	printf("Thread %d: Awaiting an incoming connection...\n", GetCurrentThreadId());
	BOOL result = ConnectNamedPipe(pipe, NULL);
	if (!result)
	{
		printf("Thread %d: Failed to connect to incoming connection...\n", GetCurrentThreadId());
		CloseHandle(pipe);
		return 1;
	}
	printf("Thread %d: Connected successfully.\n", GetCurrentThreadId());

	printf("Thread %d: Sending data through pipe...\n", GetCurrentThreadId());
	const wchar_t *msg = L"Hello Mr. Client.";
	DWORD numBytesWritten = 0;
	result = WriteFile(pipe, msg, wcslen(msg) * sizeof(wchar_t), &numBytesWritten, NULL);
	if (result)
	{
		printf("Thread %d: Number of bytes sent: %d\n", GetCurrentThreadId(), numBytesWritten);
	}
	else
	{
		printf("Thread %d: Failed to send data.\n", GetCurrentThreadId());
	}

	printf("Thread %d: Reading data from pipe...\n", GetCurrentThreadId());
	wchar_t buffer[128];
	DWORD numBytesRead = 0;
	result = ReadFile(pipe, buffer, 127 * sizeof(wchar_t), &numBytesRead, NULL);
	if (result)
	{
		printf("Thread %d: Number of bytes read: %d\n", GetCurrentThreadId(), numBytesRead);
		buffer[numBytesRead / sizeof(wchar_t)] = '\0';
		printf("Thread %d: Message: %ls\n", GetCurrentThreadId(), buffer);
	}
	else
	{
		printf("Thread %d: Failed to read from pipe.\n", GetCurrentThreadId());
	}

	printf("Thread %d: Finished pipe operations. Exiting...\n", GetCurrentThreadId());
	CloseHandle(pipe);
	Sleep(5000);
	return 0;
}

//"client" thread.
unsigned int __stdcall myClient(void* data)
{
	printf("Thread %d: Connecting to server pipe...\n", GetCurrentThreadId());
	HANDLE pipe = CreateFile(L"\\\\.\\pipe\\test_pipe", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		printf("Thread %d: Failed to connect to named pipe. Exiting...\n", GetCurrentThreadId());
		return 1;
	}
	printf("Thread %d: Successfully connected to named pipe.\n", GetCurrentThreadId());

	printf("Thread %d: Reading from pipe...\n", GetCurrentThreadId());
	wchar_t buffer[128];
	DWORD numBytesRead = 0;
	BOOL result = ReadFile(pipe, buffer, 127 * sizeof(wchar_t), &numBytesRead, NULL);
	if (result)
	{
		buffer[numBytesRead / sizeof(wchar_t)] = '\0';
		printf("Thread %d: Number of bytes read: %d\n", GetCurrentThreadId(), numBytesRead);
		printf("Thread %d: Message: %ls\n", GetCurrentThreadId(), buffer);
	}
	else
	{
		printf("Thread %d: Failed to read from pipe.\n", GetCurrentThreadId());
	}

	printf("Thread %d: Sending data through pipe...\n", GetCurrentThreadId());
	const wchar_t *msg = L"Hello Mr. Server.";
	DWORD numBytesWritten = 0;
	result = WriteFile(pipe, msg, wcslen(msg) * sizeof(wchar_t), &numBytesWritten, NULL);
	if (result)
	{
		printf("Thread %d: Number of bytes sent: %d\n", GetCurrentThreadId(), numBytesWritten);
	}
	else
	{
		printf("Thread %d: Failed to send data.\n", GetCurrentThreadId());
	}

	printf("Thread %d: Finished pipe operations. Exiting...\n", GetCurrentThreadId());
	CloseHandle(pipe);
	Sleep(5000);
	return 0;
}

int main()
{
	//Create the threads to run as "server" and "client"
	printf("Thread %d: Initializing threads...\n", GetCurrentThreadId());
	HANDLE serverHandle, clientHandle;
	unsigned serverID, clientID;
	serverHandle = (HANDLE)_beginthreadex(0, 0, &myServer, 0, 0, &serverID);
	Sleep(3000);
	clientHandle = (HANDLE)_beginthreadex(0, 0, &myClient, 0, 0, &clientID);

	printf("Thread %d: Waiting for threads to terminate...\n", GetCurrentThreadId());
	WaitForSingleObject(serverHandle, INFINITE);
	CloseHandle(serverHandle);
	WaitForSingleObject(clientHandle, INFINITE);
	CloseHandle(clientHandle);
	printf("Thread %d: Threads terminated. Exiting...\n", GetCurrentThreadId());

	Sleep(5000);
	
	return 0;
}