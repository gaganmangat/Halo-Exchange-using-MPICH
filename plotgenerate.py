import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import seaborn as sb
import numpy as np

sb.set(rc={'figure.figsize':(11.7,8.27)})
plots = [16, 36, 49, 64]
plt.ioff()
for i in plots:
    datafile = 'data' + str(i) + '.csv'
    df = pd.read_csv(datafile)
    df.columns = ['N', 'Direct', 'Packed', 'Derived']
    df['Direct'] = np.log(df['Direct'])
    df['Packed'] = np.log(df['Packed'])
    df['Derived'] = np.log(df['Derived'])
    dd = pd.melt(df, id_vars=['N'], value_vars=['Direct','Packed','Derived'], var_name='Method')
    
    plt.figure()
    ax1 = sb.pointplot(x='N', y='Direct', data=df.groupby('N', as_index=False).median(), color='Blue')
    ax2 = sb.pointplot(x='N', y='Packed', data=df.groupby('N', as_index=False).median(), ax=ax1, color='Red')
    ax3 = sb.pointplot(x='N', y='Derived', data=df.groupby('N', as_index=False).median(), ax=ax2, color='Green')
    fig = sb.boxplot(x='N', y="value", data=dd, hue='Method')
    fig.set(xlabel='Value of N for (N^2) data points', ylabel='log(Time(s))')
    figure = fig.get_figure()
    figure.savefig('plot' + str(i) + '.jpg')