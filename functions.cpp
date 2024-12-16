#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>

using namespace std;

// Функция для генерации нормального случайного числа
double generate_normal_random(double mean, double stddev, gsl_rng* rng) {
    return gsl_ran_gaussian(rng, stddev) + mean;
}

// Функция для моделирования обменных курсов
map<string, vector<vector<double>>> simulate_exchange_rates(const map<string, vector<double>>& historical_rates,
                                              int days, int simulations,
                                              const map<string, double>& drifts,
                                              const map<string, double>& volatilities,
                                              gsl_rng* rng)
{
    map<string, vector<vector<double>>> simulated_rates;
    double dt = 1.0 / days;

    for (const auto& pair : historical_rates) {
        const string& currency = pair.first;
        const vector<double>& rates = pair.second;
        double drift = drifts.at(currency);
        double volatility = volatilities.at(currency);

        vector<vector<double>> daily_returns(days, vector<double>(simulations));
        for (int i = 0; i < days; i++)
        {
          for (int j = 0; j < simulations; j++) {
            daily_returns[i][j] = exp((drift - 0.5 * pow(volatility, 2)) * dt + volatility * generate_normal_random(0, sqrt(dt), rng));
          }
        }

        vector<vector<double>> simulation(days, vector<double>(simulations));
        for (int j = 0; j < simulations; ++j)
           simulation[0][j] = rates[rates.size() - 1] * daily_returns[0][j];


        for (int i = 1; i < days; i++) {
            for (int j = 0; j < simulations; ++j)
              simulation[i][j] = simulation[i-1][j] * daily_returns[i][j];
        }

        simulated_rates[currency] = simulation;
    }
    return simulated_rates;
}


// Функция для расчета стоимости портфеля
vector<double> portfolio_value(const map<string, vector<vector<double>>>& simulated_rates, const map<string, double>& portfolio) {
    int simulations = simulated_rates.begin()->second[0].size();
    vector<double> values(simulations, portfolio.count("RUB") ? portfolio.at("RUB") : 0.0);

    for (const auto& pair : portfolio) {
        const string& currency = pair.first;
        double amount = pair.second;
         if (currency != "RUB") {
            for(int j = 0; j < simulations; ++j)
              values[j] += amount * simulated_rates.at(currency).back()[j];
          }
    }
    return values;
}


// Функция для расчета VaR
double calculate_var(vector<double> portfolio_values, double confidence_level) {
    if (portfolio_values.size() == 0) return 0.0;
    int index = static_cast<int>((1 - confidence_level) * portfolio_values.size());
    nth_element(portfolio_values.begin(), portfolio_values.begin() + index, portfolio_values.end());
    return portfolio_values[index];
}

// Функция для расчета CVaR
double calculate_cvar(vector<double> portfolio_values, double confidence_level) {
    double var = calculate_var(portfolio_values, confidence_level);
    double sum = 0.0;
    int count = 0;
    for(double value : portfolio_values)
    {
      if (value <= var)
      {
         sum += value;
         count++;
      }
    }
    return count == 0 ? 0.0 : sum / count;
}

// Функция для расчета доверительного интервала
pair<double, double> calculate_confidence_interval(vector<double> simulated_data, double confidence_level) {
    double lower_percentile = (1 - confidence_level) / 2 * 100;
    double upper_percentile = (1 - (1 - confidence_level) / 2) * 100;


    double lower_bound = gsl_stats_quantile_from_sorted_data(simulated_data.data(), 1, simulated_data.size(), lower_percentile / 100.0);
    double upper_bound = gsl_stats_quantile_from_sorted_data(simulated_data.data(), 1, simulated_data.size(), upper_percentile / 100.0);
    return make_pair(lower_bound, upper_bound);
}

