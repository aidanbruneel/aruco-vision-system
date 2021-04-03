#include "pch.h"
#include <iostream>


using namespace System;
using namespace System::IO::Ports;

int main(array<System::String ^> ^args)
{
    SerialPort port("COM3", 250000);
    port.Open();
    while (1)
    {
        std::cout << "Enter Int: " << std::endl;
        int input = Convert::ToInt32(Console::ReadLine());
        std::cout << std::endl;

        char* str;
        String^ cliStr;
        if (input == 1)
        {
            str = "AAAAAAAA";
            cliStr = gcnew String(str);
        }
        else
        {
            str = "BB";
            cliStr = gcnew String(str);
        }
        port.Write(cliStr);

    }
    port.Close();
    return 0;
}

// String guidance = "dxxxxxayyf";