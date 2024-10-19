import pandas
import sys
import matplotlib
import matplotlib.pyplot as plt

plt.xkcd()

filename = sys.argv[1]

data = pandas.read_csv(filename)


data = data[["loc"]]
# loc is the y-axis
# data = data.rename(columns={"loc": "y"})

data = data.dropna()


data.plot()
matplotlib.pyplot.show()
