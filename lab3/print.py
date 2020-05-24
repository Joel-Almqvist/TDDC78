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
