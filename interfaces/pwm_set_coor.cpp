#include <iostream>
#include <fstream>
#include <filesystem>

// define coordinate data structure
#pragma pack(push, 1)
struct Coordinates {
    float x;
    float y;
};
#pragma pack(pop)

// fuction to write to hardware pwm
void pwm_write(const std::string& path, int value)
{
    std::ofstream f(path);
    f << value;
}

int main()
{
    // variables for coordinates
    Coordinates coor = {};
    int frame_width = 1455;  // 1456 minus 1, as index starts with 0
    int frame_height = 1087;  // 1088 minus 1, as index starts with 0

    // pwm period
    int period = 100000;

    // initialize the two hardware pwm channels (GPIO 18, 19) if they aren't initializes yet
    // (written for Raspberry Pi 5, may differ for other devices)
    if (!std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm2"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/export", 2);
    }
    if (!std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm3"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/export", 3);
    }

    // request input
    int input = 0;
    std::cout << "Enter coordinates to send." << std::endl;
    std::cout << "X (0-1455): ";
    std::cin >> input;

    if(input < 0 || input > 1455) {
        std::cerr << "Invalid input. X set to 0." << std::endl;
        coor.x = 0;
    }
    else {
        coor.x = input;
    }
    
    input = 0;
    std::cout << "Y (0-1087): ";
    std::cin >> input;

    if(input < 0 || input > 1087) {
        std::cerr << "Invalid input. Y set to 0." << std::endl;
        coor.y = 0;
    }
    else {
        coor.y = input;
    }

    // set hardware pwm channels with duty_cycle/period == coor/max_coor
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 0);
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/period", period);
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/duty_cycle", (int)(period * coor.x / frame_width));
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 1);

    pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 0);
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/period", period);
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", (int)(period * coor.y / frame_height));
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 1);

    // terminate the program
    return 0;
}