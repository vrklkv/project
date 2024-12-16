#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "functions.h"

int main() {
    map<string, vector<double>> historical_rates = {
        {"USD", {70.0, 70.1, 70.2, 70.3, 70.4, 70.5, 70.6, 70.7, 70.8, 70.9, 71.0, 71.1, 71.2, 71.3, 71.4, 71.5, 71.6, 71.7, 71.8, 71.9, 72.0, 72.1, 72.2, 72.3, 72.4, 72.5, 72.6, 72.7, 72.8, 72.9, 73.0, 73.1, 73.2, 73.3, 73.4, 73.5, 73.6, 73.7, 73.8, 73.9, 74.0, 74.1, 74.2, 74.3, 74.4, 74.5, 74.6, 74.7, 74.8, 74.9, 75.0}},
        {"EUR", {80.0, 80.1, 80.2, 80.3, 80.4, 80.5, 80.6, 80.7, 80.8, 80.9, 81.0, 81.1, 81.2, 81.3, 81.4, 81.5, 81.6, 81.7, 81.8, 81.9, 82.0, 82.1, 82.2, 82.3, 82.4, 82.5, 82.6, 82.7, 82.8, 82.9, 83.0, 83.1, 83.2, 83.3, 83.4, 83.5, 83.6, 83.7, 83.8, 83.9, 84.0, 84.1, 84.2, 84.3, 84.4, 84.5, 84.6, 84.7, 84.8, 84.9, 85.0}}
    };
    int days = 180; //примерно 6 месяцев
    int simulations = 100000;
    map<string, double> drifts;
    map<string, double> volatilities;

    for (const auto& [currency, rates] : historical_rates) {
        auto [drift, volatility] = calculate_drift_volatility(rates, days);
        drifts[currency] = drift;
        volatilities[currency] = volatility;
    }

    map<string, vector<vector<double>>> simulated_rates = simulate_exchange_rates(historical_rates, days, simulations, drifts, volatilities);
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
