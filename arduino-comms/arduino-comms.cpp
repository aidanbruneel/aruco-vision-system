#include "pch.h"
#include <iostream>


using namespace System;
using namespace System::IO::Ports;

int main(array<System::String ^> ^args)
{
    SerialPort port("COM3", 9600);
    port.Open();
    while (1)
    {
        std::cout << "Enter String: " << std::endl;
        String^ input = Convert::ToString(Console::ReadLine());
        std::cout << std::endl;
        port.Write(input);

    }
    port.Close();
    return 0;
}
