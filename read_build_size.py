import pandas as pd
import numpy as np

# format is:
# timestamp,bytes,loc
# convert timestamp into date
# convert bytes into MB
# calculate average lines of code per second
# calculate average bytes per second


def main():
    filename = "build-stats.csv"
    df = pd.read_csv(filename)

    for index, row in df.iterrows():
        # df.at[index, "timestamp"] = pd.to_datetime(row["timestamp"], unit="s")
        df.at[index, "binkb"] = row["binbytes"] // 1024
        df.at[index, "libkb"] = row["libbytes"] // 1024
        # if we are at the first index, we cant calculate an LOC/s
        # however, every index past 0, we can subtract the timestamps to get how long between logs
        # then we can divide the loc by the time to get the loc/s
        if index > 0:
            pass
            time = row["timestamp"] - df.at[index - 1, "timestamp"]
            loc = row["loc"] - df.at[index - 1, "loc"]
            # bytes_ = row["binbytes"] - df.at[index - 1, "bytes"]
            df.at[index, "loc/s"] = loc / time
            # df.at[index, "bytes/s"] = bytes_ / time
            # how fast the build time is increasing or decreasing per second
            # buildtime = row["buildtime"] - df.at[index - 1, "buildtime"]
            # df.at[index, "buildtime ms/s"] = buildtime / time
        else:
            df.at[index, "loc/s"] = 0
            # df.at[index, "bytes/s"] = 0
            # df.at[index, "buildtime ms/s"] = 0

    df["timestamp"] = pd.to_datetime(df["timestamp"], unit="s")
    # convert "buildtime" from fractional seconds to milliseconds
    # df["buildtime"] = df["buildtime"] * 1000
    print(df)
    print()

    # buildtimestr = f"{df['buildtime'].mean():.4f}"
    locstr = f"{df['loc'].mean():.1f}"
    kbstr = f"{df['binkb'].mean():.1f}"
    kbstr2 = f"{df['libkb'].mean():.1f}"
    buildtimestr = f"{df['buildtime'].mean():.3f}"

    # bpsstr = f"{df['bytes/s'].mean():.2f}"
    # locsstr = f"{df['loc/s'].mean():.2f}"
    # buildtimestr = f"{df['buildtime ms/s'].mean():.2f}"

    print("Average Stats:")
    print("--------------")
    # print(f"Avg build Time (ms):      {buildtimestr}")
    print(f"Avg line count:   {locstr:>10} loc")
    print(f"Avg binary size:  {kbstr:>10} kb")
    print(f"Avg library size: {kbstr2:>10} kb")
    print(f"Avg build Time:   {buildtimestr:>10} s")
    # print(f"Avg build size (bytes/s): {bpsstr}")
    # print(f"Avg line count (loc/s):   {locsstr}")
    # print(f"Avg build Time (ms/s):    {buildtimestr}")

    # current stats
    print()
    print("Current Stats:")
    print("--------------")
    print(f"Current line count:   {df.at[df.index[-1], 'loc']} loc")
    print(f"Current binary size:  {df.at[df.index[-1], 'binkb']} kb")
    print(f"Current library size: {df.at[df.index[-1], 'libkb']} kb")
    print(f"Current build Time:   {df.at[df.index[-1], 'buildtime']} s")


if __name__ == "__main__":
    main()
