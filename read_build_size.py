import pandas as pd
import numpy as np

# format is:
# timestamp,bytes,loc
# convert timestamp into date
# convert bytes into MB
# calculate average lines of code per second
# calculate average bytes per second


def main():
    filename = "build_size.csv"
    df = pd.read_csv(filename)

    for index, row in df.iterrows():
        # df.at[index, "timestamp"] = pd.to_datetime(row["timestamp"], unit="s")
        df.at[index, "kb"] = row["bytes"] // 1024
        # if we are at the first index, we cant calculate an LOC/s
        # however, every index past 0, we can subtract the timestamps to get how long between logs
        # then we can divide the loc by the time to get the loc/s
        if index > 0:
            time = row["timestamp"] - df.at[index - 1, "timestamp"]
            loc = row["loc"] - df.at[index - 1, "loc"]
            bytes_ = row["bytes"] - df.at[index - 1, "bytes"]
            df.at[index, "loc/s"] = loc / time
            df.at[index, "bytes/s"] = bytes_ / time
        else:
            df.at[index, "loc/s"] = 0
            df.at[index, "bytes/s"] = 0

    df["timestamp"] = pd.to_datetime(df["timestamp"], unit="s")
    print(df)


if __name__ == "__main__":
    main()
