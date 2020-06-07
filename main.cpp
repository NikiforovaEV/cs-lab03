#include <iostream>
#include <vector>
#include "histogram.h"
#include "svg_histogram.h"
#include <curl/curl.h>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

struct Options
{
    string stroke;
    bool stroke_veracity;
    bool violation;
    char* url;
};

Options parse_args(int argc, char** argv)
{
    Options opt;
    opt.url=0;
    opt.stroke_veracity = false;
    opt.violation = false;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if(string(argv[i]) == "-stroke")
            {
                if(i + 1 < argc)
                {
                    opt.stroke = string(argv[i+1]);
                    if (opt.stroke.size())
                    {
                        opt.stroke_veracity = true;
                        i++;
                    }
                    else
                    {
                        opt.violation = true;
                    }
                }
                else
                {
                    opt.violation = true;
                }
            }
        }
        else
        {
            opt.url = argv[i];
        }
    }
    return opt;
}

vector<double> input_numbers(istream& in, const size_t count)
{
    vector<double> result(count);
    for (size_t i = 0; i < count; i++)
    {
        in >> result[i];
    }
    return result;
}

Input read_input(istream& in, bool prompt)
{
    Input data;
    size_t number_count;
    if (prompt)
    {
        cerr << "Enter number count: ";
        in >> number_count;

        cerr << "Enter numbers: ";
        data.numbers = input_numbers(in, number_count);

        cerr << "Enter column count: ";
        in >> data.bin_count;
    }
    else
    {
        in >> number_count;
        data.numbers = input_numbers(in, number_count);
        in >> data.bin_count;
    }
    return data;
}

size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx)
{
    const size_t data_size = item_size * item_count;
    const char* new_items = reinterpret_cast<const char*>(items);
    stringstream* buffer = reinterpret_cast<stringstream*>(ctx);
    buffer->write(new_items, data_size);
    return data_size;
}

Input download(const string& address, const Options &opt)
{
    stringstream buffer;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    if(curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            cout << curl_easy_strerror(res) << endl;
            exit(1);
        }
        curl_easy_cleanup(curl);
    }
   return read_input(buffer, false);
}

/*void show_histogram_text(vector<size_t> bins, size_t number_count)
{
    const size_t SCREEN_WIDTH = 80;
    const size_t MAX_ASTERISK = SCREEN_WIDTH - 4 - 1;

    size_t max_count = 0;
    for (size_t count : bins)
    {
        if (count > max_count)
        {
            max_count = count;
        }
    }
    const bool scaling_needed = max_count > MAX_ASTERISK;

    for (size_t bin : bins)
    {
        if (bin < 100)
        {
            cout << ' ';
        }
        if (bin < 10)
        {
            cout << ' ';
        }
        cout << bin << "|";

        size_t height = bin;
        if (scaling_needed)
        {
            const double scaling_factor = (double)MAX_ASTERISK / max_count;
            height = (size_t)(bin * scaling_factor);
        }

        for (size_t i = 0; i < height; i++)
        {
            cout << '*';
        }
        cout << '\n';
    }
}*/

int main(int argc, char* argv[])
{
   Input input;
   Options opt = parse_args(argc,argv);
    if (opt.violation)
    {
        cerr<<"Error";
        return 1;
    }
    if (opt.url)
    {
        input = download(opt.url,opt);
    }
    else
    {
        input = read_input(cin, true);
    }
    const auto bins = make_histogram(input);
    show_histogram_svg(bins,opt.stroke);
    return 0;
}
