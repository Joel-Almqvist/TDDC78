import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


#plt.plot([1, 2,4,8,16, 32], [3.330033, 4.024569,3.482626, 3.254335, \
#    3.582518, 4.653898], color="blue")

# Thresh
plt.plot([1, 2,4,8, 16, 32, 64, 128], [0.0363923, 0.0290113, 0.0157846, 0.00897164, \
    0.00578816, 0.00625684, 0.00976335, 0.0147044], color="blue")



plt.ylabel("Seconds")
plt.xlabel("Number of threads")
#0.909932 seconds
plt.title("Thresh pthreads execution time on 3000x3000")
#0.909932 seconds
#patch_thresh = mpatches.Patch(color='blue', label="Threshold")
#first_legend = plt.legend(handles=[patch_thresh], loc=(0.5,0.908))
plt.savefig("thresh_pt")
plt.close()
#plt.gca().add_artist(first_legend)


# Radius was set to 8 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.693649, 0.372533, 0.197879, 0.11383, \
    0.0754018, 0.0746653, 0.076905], color="orange")



# Radius was set to 10 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [0.861889, 0.45434, 0.290246, 0.133842, \
    0.103648, 0.0961326, 0.0828006], color="purple")


# Radius was set to 12 for these values
plt.plot([1, 2, 4, 8, 16, 32, 64], [1.09903, 0.511167, 0.321628, 0.161096, \
    0.0958911,  0.103535, 0.0994377], color="blue")


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
plt.xlabel("Number of threads")
plt.title("Blur pthreads execution time on 3000x3000")

plt.savefig("blur_pt")
#plt.show()
