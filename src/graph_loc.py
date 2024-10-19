import pandas
import sys
import matplotlib
import matplotlib.pyplot as plt

plt.xkcd()

filename = sys.argv[1]
column_name = sys.argv[2]

data = pandas.read_csv(filename)

data = data[[column_name]]
# loc is the y-axis
# data = data.rename(columns={"loc": "y"})

data = data.dropna()


data.plot()
matplotlib.pyplot.show()
