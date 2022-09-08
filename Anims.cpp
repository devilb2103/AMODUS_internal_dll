#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
json jsonx;

//nice addresses
DWORD base = (DWORD)GetModuleHandleA("GameAssembly.dll");
DWORD base_pointer = base + 0x1BE0FE0;
DWORD medScan = base + 0x8F06B0;
DWORD player_control_address = base + 0x1BE1DC4;
DWORD ship_control_address = base + 0x1BEE81C;
DWORD rpc_play_animation = base + 0x65A7E0;
DWORD CmdSetColorAddress = base + 0x656EE0; //cmdCheckColor
DWORD RpcSetColorAddress = base + 0x65AB30; //RpcSetColor
DWORD call_name = base + 0x656F90;//call name
DWORD rpcSetName = base + 0x65ADF0;//set name serveside
DWORD rpcRepairSystem = base + 0x3C4B20;//sabotage stuff

//lobby Player Offsets
std::vector<unsigned int> player2_Offset = { 0x5C, 0x8, 0x8, 0x14 };
std::vector<unsigned int> player3_Offset = { 0x5C, 0x8, 0x8, 0x18 };
std::vector<unsigned int> player4_Offset = { 0x5C, 0x8, 0x8, 0x1C };
std::vector<unsigned int> player5_Offset = { 0x5C, 0x8, 0x8, 0x20 };
std::vector<unsigned int> player6_Offset = { 0x5C, 0x8, 0x8, 0x24 };
std::vector<unsigned int> player7_Offset = { 0x5C, 0x8, 0x8, 0x28 };
std::vector<unsigned int> player8_Offset = { 0x5C, 0x8, 0x8, 0x2C };
std::vector<unsigned int> player9_Offset = { 0x5C, 0x8, 0x8, 0x30 };
std::vector<unsigned int> player10_Offset = { 0x5C, 0x8, 0x8, 0x34 };

//player status
int targetPlayerColorID = 0;
bool player2Online = false;
bool player3Online = false;
bool player4Online = false;
bool player5Online = false;
bool player6Online = false;
bool player7Online = false;
bool player8Online = false;
bool player9Online = false;
bool player10Online = false;

//animiation triggger bools
bool isScanner = false;
bool isTrash = false;
bool isWeapons = false;
bool isRainbowHackColor = false;
bool isEveryoneSameColor = false;
bool isEveryoneSameName = false;
bool isChangeNameTriggered = false;

//sabotage bools
bool isLightSabotaged = false;
bool isCommunicationSabotaged = false;
bool is02Sabotaged = false;
bool isReactorSabotaged = false;

//for player Rainbow Hack
DWORD ColorSelected = 1;

DWORD findAddress(uintptr_t ptr, std::vector<unsigned> offsets) 
{
    uintptr_t addr = ptr;

    for (unsigned int i = 0; i < offsets.size(); ++i) 
    {
        Sleep(10);
        addr = *(uintptr_t*)addr;
        addr += offsets[i];
    }
    return addr;
}

struct system_string
{
    DWORD instance;
    DWORD padding;
    DWORD length;
    wchar_t content[1024];
};

system_string name_container;
system_string name_container2;
system_string name_container3;
system_string name_container4;

DWORD set_name_inner(DWORD instance)
{
    wcscpy_s(name_container.content, L"OOOOOO");
    name_container.instance = instance;
    name_container.length = (DWORD)wcslen(name_container.content);
    return (DWORD)&name_container;

}

DWORD set_name_inner2(DWORD instance)
{

    name_container2.instance = instance;
    name_container2.length = (DWORD)wcslen(name_container2.content);
    return (DWORD)&name_container2;

}

