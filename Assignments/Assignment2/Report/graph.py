#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 15 19:16:23 2018

@author: anirban
"""
import numpy as np
import matplotlib.pyplot as plt


if __name__ == "__main__":
    rippleCarry = 0.0566
    woBarrier_1 = 48.1984
    woBarrier_2 = [12.2498, 12.2506]
    woBarrier_4 = [3.0137, 3.0892, 3.0885, 3.0894]
    woBarrier_8 = [0.7599, 0.7632, 0.7745, 0.7815, 0.7820, 0.8605, 0.8609, 0.8612]
    woBarrier_16 = [0.2055, 0.2061, 0.2060, 0.2062, 0.2347, 0.3028, 0.3057, 0.3291, 0.3300, 0.3302, 0.3357, 0.3361, 0.3360, 0.3361, 0.3362, 0.3368]
    
    withBarrier_1 = 48.5256
    withBarrier_2 = [12.5494, 12.5502]
    withBarrier_4 = [3.1250, 3.1244, 3.1237, 3.1245]
    withBarrier_8 = [0.8595, 0.8589, 0.8589, 0.8590, 0.8593, 0.8591, 0.8592, 0.8608]
    withBarrier_16 = [0.4656, 0.4656, 0.4559, 0.4654, 0.4657, 0.4657, 0.4657, 0.4659, 0.4564, 0.4689, 0.4563, 0.4661, 0.4658, 0.4664, 0.4662, 0.4678]

    ranks = [2, 4, 8, 16]
    withoutBarrierTimings = [np.sum(woBarrier_2)/len(woBarrier_2), np.sum(woBarrier_4)/len(woBarrier_4), np.sum(woBarrier_8)/len(woBarrier_8), np.sum(woBarrier_16)/len(woBarrier_16)]
    withBarrierTimings = [np.sum(withBarrier_2)/len(withBarrier_2), np.sum(withBarrier_4)/len(withBarrier_4), np.sum(withBarrier_8)/len(withBarrier_8), np.sum(withBarrier_16)/len(withBarrier_16)]
    
    
    #---------------------------------------GRAPH 1----------------------------
    plt.figure()
    plt.plot(ranks, withoutBarrierTimings, '-bo',label='Without Barrier')
    plt.plot(ranks, withBarrierTimings, '-rx', label='With Barrier')
    plt.legend()
    plt.title('Execution time of 2, 4, 8 and 16 rank runs as function of their number of ranks')
    plt.xlabel('Ranks')
    plt.ylabel('Execution time')
    plt.show()
    
    plt.figure()
    plt.semilogy(ranks, withoutBarrierTimings, '-bo', label='Without Barrier')
    plt.semilogy(ranks, withBarrierTimings, '-rx', label='With Barrier')
    plt.legend()
    plt.title('Execution time of 2, 4, 8 and 16 rank runs as function of their number of ranks, semilogy')
    plt.xlabel('Ranks')
    plt.ylabel('Execution time, semilogy')
    plt.show()
    
    
    #------------------------------------GRAPH 2-------------------------------
    speedup_2_wob = woBarrier_1 / (np.sum(woBarrier_2)/len(woBarrier_2))
    speedup_4_wob = woBarrier_1 / (np.sum(woBarrier_4)/len(woBarrier_4))
    speedup_8_wob = woBarrier_1 / (np.sum(woBarrier_8)/len(woBarrier_8))
    speedup_16_wob = woBarrier_1 / (np.sum(woBarrier_16)/len(woBarrier_16))
    speedup_wob = [speedup_2_wob, speedup_4_wob, speedup_8_wob, speedup_16_wob]
    
    speedup_2_wb = withBarrier_1 / (np.sum(withBarrier_2)/len(withBarrier_2))
    speedup_4_wb = withBarrier_1 / (np.sum(withBarrier_4)/len(withBarrier_4))
    speedup_8_wb = withBarrier_1 / (np.sum(withBarrier_8)/len(withBarrier_8))
    speedup_16_wb = withBarrier_1 / (np.sum(withBarrier_16)/len(withBarrier_16))
    speedup_wb = [speedup_2_wb, speedup_4_wb, speedup_8_wb, speedup_16_wb]
    
    plt.figure()
    plt.plot(ranks, np.log10(speedup_wob), '-bo',label='Without Barrier')
    plt.plot(ranks, np.log10(speedup_wb), '-rx', label='With Barrier')
    plt.legend()
    plt.title('Speedup of 2, 4, 8 and 16 rank runs relative to serial MPI CLA run with no. of Rank 1')
    plt.xlabel('Ranks')
    plt.ylabel(r'$\log_{10} (\times$ Speedup $)$')
    plt.show()
    
    #----------------------------------Graph 3---------------------------------
    speedup_2_wob = rippleCarry / (np.sum(woBarrier_2)/len(woBarrier_2))
    speedup_4_wob = rippleCarry / (np.sum(woBarrier_4)/len(woBarrier_4))
    speedup_8_wob = rippleCarry / (np.sum(woBarrier_8)/len(woBarrier_8))
    speedup_16_wob = rippleCarry / (np.sum(woBarrier_16)/len(woBarrier_16))
    speedup_wob = [speedup_2_wob, speedup_4_wob, speedup_8_wob, speedup_16_wob]
    
    speedup_2_wb = rippleCarry / (np.sum(withBarrier_2)/len(withBarrier_2))
    speedup_4_wb = rippleCarry / (np.sum(withBarrier_4)/len(withBarrier_4))
    speedup_8_wb = rippleCarry / (np.sum(withBarrier_8)/len(withBarrier_8))
    speedup_16_wb = rippleCarry / (np.sum(withBarrier_16)/len(withBarrier_16))
    speedup_wb = [speedup_2_wb, speedup_4_wb, speedup_8_wb, speedup_16_wb]
    
    plt.figure()
    plt.plot(ranks, (speedup_wob), '-bo',label='Without Barrier')
    plt.plot(ranks, (speedup_wb), '-rx', label='With Barrier')
    plt.legend()
    plt.title('Speedup of 2, 4, 8 and 16 rank runs relative to serial ripple carry adder')
    plt.xlabel('Ranks')
    plt.ylabel(r'$\times$ Speedup')
    plt.show()
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    