import pandas
import sys
import matplotlib
import matplotlib.pyplot as plt
from datetime import datetime
import os

if len(sys.argv) < 3:
    print("Usage: python graph_loc.py <filename> <column_names> ")
    sys.exit(1)

plt.xkcd()

filename = sys.argv[1]

y_axis_label = sys.argv[2]

# column names
column_names = []
for i in range(3, len(sys.argv)):
    column_names.append(sys.argv[i])

data = pandas.read_csv(filename)

data = data[column_names]

data = data.dropna()

# set the font size of the labels on the x-axis and y-axis
matplotlib.rcParams.update({"font.size": 10})
# set the size of the plot
# the first value is the width and the second value is the height in inches
# plt.figure(figsize=(19.2, 10.8))

figure = plt.figure(figsize=(19.2, 10.8))
# add a title
plt.title("Graph of " + filename)


# set the font size of the legend
plt.legend(fontsize=10)

# add a label to the y-axis
plt.ylabel(y_axis_label)

# add a label to the x-axis
plt.xlabel("Code Over Time")


# data.plot()

# plot the data
data.plot(ax=figure.gca())


# save the plot to file
# get a unix timestamp
timestamp = datetime.now().strftime("%s")

# check if the directory exists
if not os.path.exists("graphs"):
    os.makedirs("graphs")


base_filename = "graphs/"
main_filename = ""
for column_name in column_names:
    main_filename += column_name + "-"
main_filename += timestamp + ".png"

out_filename = "graphs/" + main_filename
plt.savefig(out_filename, dpi=100)

# matplotlib.pyplot.show()
