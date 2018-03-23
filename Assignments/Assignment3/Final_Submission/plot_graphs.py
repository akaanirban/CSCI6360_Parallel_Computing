#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 15 21:45:46 2018

@author: anirban
"""
import matplotlib.pyplot as plt
import numpy as np

ntasks = [64,128,256,512,1024,2048,4096,8192]
results_p2p = [0.420027,0.210152,0.120292,0.052671,0.030676,0.020943,0.010322,0.007179]
results_mpi = [0.419725,0.209963,0.118705,0.052526,0.029684,0.020122,0.003101,0.000735]


plt.figure()
plt.plot(ntasks, results_mpi, label='MPI_Reduce', marker='o',markerfacecolor='white')
plt.plot(ntasks, results_p2p, label='MPI_P2P_reduce', marker='^')#,markerfacecolor='white')
plt.xlabel('Nunber of Tasks/ MPI ranks')
plt.ylabel('Time in Seconds')
plt.grid(True,which="both",ls="-", lw=.5)
plt.legend()
plt.show



plt.figure()
plt.plot(ntasks, np.log10(results_mpi), label='MPI_Reduce')
plt.plot(ntasks, np.log10(results_p2p), label='MPI_P2P_reduce')
plt.grid(True,which="both",ls="-", lw=.5)
plt.xlabel('Nunber of Tasks/ MPI ranks')
plt.ylabel(r'Time in $log_{10}$(seconds)')
plt.legend()
plt.show
