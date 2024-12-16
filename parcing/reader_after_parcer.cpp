#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

int main() {
    ifstream inputFile("data.txt");//Открываем файл
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open data.txt" << endl;
        return 1;
    }
    map<string, vector<double>> historical_rates;//Создаем словарь
    historical_rates["USD"] = vector<double>();
    historical_rates["EUR"] = vector<double>();

    string line;//Читаем данные из файла
    while (getline(inputFile, line)) {
        stringstream ss(line);
        float usd_rate, eur_rate;
        if (ss >> usd_rate >> eur_rate) {
            historical_rates["USD"].push_back(usd_rate);
            historical_rates["EUR"].push_back(eur_rate);
        } else {
            cerr << "Warning: Invalid data in line: " << line << ". Skipping." << endl;//Проверка корректного чтения
        }
    }
    inputFile.close();
    /* //Вывод для проверки
    cout << "map<string, vector<double>> historical_rates = {" << endl;
     for (const auto& pair : historical_rates)
     {
         cout << "    {\"" << pair.first << "\", {";
         for(size_t i = 0; i < pair.second.size(); ++i)
         {
           cout << fixed << setprecision(3) << pair.second[i];
           if (i < pair.second.size() - 1)
           {
             cout << ", ";
           }
         }
         cout << "}}," << endl;
     }
    cout << "};" << endl;
   */
    return 0;
}