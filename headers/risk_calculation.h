void risk_calculation(map<string, vector<vector<double> > >& simulated_rates, map<string, vector<double> >& portfolio){  
    // Расчет стоимости портфеля
    ofstream outputFile("./result.txt");//Открываем файл
    for (int i; i < portfolio["RUB"].size(); i++){
        map<string, double> p;
        p["RUB"] = portfolio["RUB"][i];
        p["USD"] = portfolio["USD"][i];
        p["EUR"] = portfolio["EUR"][i];
        vector<double> portfolio_values = portfolio_value(simulated_rates, p);
        double confidence_lvl = 0.95; 
        double cvar = calculate_cvar(portfolio_values, confidence_lvl);
        outputFile << "Максимальный риск портфеля (95% довер): " << cvar<<endl;
    }
}