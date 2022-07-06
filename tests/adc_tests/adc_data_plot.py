import numpy as np
import matplotlib.pyplot as plt

data_adc = np.loadtxt("bin/data/adc_test.dat")
#data_dac = np.loadtxt("../../bin/data/dac_data.dat")

fig, ax = plt.subplots()
time = np.arange(0,data_adc.size)

#ax.plot(time, data_adc, time, data_dac)
ax.plot(time, data_adc);

plt.show()
