import numpy as np
import matplotlib.pyplot as plt

data_s = np.loadtxt("single_task_debug.dat")
data_m = np.loadtxt("multi_task_debug.dat")

fig, ax = plt.subplots()
time = np.arange(0,data_m.size)

#ax.plot(time, data_m)
ax.plot(time, data_s, time, data_m)

plt.show()
