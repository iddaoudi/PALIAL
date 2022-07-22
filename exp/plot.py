#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File              : plot.py
# Author            : Idriss Daoudi <idaoudi@anl.gov>
# Date              : 20.07.2022
# Last Modified Date: 20.07.2022
# Last Modified By  : Idriss Daoudi <idaoudi@anl.gov>

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import csv
import os.path

matrix_sizes = ["8192", "16384", "24576", "32768"]
thread_sizes = ["31", "63"]
governors    = ["userspace"] #, "userspace"]
note         = "_syrk_trsm_to_min"

all_tasks = []
all_times = []
tasks = ["gemm", "potrf", "trsm", "syrk"]
#tasks = ["potrf"]
task_name = ""
tmp_index = []
tmp_times = []
index = 0
counter = 0

for matrix in matrix_sizes:
    for thread in thread_sizes:
        for governor in governors:
            file_name = "tasks_times_" + matrix + "_512_" + thread + "_" + governor + note
            if os.path.exists("data/" + file_name):
                my_file = open("data/" + file_name, "r")
                csv_file = csv.reader(my_file)
                for row in csv_file:
                    for char in row[0]:
                        if char.isdigit():
                            index = row[0].index(char)
                            task_name = row[0][0:index]
                            all_tasks.append(task_name)
                            break
                    all_times.append(row[1])

        index = 1
        
        sns.set_theme(style="darkgrid")
        for t in tasks:
            for task_name in all_tasks: 
                if task_name == t:
                    tmp_index.append(task_name)
                    tmp_times.append(float(all_times[counter]))
                counter += 1
            data_plot = pd.DataFrame({"Tasks":tmp_index, "Times":tmp_times})
            sns.violinplot(data = data_plot, x = "Tasks", y = "Times", inner=None, palette="muted")
            sns.stripplot(data = data_plot, x = "Tasks", y = "Times", alpha=0.5, s=5, linewidth=1.0)
            
            plt.xlabel("Task type")
            plt.ylabel("Time (ms)")
            
            counter = 0
            index += 1
        
        #plt.show()
        plt.savefig(file_name + ".png", format='png')


my_file.close()
