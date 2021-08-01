// ConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Server

#include <iostream>
#include <Windows.h>
#include <iostream>
#include <optional>
#include <chrono>

//Global Handle for Semaphore
HANDLE k2api_to_Semaphore,
k2api_from_Semaphore,
k2api_start_Semaphore;

int main()
{
	// Create the *to* semaphore
	{
		k2api_to_Semaphore = CreateSemaphoreA(
			NULL, //Security Attributes
			0,	  //Initial State i.e.Non Signaled
			1,    //No. of Resources
			"Global\\k2api_to_sem");//Semaphore Name

		if (NULL == k2api_to_Semaphore)
		{
			std::cout << "Semaphore Creation Failed\n";
			std::cout << "Error No - " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Creation Success\n";

		// Automatically release the sem after creation
		ReleaseSemaphore(k2api_to_Semaphore, 1, 0);
	}

	// Create the *from* semaphore
	{
		k2api_from_Semaphore = CreateSemaphoreA(
			NULL, //Security Attributes
			0,    //Initial State i.e.Non Signaled
			1,    //No. of Resources
			"Global\\k2api_from_sem");//Semaphore Name

		if (NULL == k2api_from_Semaphore)
		{
			std::cout << "Semaphore Creation Failed\n";
			std::cout << "Error No - " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Creation Success\n";
	}

	// Create the *start* semaphore
	{
		k2api_start_Semaphore = CreateSemaphoreA(
			NULL, //Security Attributes
			0,    //Initial State i.e.Non Signaled
			1,    //No. of Resources
			"Global\\k2api_start_sem");//Semaphore Name

		if (NULL == k2api_start_Semaphore)
		{
			std::cout << "Semaphore Creation Failed\n";
			std::cout << "Error No - " << GetLastError() << '\n';

			system("PAUSE");
			return 0;
		}
		std::cout << "Semaphore Creation Success\n";
	}

	// Repeat the task
	while (true) {
		using clock = std::chrono::system_clock;
		using ms = std::chrono::duration<double, std::milli>;
		/*************************************************************************/


		// Wait for the client to request a read
		while (WaitForSingleObject(k2api_start_Semaphore, 5000L) != WAIT_OBJECT_0) {
			std::cout << "Releasing the *to* semaphore\n";
			ReleaseSemaphore(k2api_to_Semaphore, 1, 0);
		}

		const auto before = clock::now();

		/************************************/
		std::cout << "Reading the message!\n";
		/************************************/

		// Here, read from the *to* pipe
		/**/

		// Create the pipe file
		std::optional<HANDLE> ReaderPipe = CreateFile(
			TEXT("\\\\.\\pipe\\k2api_to_pipe"),
			GENERIC_READ | GENERIC_WRITE,
			0, nullptr, OPEN_EXISTING, 0, nullptr);

		// Create the buffer
		char read_buffer[1024];
		DWORD Read = DWORD();

		// Check if we're good
		if (ReaderPipe.has_value()) {

			// Read the pipe
			ReadFile(ReaderPipe.value(),
				read_buffer, 1024,
				&Read, nullptr);

			// Convert the message to string
			std::string read_string = read_buffer;

			// Write out the read string
			std::cout << read_string << '\n';
		}
		else
			std::cout << "Error: Pipe object was not initialized.";

		/**/

		/************************************/
		std::cout << "Sending the reply!\n";
		/************************************/

		// Here, write to the *from* pipe
		/**/

		HANDLE WriterPipe = CreateNamedPipe(
			TEXT("\\\\.\\pipe\\k2api_from_pipe"),
			PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			1, 1024, 1024, 1000L, nullptr);
		DWORD Written;

		// Compose the message
		std::string write_string = "Yoroshiku!";

		// Let the client know that we'll be writing soon
		ReleaseSemaphore(k2api_from_Semaphore, 1, 0);

		// Read from the pipe
		ConnectNamedPipe(WriterPipe, nullptr);
		WriteFile(WriterPipe,
			write_string.c_str(),
			strlen(write_string.c_str()),
			&Written, nullptr);
		FlushFileBuffers(WriterPipe);

		/**/


		CloseHandle(ReaderPipe.value());
		DisconnectNamedPipe(WriterPipe);
		CloseHandle(WriterPipe);

		/*************************************************************************/
		const ms duration = clock::now() - before;
		std::cout << "took " << duration.count() << "ms\n\n";
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