void Change_every_name()
{
    bool players[9] = {player2Online, player3Online, player4Online, player5Online, player6Online, player7Online, player8Online, player9Online, player10Online};
    int player_amount = 0;
    for (int i = 0; i < 10; i++)
    {
        if (players[i]) 
        {
            player_amount += 1;
        }
    }

    DWORD s_instance_ptr = findAddress(player_control_address, { 0x5C, 0x00, 0x34, 0xC, 0x00, 0x00 });
    name_container3.instance = s_instance_ptr;
    name_container3.length = (DWORD)wcslen(name_container3.content);
    DWORD s_instance_ptr2 = findAddress(player_control_address, { 0x5C, 0x00, 0x34, 0xC, 0x00, 0x00 });
    name_container4.instance = s_instance_ptr2;
    name_container4.length = (DWORD)wcslen(name_container4.content);
    for (unsigned int p = 0; p < player_amount; ++p)
    {
        DWORD player_instance = findAddress(player_control_address, { 0x5C, 0x8, 0x8, (0x10 + (p * 4)), 0x0 });
        reinterpret_cast<void(*)(DWORD, DWORD)>(rpcSetName)(player_instance, (DWORD)&name_container3);
        DWORD player_instance2 = findAddress(player_control_address, { 0x5C, 0x8, 0x8, (0x10 + (p * 4)), 0x0 });
        reinterpret_cast<void(*)(DWORD, DWORD)>(rpcSetName)(player_instance2, (DWORD)&name_container4);
    }
}

void refreshPlayerStatus() 
{

    uintptr_t P2ONLINE = findAddress(player_control_address, player2_Offset);
    uintptr_t P3ONLINE = findAddress(player_control_address, player3_Offset);
    uintptr_t P4ONLINE = findAddress(player_control_address, player4_Offset);
    uintptr_t P5ONLINE = findAddress(player_control_address, player5_Offset);
    uintptr_t P6ONLINE = findAddress(player_control_address, player6_Offset);
    uintptr_t P7ONLINE = findAddress(player_control_address, player7_Offset);
    uintptr_t P8ONLINE = findAddress(player_control_address, player8_Offset);
    uintptr_t P9ONLINE = findAddress(player_control_address, player9_Offset);
    uintptr_t P10ONLINE = findAddress(player_control_address, player10_Offset);

    if(*(int*)P2ONLINE != 0) 
    {
        player2Online = true;
    }
    else 
    {
        player2Online = false;
    }
    if (*(int*)P3ONLINE != 0)
    {
        player3Online = true;
    }
    else
    {
        player3Online = false;
    }
    if (*(int*)P4ONLINE != 0)
    {
        player4Online = true;
    }
    else
    {
        player4Online = false;
    }
    if (*(int*)P5ONLINE != 0)
    {
        player5Online = true;
    }
    else
    {
        player5Online = false;
    }
    if (*(int*)P6ONLINE != 0)
    {
        player6Online = true;
    }
    else
    {
        player6Online = false;
    }
    if (*(int*)P7ONLINE != 0)
    {
        player7Online = true;
    }
    else
    {
        player7Online = false;
    }
    if (*(int*)P8ONLINE != 0)
    {
        player8Online = true;
    }
    else
    {
        player8Online = false;
    }
    if (*(int*)P9ONLINE != 0)
    {
        player9Online = true;
    }
    else
    {
        player9Online = false;
    }
    if (*(int*)P10ONLINE != 0)
    {
        player10Online = true;
    }
    else
    {
        player10Online = false;
    }
}

//json stuff
std::wstring XFORCEPATH;
std::wstring AmodusFullPath;

void ParseAddressesToJson()
{
    json jsonx = {
      {"isScanner", (DWORD)&isScanner},
      {"isTrash",(DWORD)&isTrash },
      {"isWeapons", (DWORD)&isWeapons},
      {"isRainbowHackColor", (DWORD)&isRainbowHackColor},
      {"isEveryoneSameColor", (DWORD)&isEveryoneSameColor},
      {"isEveryoneSameName", (DWORD)&isEveryoneSameName},
      {"isChangeNameTriggered", (DWORD)&isChangeNameTriggered},
      {"isLightSabotaged", (DWORD)&isLightSabotaged},
      {"isCommunicationSabotaged", (DWORD)&isCommunicationSabotaged},
      {"is02Sabotaged", (DWORD)&is02Sabotaged},
      {"isReactorSabotaged", (DWORD)&isReactorSabotaged},
      {"targetPlayerColorID", (DWORD)&targetPlayerColorID},
      {"name_container", (DWORD)&name_container},
      {"name_container2", (DWORD)&name_container2},
      {"name_container3", (DWORD)&name_container3},
      {"name_container4", (DWORD)&name_container4}
    };
    std::ofstream outSettings(XFORCEPATH);
    outSettings << std::setw(4) << jsonx << std::endl;
}

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

