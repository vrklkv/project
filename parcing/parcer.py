from IPython.display import clear_output
clear_output()
import pandas as pd
import numpy as np
import yfinance as yf 
import os
from datetime import datetime
from dateutil.relativedelta import relativedelta

s = datetime.now() - relativedelta(years=10)
e = datetime.now()
data = yf.download("USDRUB=X EURRUB=X", start=s, end=e)
usd_rates = data['Close']['USDRUB=X'].dropna()
eur_rates = data['Close']['EURRUB=X'].dropna()
frames = [usd_rates, eur_rates]
result = pd.concat(frames, axis=1)
#result.to_csv('result1.csv', index=True) 
current_file = os.path.realpath(__file__)
current_directory = os.path.dirname(current_file)
#excel_writer=str(current_directory)+"\data.xlsx"
where=str(current_directory)+"\data.txt"
#result.to_excel(excel_writer, sheet_name='Sheet1', index=True)
np.savetxt(where, result.values, fmt='%f')