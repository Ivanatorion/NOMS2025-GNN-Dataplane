#!/usr/bin/python3

import sys
from scipy.stats import ranksums

EXPERIMENTS = ["ECN", "NoECN", "GNN"]

N_LINES = -1

expJains = {}
expTps = {}

logsDir = sys.argv[1]
if logsDir[-1] == '/':
    logsDir = logsDir[:-1]

for experiment in EXPERIMENTS:
    files = []
    for i in range(1, 5):
        fName = logsDir + "/iperfLogs" + experiment + "/h" + str(i) + ".txt"
        with open(fName) as file:
            files.append([line.rstrip() for line in file])
    
        if N_LINES == -1:
            N_LINES = len(files[0])
        elif len(files[i-1]) != N_LINES:
            print("ERROR: Number of lines differ for file \"" + fName + "\" (" + str(len(files[i-1])) + ")")
            sys.exit(1)

        for j in range(len(files[i-1])):
            crs = files[i-1][j].find("/sec")
            while files[i-1][j][crs] != ' ':
                crs = crs - 1
            endCrs = crs
            crs = crs - 1
            while files[i-1][j][crs] != ' ':
                crs = crs - 1
            files[i-1][j] = files[i-1][j][crs+1:endCrs]

    jains = []
    tps = []
    for i in range(0, N_LINES):
        totalTP = 0.0
        divisor = 0.0
        for j in range(0, 4):
            n = float(files[j][i])
            totalTP = totalTP + n
            divisor = divisor + n * n
        jains.append((totalTP * totalTP) / (4 * (divisor)))
        tps.append(totalTP)
        
    averageJ = 0.0
    averageTP = 0.0
    for j in jains:
        averageJ = averageJ + j
    for t in tps:
        averageTP = averageTP + t
    averageJ = averageJ / len(jains)
    averageTP = averageTP / len(tps)
    print("Average Jain " + experiment + ": " + str(averageJ) + " (TP: " + str(averageTP) + ")")

    expJains[experiment] = jains
    expTps[experiment] = tps

for i in range(0, len(EXPERIMENTS)):
    for j in range(i + 1, len(EXPERIMENTS)):
        print("RankSum - " + EXPERIMENTS[i] + " vs " + EXPERIMENTS[j])
        print(ranksums(expJains[EXPERIMENTS[i]], expJains[EXPERIMENTS[j]]))

    