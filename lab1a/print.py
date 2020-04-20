import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


plt.plot([1, 2,4,8,16, 32], [3.330033, 4.024569,3.482626, 3.254335, \
    3.582518, 4.653898], color="blue")

plt.ylabel("Seconds")
plt.xlabel("Number of cores")

plt.title("Filter execution time on 3000x3000 image with radius 10")

patch_thresh = mpatches.Patch(color='blue', label="Threshold")
first_legend = plt.legend(handles=[patch_thresh], loc=(0.5,0.908))


plt.gca().add_artist(first_legend)

# Radius was set to 10 for these values
plt.plot([1, 2,4,8, 16, 24, 28, 32], [4.736395, 4.034436, 4.040518, 4.099088, \
    3.941547, 3.575454, 3.688609, 4.548544], color="red")

patch_blur = mpatches.Patch(color='red', label="Blur")
plt.legend(handles=[patch_blur], loc="upper right")

plt.savefig("exectime_mpi")

#plt.show()
