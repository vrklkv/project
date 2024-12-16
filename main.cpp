#include <fstream>
#include <sstream>

#include "headers/functions.h"
#include "parsing/py_runner.h"
#include "headers/user_request.h"
#include "headers/risk_calculation.h"

int main() {
    //Запускаем парсер на питоне
    //std::string py_script_path = "parcing/parcer.py"; //Работает на MacOS
    std::string py_script_path = "../parcing/parcer.py"; //Работает на Windows
    int result = run_python_script(py_script_path);

    //ifstream inputFile("parcing/data.txt"); //Работает на MacOS
    ifstream inputFile("../parcing/data.txt");//Работает на Windows
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open data.txt" << endl;
    }
    map <string, vector <double> > historical_rates;//Создаем словарь
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

    int days = 180; //half a year
    int simulations = 100000;
    map<string, double> drifts;
    map<string, double> volatilities;

    for (const auto& [currency, rates] : historical_rates) {
        const auto [drift, volatility] = calculate_drift_volatility(rates, days);
        drifts[currency] = drift;
        volatilities[currency] = volatility;
    }

    map<string, vector<vector<double> > > simulated_rates = simulate_exchange_rates(historical_rates, days, simulations, drifts, volatilities);
    
    map<string, vector<double> > portfolio = user_request();

    risk_calculation(simulated_rates, portfolio);
    return 0;
}