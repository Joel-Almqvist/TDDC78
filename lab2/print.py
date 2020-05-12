import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


plt.plot([1, 2,4,8, 16, 32], [0.311975, 0.174822, 0.090478, 0.061164, \
    0.054895, 0.088259], color="blue")


#n = 800
plt.plot([1, 2,4,8, 16, 32], [0.193653, .115707, 0.071826, 0.049060, \
    0.048571, 0.146408], color="purple")

#n = 600
plt.plot([1, 2,4,8, 16, 32], [0.110192, 0.069908, .054130, 0.036649, \
    0.053916, 0.166972], color="red")

patch_n600 = mpatches.Patch(color='red', label="n = 600")
first_legend = plt.legend(handles=[patch_n600], loc=(0.5,0.908))
plt.gca().add_artist(first_legend)


patch_n800 = mpatches.Patch(color='purple', label="n = 800")
sec = plt.legend(handles=[patch_n800], loc=(0.5,0.808))
plt.gca().add_artist(sec)


patch_n1000 = mpatches.Patch(color='blue', label="n = 1000")
third = plt.legend(handles=[patch_n1000], loc=(0.5,0.708))
plt.gca().add_artist(third)


plt.ylabel("Seconds")
plt.xlabel("Number of threads")
#0.909932 seconds
plt.title("Lab2 with 100 iterations and thresh = 0")
#0.909932 seconds
#patch_thresh = mpatches.Patch(color='blue', label="Threshold")
#first_legend = plt.legend(handles=[patch_thresh], loc=(0.5,0.908))
plt.savefig("lab2_exec_results")
plt.close()