// Функция для расчета дрейфа и волатильности
pair<double, double> calculate_drift_volatility(const vector<double>& rates, int lookback_period) {
  if (rates.size() <= lookback_period) return make_pair(0.0, 0.0);
      
    vector<double> log_returns(rates.size() - 1);
    for (size_t i = 1; i < rates.size(); ++i) {
      log_returns[i - 1] = log(rates[i] / rates[i - 1]);
    }


    int n = log_returns.size();
    vector<double> rolling_mean(n - lookback_period + 1);
    vector<double> rolling_std(n - lookback_period + 1);
    for (int i = 0; i < n - lookback_period + 1; i++) {
        double sum = 0.0;
         for(int j = 0; j < lookback_period; j++){
            sum += log_returns[i+j];
         }
        rolling_mean[i] = sum / lookback_period;

         double sum_sq_diff = 0.0;
         for(int j = 0; j < lookback_period; j++){
           sum_sq_diff += pow(log_returns[i+j] - rolling_mean[i], 2);
         }
        rolling_std[i] = sqrt(sum_sq_diff / lookback_period);
    }


    double drift = rolling_mean[rolling_mean.size()-1];
    double volatility = rolling_std[rolling_std.size()-1];

    return make_pair(drift, volatility);
}

int main() {

  gsl_rng * rng;
  gsl_rng_env_setup();
  const gsl_rng_type * T = gsl_rng_default;
  rng = gsl_rng_alloc(T);

  // Пример использования (замените на реальные данные)
    map<string, vector<double>> historical_rates = {
        {"USD", {70.0, 70.1, 70.2, 70.3, 70.4, 70.5, 70.6, 70.7, 70.8, 70.9, 71.0, 71.1, 71.2, 71.3, 71.4, 71.5, 71.6, 71.7, 71.8, 71.9, 72.0, 72.1, 72.2, 72.3, 72.4, 72.5, 72.6, 72.7, 72.8, 72.9, 73.0, 73.1, 73.2, 73.3, 73.4, 73.5, 73.6, 73.7, 73.8, 73.9, 74.0, 74.1, 74.2, 74.3, 74.4, 74.5, 74.6, 74.7, 74.8, 74.9, 75.0}},
        {"EUR", {80.0, 80.1, 80.2, 80.3, 80.4, 80.5, 80.6, 80.7, 80.8, 80.9, 81.0, 81.1, 81.2, 81.3, 81.4, 81.5, 81.6, 81.7, 81.8, 81.9, 82.0, 82.1, 82.2, 82.3, 82.4, 82.5, 82.6, 82.7, 82.8, 82.9, 83.0, 83.1, 83.2, 83.3, 83.4, 83.5, 83.6, 83.7, 83.8, 83.9, 84.0, 84.1, 84.2, 84.3, 84.4, 84.5, 84.6, 84.7, 84.8, 84.9, 85.0}}
    };
    map<string, double> drifts = {
        {"USD", 0.0001},
        {"EUR", 0.0002}
    };
     map<string, double> volatilities = {
        {"USD", 0.01},
        {"EUR", 0.012}
    };
    map<string, double> portfolio = {
         {"USD", 1000},
         {"EUR", 500},
         {"RUB", 10000}
     };

    int days = 250;
    int simulations = 10000;

    map<string, vector<vector<double>>> simulated_rates = simulate_exchange_rates(historical_rates, days, simulations, drifts, volatilities, rng);
    vector<double> portfolio_values = portfolio_value(simulated_rates, portfolio);


    double var = calculate_var(portfolio_values, 0.95);
    double cvar = calculate_cvar(portfolio_values, 0.95);
    pair<double, double> confidence_interval = calculate_confidence_interval(portfolio_values, 0.95);

    cout << fixed << setprecision(3);
    cout << "VaR: " << var << endl;
    cout << "CVaR: " << cvar << endl;
    cout << "Confidence Interval: [" << confidence_interval.first << ", " << confidence_interval.second << "]" << endl;
    
    gsl_rng_free(rng);
    return 0;
}