#include <fstream>
#include <sstream>

#include "functions.h"
#include "parcing/py_runner.h"

int main() {
    //запускаем парсер на питоне
    std::string py_script_path = "parcing/parcer.py"; 
    int result = run_python_script(py_script_path);

    ifstream inputFile("parcing/data.txt");//Открываем файл
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open data.txt" << endl;
        return 1;
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

    int days = 180; //approximately half a year
    int simulations = 100000;
    map<string, double> drifts;
    map<string, double> volatilities;

    for (const auto& [currency, rates] : historical_rates) {
        const auto [drift, volatility] = calculate_drift_volatility(rates, days);
        drifts[currency] = drift;
        volatilities[currency] = volatility;
    }

    map<string, vector<vector<double> > > simulated_rates = simulate_exchange_rates(historical_rates, days, simulations, drifts, volatilities);
    /*  //To see the simulated rates
	for (const auto& pair : simulated_rates)
    {
      cout << "Currency: " << pair.first << endl;
      for(int i = 0; i < days; i++)
      {
          cout << "Day " << i + 1 << ": ";
           for(int j = 0; j < simulations; j++)
           {
              cout << fixed << setprecision(3) << pair.second[i][j] << " ";
           }
        cout << endl;
      }
     cout << endl;
    }
	*/
    return 0;
}
