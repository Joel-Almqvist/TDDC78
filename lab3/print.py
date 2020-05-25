import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

plt.bar(np.arange(5), [0.001079, 0.002183, 0.004148, 0.008215, \
    0.016426], color="blue", width=1)
plt.xticks([1,2,3,4,5], ("250","500","1000","2000","4000"))


plt.ylabel("Average pressure")
plt.xlabel("Number of particles")
plt.title("Pressure changes due to different particle amount")
plt.savefig("plots/pressure_to_n")
plt.close()


plt.bar(np.arange(5), [2.208953, 0.605501, 0.155660, 0.037965, \
    0.009858], color="red", width=1)
plt.xticks([1,2,3,4,5], ("2.5","10","40","160","640"))


plt.ylabel("Average pressure")
plt.xlabel("Volume in potens 10^5")
plt.title("Pressure changes due to different volume")
plt.savefig("plots/pressure_to_v")
plt.close()




plt.plot([1, 2, 4, 8, 16, 32, 64, 128, 256], [121, 31, 8.46, 2.22, 0.668, 0.22, 0.067, 0.04, 0.03], color="orange")
plt.ylabel("Seconds")
plt.xlabel("proccceses n")
plt.title("Execution time using t=1000, n=4000 and size 10^4 * 10^4")
plt.savefig("plots/exec_time_large")
#plt.show()
plt.close()


plt.plot([1, 2, 4, 8, 16, 32, 64], [0.386606, 0.108631, 0.032356, 0.013551, 0.009565, 0.006060, 0.003957], color="purple")
plt.ylabel("Seconds")
plt.xlabel("proccceses n")
plt.title("Execution time using t=200, n=500 and size 2*10^4 * 2*10^4")
plt.savefig("plots/exec_time_small")
#plt.show()
plt.close()


plt.plot([1, 2, 4, 8, 16, 32, 64], [0.161802, 0.171420, 0.195660, 0.203571, 0.231738, 0.267691, 0.294570], color="purple")
plt.ylabel("Seconds")
plt.xlabel("proccceses n")
plt.title("Execution time")
plt.savefig("plots/pscale")
plt.close()
