map<string, vector<double> > user_request(){
    ifstream portFile("./parsing/portfolio.txt");//Открываем файл
    if (!portFile.is_open()) {
        cerr << "Error: Unable to open user_input.txt" << endl;
    }

    map<string, vector<double> > portfolio;
    portfolio["RUB"] = vector <double>();
    portfolio["USD"] = vector <double>();
    portfolio["EUR"] = vector <double>();
    
    string l;//Читаем данные из файла
    while (getline(portFile, l)) {
        stringstream ss(l);
        float rub_amount, usd_amount, eur_amount;
        if (ss >> rub_amount >> usd_amount >> eur_amount) {
            portfolio["RUB"].push_back(rub_amount);
            portfolio["USD"].push_back(usd_amount);
            portfolio["EUR"].push_back(eur_amount);
        } else {
            cerr << "Warning: Invalid data in line: " << l << ". Skipping." << endl;//Проверка корректного чтения
        }
    }
    portFile.close();
    return portfolio;
}