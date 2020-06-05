#include <iostream>
#include <vector>
#include "histogram.h"
#include "svg_histogram.h"
#include <curl/curl.h>
#include <sstream>
#include <string>

using namespace std;

vector<double> input_numbers(istream& in, size_t count)
{
    vector<double> result(count);
    for (size_t i = 0; i < count; i++)
    {
        cin >> result[i];
    }
    return result;
}

Input read_input(istream& in, bool prompt)
{
    Input data;
    if (prompt)
    {
        cerr << "Enter number count: ";
    }
    size_t number_count;
    cin >> number_count;
    if (prompt)
    {
        cerr << "Enter numbers: ";
    }
    data.numbers = input_numbers(in, number_count);
    if (prompt)
    {
        cerr << "Enter column count: ";
    }
    size_t bin_count;
    cin >> bin_count;
    data.bin_count = bin_count;
    return data;
}

size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx)
{
    const size_t data_size = item_size * item_count;
    const char* new_items = reinterpret_cast<const char*>(items);
    stringstream* buffer = reinterpret_cast<stringstream*>(ctx);
    buffer->write(new_items, data_size);
    return 0;
}

Input download(const string& address)
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
            res = CURLE_FAILED_INIT;
            if (res != CURLE_OK)
            {
                cout << curl_easy_strerror(res) << endl;
                exit(1);
            }
            curl_easy_cleanup(curl);
        }

    return read_input(buffer, false);
}

vector<size_t> make_histogram(Input data)
{
    double min;
    double max;
    find_minmax(data.numbers, min, max);
    vector<size_t> bins(data.bin_count);
    for (double number : data.numbers)
    {
        size_t bin = (size_t)((number - min) / (max - min) * data.bin_count);
        if (bin == data.bin_count)
        {
            bin--;
        }
        bins[bin]++;
    }
    return(bins);
}

void show_histogram_text(vector<size_t> bins, size_t number_count)
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
}

int main(int argc, char* argv[])
{
    Input input;
    if (argc>1)
    {
        input = download(argv[1]);
    }
    else
    {
        input = read_input(cin, true);
    }
        // Обработка данных
        const auto bins = make_histogram(input);
        // Вывод данных
        show_histogram_svg(bins);
        return 0;
}
