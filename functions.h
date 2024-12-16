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