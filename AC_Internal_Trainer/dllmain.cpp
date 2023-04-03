// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include <iostream>
#include "globalOffsets.h"

DWORD WINAPI HackThread(HMODULE hModule) {
    //Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);


    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");


    bool bHealth = false, bAmmo = false, bRecoil = false, bArmor = false;

    while (true)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1){
            break;
        }

        //P
        if (GetAsyncKeyState(0x50) & 1) {
            bHealth = !bHealth;
        }

        //O
        if (GetAsyncKeyState(0x4F) & 1) {
            bArmor = !bArmor;
        }

        //I
        if (GetAsyncKeyState(0x49) & 1){
            bAmmo = !bAmmo;
        }

        //no recoil NOP
        //U
        if (GetAsyncKeyState(0x55) & 1){
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::Nop((BYTE*)(moduleBase + OFFSET_WRITE_RECOIL), 10);
            }
            else {
                //50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
                mem::Patch((BYTE*)(moduleBase + OFFSET_WRITE_RECOIL), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }

        //Continuous write/freeze
        uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + OFFSET_PLAYER_CLASS);
        if (localPlayerPtr) {
            if (bHealth) {
                *(int*)(*localPlayerPtr + OFFSET_HEALTH) = 1337;
            }

            if (bArmor) {
                *(int*)(*localPlayerPtr + OFFSET_ARMOR) = 1337;
            }

            if (bAmmo) {
                uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + OFFSET_PLAYER_CLASS, {OFFSET_RIFLE_AMMO});
                int* ammo = (int*)ammoAddr;
                *ammo = 1337;


            }
        }
        Sleep(5);

    }

    //Cleanup and eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