std::wstring AmodusPath = L"\AMODUS MultiHack.exe";
std::wstring JsonFileName = L"\Addresses.json";

std::wstring getDllPath()
{
    TCHAR DllPath[MAX_PATH] = { 0 };
    GetModuleFileName((HINSTANCE)&__ImageBase, DllPath, _countof(DllPath));
    return DllPath;
}
std::wstring getDllDirectory()
{
    std::wstring DllDir = getDllPath();
    std::filesystem::path NewDir(DllDir);
    return NewDir.parent_path().wstring();
}


//animations
void StartMedBayScan()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, 0x01
        push eax
        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]
        test eax, eax
        push eax
        call medScan
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void StopMedBayScan()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, 0x00
        push eax
        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]
        test eax, eax
        push eax
        call medScan
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void FakeTrash() {

    __asm {

        push ebp
        mov ebp, esp
        //[push = 6=weapons // 9= trash]
        push 9
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        test eax, eax
        je endsc
        mov eax, [eax + 0x00]
        test eax, eax
        je endsc
        push eax
        call rpc_play_animation
        endsc :
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void FakeWeapons() {

    __asm {

        push ebp
        mov ebp, esp
        //[push = 6=weapons // 9= trash]
        push 6
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        test eax, eax
        je endsc
        mov eax, [eax + 0x00]
        test eax, eax
        je endsc
        push eax
        call rpc_play_animation
        endsc :
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}

//sabotages
void sabsabotageLights_2() 
{
    reinterpret_cast<void(*)(DWORD, DWORD, DWORD)>(rpcRepairSystem)(findAddress(ship_control_address, { 0x5C, 0x00, 0x00 }), 0x11, 0x7);
}
void sabotageCommunications_2()
{
    reinterpret_cast<void(*)(DWORD, DWORD, DWORD)>(rpcRepairSystem)(findAddress(ship_control_address, { 0x5C, 0x00, 0x00 }), 0x11, 0xE);
}
void sabotageO2_2()
{
    reinterpret_cast<void(*)(DWORD, DWORD, DWORD)>(rpcRepairSystem)(findAddress(ship_control_address, { 0x5C, 0x00, 0x00 }), 0x11, 0x8);
}
void sabotageReactor_2(byte offset)
{
    reinterpret_cast<void(*)(DWORD, DWORD, DWORD)>(rpcRepairSystem)(findAddress(ship_control_address, { 0x5C, 0x00, 0x00 }), 0x11, offset); //15 for polus, 3 for airship
}

void sabotageLights() 
{
    __asm {

        push ebp
        mov ebp, esp
        push 7
        push 0x11
        mov eax, ship_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call rpcRepairSystem
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void sabotageCommunications()
{
    __asm {

        push ebp
        mov ebp, esp
        push 0xE
        push 0x11
        mov eax, ship_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call rpcRepairSystem
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void sabotageO2()
{
    __asm {

        push ebp
        mov ebp, esp
        push 8
        push 0x11
        mov eax, ship_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call rpcRepairSystem
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void sabotageReactor()
{
    __asm {

        push ebp
        mov ebp, esp
        push 3
        push 0x11
        mov eax, ship_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call rpcRepairSystem
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void sabotageReactor2()
{
    __asm {

        push ebp
        mov ebp, esp
        push 0x15
        push 0x11
        mov eax, ship_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call rpcRepairSystem
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}

//setLocalNames
void Set_Name()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call set_name_inner
        push eax
        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]
        push eax

        call call_name


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name2()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call set_name_inner2
        push eax
        mov eax, base_pointer
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x20]
        push eax

        call call_name


        mov esp, ebp
        pop ebp
        popad

    }

}

//changecolors
void Color_Change()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, ColorSelected
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x00]
        test eax, eax
        push eax
        call CmdSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void changeColorPlayer2()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x14]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void changeColorPlayer3()
{

    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x18]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer4()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x1C]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer5()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x20]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer6()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x24]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer7()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x28]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer8()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x2C]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}
