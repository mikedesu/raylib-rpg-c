import pandas
import sys
import matplotlib
import matplotlib.pyplot as plt
from datetime import datetime

if len(sys.argv) < 3:
    print("Usage: python graph_loc.py <filename> <column_names> ")
    sys.exit(1)

# plt.xkcd()

filename = sys.argv[1]
# column names
column_names = []
for i in range(2, len(sys.argv)):
    column_names.append(sys.argv[i])

data = pandas.read_csv(filename)

data = data[column_names]

data = data.dropna()


data.plot()

# save the plot to file
# get a unix timestamp
timestamp = datetime.now().strftime("%s")

base_filename = "graphs/"
main_filename = ""
for column_name in column_names:
    main_filename += column_name + "-"
main_filename += timestamp + ".png"

out_filename = "graphs/" + main_filename
plt.savefig(out_filename)

# matplotlib.pyplot.show()
