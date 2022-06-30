import numpy as np
import matplotlib.pyplot as plt

data_m = np.loadtxt("multi_task_debug.dat")

fig, ax = plt.subplots()
time = np.arange(0,data_s.size)

ax.plot(time, data_m)

plt.show()

