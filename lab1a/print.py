import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


#plt.plot([1, 2,4,8,16, 32], [3.330033, 4.024569,3.482626, 3.254335, \
#    3.582518, 4.653898], color="blue")

# Thresh
plt.plot([1, 2,4,8], [0.014573, 0.011328,0.009878, 0.014445 \
    ], color="blue")



plt.ylabel("Seconds")
plt.xlabel("Number of tasks")
#0.909932 seconds
plt.title("Thresh MPI execution time on 3000x3000")
#0.909932 seconds
#patch_thresh = mpatches.Patch(color='blue', label="Threshold")
#first_legend = plt.legend(handles=[patch_thresh], loc=(0.5,0.908))
plt.savefig("thresh_mpi")
plt.close()
#plt.gca().add_artist(first_legend)

# Radius was set to 2 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.237409, 0.130043, 0.068725, 0.042342, \
    0.028090, 0.019774, 0.016857], color="red")

# Radius was set to 8 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.633665, 0.335970, 0.169589, 0.097996, \
    0.055202, 0.036616, 0.048406], color="orange")



# Radius was set to 10 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.792625, 0.399015, 0.205152, 0.111148, \
    0.061538, 0.042425, 0.054249], color="purple")


# Radius was set to 12 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.909932, 0.464893, 0.237673, 0.131003, \
    0.081481, 0.048663, 0.060083], color="blue")

patch_blur_r2 = mpatches.Patch(color='red', label="Radius 2")
first_legend = plt.legend(handles=[patch_blur_r2], loc=(0.5,0.908))
plt.gca().add_artist(first_legend)

patch_blur_r8 = mpatches.Patch(color='orange', label="Radius 8")
sec = plt.legend(handles=[patch_blur_r8], loc=(0.5,0.808))
plt.gca().add_artist(sec)


patch_blur_r10 = mpatches.Patch(color='purple', label="Radius 10")
third = plt.legend(handles=[patch_blur_r10], loc=(0.5,0.708))
plt.gca().add_artist(third)

patch_blur_r12 = mpatches.Patch(color='blue', label="Radius 12")
fourth = plt.legend(handles=[patch_blur_r12], loc=(0.5,0.608))
plt.gca().add_artist(fourth)




plt.ylabel("Seconds")
plt.xlabel("Number of tasks")
plt.title("Blur MPI execution time on 3000x3000")

plt.savefig("blurmpi_mpi_v2")
#plt.show()
