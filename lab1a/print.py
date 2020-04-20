import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


plt.plot([1, 2,4,8,16, 32], [3.330033, 4.024569,3.482626, 3.254335, 3.582518, 4.653898])

plt.ylabel("Seconds")
plt.xlabel("Number of cores")

plt.title("Threshfilter's execution time on image4 in seconds")

patch = mpatches.Patch(color='blue', label="Execution time in seconds")
plt.legend(handles=[patch])

plt.savefig("thresh_filter")

#plt.show()