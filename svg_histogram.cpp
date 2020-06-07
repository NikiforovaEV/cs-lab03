#include <iostream>
#include <vector>
#include "svg_histogram.h"
#include <windows.h>
#include <sstream>
#include <string>

using namespace std;

void svg_begin(double width, double height)
{
    cout << "<?xml version='1.0' encoding='UTF-8'?>\n";
    cout << "<svg ";
    cout << "width='" << width << "' ";
    cout << "height='" << height << "' ";
    cout << "viewBox='0 0 " << width << " " << height << "' ";
    cout << "xmlns='http://www.w3.org/2000/svg'>\n";
}

void svg_end()
{
    cout << "</svg>\n";
}

void svg_rect(double x, double y, double width, double height, string stroke, string fill)
{
    cout << "<rect x='" << x << "' y='" << y << "' width='" << width << "' height='" << height << "' stroke='" << stroke << "' fill='" << fill << "' />";
}

void svg_text(double left, double baseline, string text)
{
    cout << "<text x='" << left << "' y='" << baseline << "'>" << text << "</text>";
}

void poisk_max_count(const vector<size_t>& bins, double& max_count)
{
    if (bins.size() == 0)
    {
        return;
    }
    else
    {
        max_count = bins[0];
        for (size_t count : bins)
        {
            if (count > max_count)
            {
                max_count = count;
            }
        }
    }
}

void poisk_max_count(const vector<size_t>& bins, double& max_count)
{
    if (bins.size() == 0)
    {
        return;
    }
    else
    {
        max_count = bins[0];
        for (size_t count : bins)
        {
            if (count > max_count)
            {
                max_count = count;
            }
        }
    }
}

string make_info_text()
{
    stringstream buffer;

    DWORD mask = 0x0000ffff;
    DWORD mask_major = 0b00000000'00000000'00000000'11111111;
    DWORD info = GetVersion();
    DWORD build;
    DWORD platform = info >> 16;
    DWORD version = info & mask;
    DWORD version_major = version & mask_major;
    DWORD version_minor = version >> 8;
    if ((info & 0x80000000) == 0)
    {
        build = platform;
        buffer << "Windows v" << version_major << "." << version_minor << "(build " << build << ")\n";
    }
    char system_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(system_name);
    GetComputerNameA(system_name, &size);
    //printf("Computer name: %s\n", system_name);
    buffer << "Computer name: " << system_name;

    return buffer.str();
}

void show_histogram_svg(const vector<size_t>& bins, string stroke)
{
    const auto IMAGE_WIDTH = 600;
    const auto IMAGE_HEIGHT = 300;
    const auto TEXT_HEIGHT = 40;
    const auto BLOCK_WIDTH = 30;
    const auto TEXT_WIDTH = BLOCK_WIDTH / 2;
    svg_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
    double top = 0;
    string fill = "green";
    double max_count;
    poisk_max_count(bins, max_count);
    double max_height = 0;
    double bottom = 0;
    max_height = IMAGE_HEIGHT - 2 * TEXT_HEIGHT;
    const double scaling_factor = (double)(IMAGE_HEIGHT - 2 * TEXT_HEIGHT) / max_count;
    for (size_t bin : bins)
    {
        const double bin_height = bin * scaling_factor;
        top = max_height - bin_height;
        svg_rect(bottom, top, BLOCK_WIDTH, bin_height, stroke, "green");
        svg_text(bottom + TEXT_WIDTH, max_height + TEXT_HEIGHT, to_string(bin));
        bottom += BLOCK_WIDTH;
    }
    bottom = 0;
    string info = make_info_text();
    svg_text(bottom + TEXT_WIDTH, max_height + 2 * TEXT_HEIGHT, info);
    svg_end();
}
