/**
 * @file disconnect.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief .disconnect command
 * @details
 * @version 0.1
 * @date 2020-05-27
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern BOOLEAN g_IsConnectedToHyperDbgLocally;
extern BOOLEAN g_IsConnectedToRemoteDebuggee;
extern HANDLE  g_RemoteDebuggeeListeningThread;
extern HANDLE  g_EndOfMessageReceivedEvent;

/**
 * @brief help of the .disconnect command
 *
 * @return VOID
 */
VOID
CommandDisconnectHelp()
{
    ShowMessages(".disconnect : disconnects from a debugging session (it won't "
                 "unload the modules).\n\n");

    ShowMessages("syntax : \t.disconnect \n");
}

/**
 * @brief .disconnect command handler
 *
 * @param CommandTokens
 *
 * @return VOID
 */
VOID
CommandDisconnect(vector<CommandToken> CommandTokens)
{
    if (CommandTokens.size() != 1)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandDisconnectHelp();
        return;
    }

    if (!g_IsConnectedToHyperDbgLocally && !g_IsConnectedToRemoteDebuggee)
    {
        ShowMessages("you're not connected to any instance of HyperDbg, did you "
                     "use '.connect'? \n");
        return;
    }

    //
    // Check if it's local debugger and the driver is still
    // loading (not unloaded)
    //
    if (g_IsConnectedToHyperDbgLocally && g_DeviceHandle)
    {
        ShowMessages("you cannot disconnect in local debugging while the "
                     "driver is still loaded. please use 'unload' command before "
                     "disconnecting from the current instance of debugger\n");
        return;
    }

    //
    // Disconnect the session
    //
    g_IsConnectedToHyperDbgLocally = FALSE;

    //
    // This computer is connected to a remote system
    //
    if (g_IsConnectedToRemoteDebuggee)
    {
        //
        // We should kill the thread that was listening for the
        // remote commands and close the connection
        //
        TerminateThread(g_RemoteDebuggeeListeningThread, 0);
        CloseHandle(g_RemoteDebuggeeListeningThread);
        CloseHandle(g_EndOfMessageReceivedEvent);
        g_EndOfMessageReceivedEvent = NULL;

        RemoteConnectionCloseTheConnectionWithDebuggee();

        g_IsConnectedToRemoteDebuggee = FALSE;
    }

    ShowMessages("disconnected successfully\n");
}