void changeColorPlayer9()
{
    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x30]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
}
void changeColorPlayer10()
{

    __asm {

        push ebp
        mov ebp, esp
        mov eax, targetPlayerColorID
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x34]
        test eax, eax
        push eax
        call RpcSetColorAddress
        mov esp, ebp
        pop ebp
        xor eax, eax
        inc eax


    }
    
    
}

//setNamesServerSided
/*
void Set_Name_Player2()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x14]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x14]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player3()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x18]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x18]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player4()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x1C]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x1C]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player5()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x20]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x20]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player6()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x24]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x24]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player7()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x28]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x28]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player8()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x2C]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x2C]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player9()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x30]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x30]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
void Set_Name_Player10()
{

    __asm
    {

        pushad
        push ebp
        mov ebp, esp

        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x34]

        mov ecx, eax

        mov eax, [eax + 0x34]
        mov eax, [eax + 0x0C]
        mov eax, [eax]
        push eax
        call setAll_names
        push eax
        mov eax, player_control_address
        mov eax, [eax]
        mov eax, [eax + 0x5C]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x8]
        mov eax, [eax + 0x34]
        push eax

        call rpcSetName


        mov esp, ebp
        pop ebp
        popad

    }

}
*/

DWORD WINAPI main(LPVOID lpParam)
{
    XFORCEPATH = getDllDirectory();
    XFORCEPATH = XFORCEPATH + JsonFileName;
    AmodusFullPath = getDllDirectory();
    AmodusFullPath = AmodusFullPath + AmodusPath;
    ParseAddressesToJson();

    while (true)
    {
        if (isScanner)
        {
            StartMedBayScan();
            isScanner = false;
            Sleep(10000);
            StopMedBayScan();
        }

        if (isTrash)
        {
            FakeTrash();
            isTrash = false;
            Sleep(140);
        }

        if (isWeapons)
        {
            FakeWeapons();
            isWeapons = false;
            Sleep(50);
        }

        if (isChangeNameTriggered)
        {
            Set_Name();
            Sleep(100);
            Set_Name2();
            isChangeNameTriggered = false;
            Sleep(50);
        }

        if (isEveryoneSameColor)
        {
            refreshPlayerStatus();
            
            if (player2Online) {
                changeColorPlayer2();
            }

            if (player3Online) {
                changeColorPlayer3();
            }

            if (player4Online) {
                changeColorPlayer4();
            }

            if (player5Online) {
                changeColorPlayer5();
            }

            if (player6Online) {
                changeColorPlayer6();
            }

            if (player7Online) {
                changeColorPlayer7();
            }

            if (player8Online) {
                changeColorPlayer8();
            }

            if (player9Online) {
                changeColorPlayer9();
            }

            if (player10Online) {
                changeColorPlayer10();
            }
            
            isEveryoneSameColor = false;
            Sleep(50);
        }

        if (isEveryoneSameName)
        {
            refreshPlayerStatus();
            Change_every_name();

            isEveryoneSameName = false;
            Sleep(50);
        }

        if (isRainbowHackColor)
        {
            if (ColorSelected < 11)
            {
                Color_Change();
                ColorSelected++;
                Sleep(50);
            }
            else
            {
                Color_Change();
                ColorSelected = 0;
            }
            Sleep(100);
        }

        if (isLightSabotaged) 
        {
            sabsabotageLights_2();
            isLightSabotaged = false;
            Sleep(150);
        }

        if (isCommunicationSabotaged)
        {
            sabotageCommunications_2();
            isCommunicationSabotaged = false;
            Sleep(150);
        }

        if (is02Sabotaged)
        {
            sabotageO2_2();
            is02Sabotaged = false;
            Sleep(150);
        }

        if (isReactorSabotaged)
        {
            DWORD MapID = findAddress(player_control_address, { 0x5C, 0x04, 0x10 });
            if (*(int*)MapID == 2)
            {
                sabotageReactor_2(0x15);
            }
            else if (*(int*)MapID == 0 || *(int*)MapID == 1 || *(int*)MapID == 4)
            {
                sabotageReactor_2(0x3);
            }
            isReactorSabotaged = false;
            Sleep(150);
        }
        Sleep(100);
    }
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID lp) {
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 4096, &main, 0, 0, NULL);
    return TRUE;
}