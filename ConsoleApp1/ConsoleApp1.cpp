// ConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Client

#include <iostream>
#include <Windows.h>
#include <iostream>
#include <optional>

//Global Handle for Semaphore
HANDLE k2api_to_Semaphore,
k2api_from_Semaphore,
k2api_start_Semaphore;

int main()
{
	// Open existing *to* semaphore
	{
		k2api_to_Semaphore = OpenSemaphoreA(
			SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
			FALSE,
			"Global\\k2api_to_sem");//Semaphore Name

		if (NULL == k2api_to_Semaphore)
		{
			std::cout << "Semaphore Opening Failed\n";
			std::cout << "Error " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Opening Success\n";
	}

	// Open existing *from* semaphore
	{
		k2api_from_Semaphore = OpenSemaphoreA(
			SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
			FALSE,
			"Global\\k2api_from_sem");//Semaphore Name

		if (NULL == k2api_from_Semaphore)
		{
			std::cout << "Semaphore Opening Failed\n";
			std::cout << "Error " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Opening Success\n";
	}

	// Open existing *start* semaphore
	{
		k2api_start_Semaphore = OpenSemaphoreA(
			SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
			FALSE,
			"Global\\k2api_start_sem");//Semaphore Name

		if (NULL == k2api_start_Semaphore)
		{
			std::cout << "Semaphore Opening Failed\n";
			std::cout << "Error " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Opening Success\n";
	}

	// Repeat the task
	while (true) {
		/*************************************************************************/


		// Wait for the semaphore if it's locked
		WaitForSingleObject(k2api_to_Semaphore, INFINITE);

		/************************************/
		std::cout << "Sending the message!\n";
		/************************************/

		// Here, write to the *to* pipe
		/**/

		HANDLE API_WriterPipe = CreateNamedPipe(
			TEXT("\\\\.\\pipe\\k2api_to_pipe"),
			PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			1, 1024, 1024, 1000L, nullptr);
		DWORD Written;

		// Compose the message
		std::string API_write_string = "Konnichiwa!";

		// Let the server know we'll be writing soon
		ReleaseSemaphore(k2api_start_Semaphore, 1, 0);

		// Read from the pipe
		ConnectNamedPipe(API_WriterPipe, nullptr);
		WriteFile(API_WriterPipe,
			API_write_string.c_str(),
			strlen(API_write_string.c_str()),
			&Written, nullptr);
		FlushFileBuffers(API_WriterPipe);
		/**/

		/************************************/
		std::cout << "Reading the reply!\n";
		/************************************/

		// Here, read from the *from* pipe

		/**/
		// Wait for the server to request a response, max 1s
		if (WaitForSingleObject(k2api_from_Semaphore, 1000L) != WAIT_OBJECT_0) {
			std::cout << "Server didn't receive the message.\n";
			system("pause");
			exit(0);
		}

		// Create the pipe file
		std::optional<HANDLE> API_ReaderPipe = CreateFile(
			TEXT("\\\\.\\pipe\\k2api_from_pipe"),
			GENERIC_READ | GENERIC_WRITE,
			0, nullptr, OPEN_EXISTING, 0, nullptr);

		// Create the buffer
		char API_read_buffer[1024];
		DWORD Read = DWORD();

		// Check if we're good
		if (API_ReaderPipe.has_value()) {

			// Read the pipe
			ReadFile(API_ReaderPipe.value(),
				API_read_buffer, 1024,
				&Read, nullptr);

			// Convert the message to string
			std::string API_read_string = API_read_buffer;

			// Write out the read string
			std::cout << API_read_string << '\n';
		}
		else
			std::cout << "Error: Pipe object was not initialized.";

		/**/

		std::cout << "Blocking for 1s...\n\n"; ////
		Sleep(100L); /////////////////////////////

		DisconnectNamedPipe(API_WriterPipe);
		CloseHandle(API_WriterPipe);
		CloseHandle(API_ReaderPipe.value());

		// Unlock the semaphore after job done
		ReleaseSemaphore(k2api_to_Semaphore, 1, 0);

		/*************************************************************************/
	}

	// sem handle may be reused
	CloseHandle(k2api_to_Semaphore);

	system("PAUSE");
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
