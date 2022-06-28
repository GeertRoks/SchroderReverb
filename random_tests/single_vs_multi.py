import numpy as np
import matplotlib.pyplot as plt

data_s = np.loadtxt("rev_single_thread_test.dat")
data_m = np.loadtxt("rev_multi_thread_test.dat")

fig, ax = plt.subplots()
time = np.arange(0,data_s.size)

ax.plot(time, data_s, time, data_m)

plt.show()
