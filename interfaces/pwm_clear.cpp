#include <iostream>
#include <fstream>
#include <filesystem>

void pwm_write(const std::string& path, int value)
{
    std::ofstream f(path);
    f << value;
}

int main()
{
    // disable the two hardware pwm channels (GPIO 18, 19) if they are initialized
    // (written for Raspberry Pi 5, may differ for other devices)
    if (std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm2"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 0);
    }
    if (std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm3"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 0);
    }

    // terminate the program
    return 0;
}