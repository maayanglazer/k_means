import math
import sys
import pandas as pd
import random
import numpy as np


def euclid_dist(vector1, vector2):
    return np.linalg.norm(vector1 - vector2)


def calc_dx(i, vectors, chosen_indices):
    min_dist = math.inf
    vector1 = vectors.iloc[i].drop(0).to_numpy()
    for j in chosen_indices:
        vector2 = vectors.iloc[j].drop(0).to_numpy()
        dist = euclid_dist(vector1, vector2)
        if dist < min_dist:
            min_dist = dist
    return min_dist


def kmeans_pp(vectors, N, k):
    # if K=N return vectors
    rand_vector = random.randint(0, N - 1)
    chosen_indices = [rand_vector]
    num_chosen = 1

    while num_chosen < k:
        Dx = np.zeros(N)
        num_chosen += 1
        for i in range(N):
            Dx[i] = calc_dx(i, vectors, chosen_indices)
        Dx = Dx / np.sum(Dx)
        new_point = np.random.choice(N, 1, p=Dx)[0]
        chosen_indices.append(new_point)


if __name__ == '__main__':

    try:
        K = sys.argv[1]
        iter = 300
        if len(sys.argv) == 6:
            iter = sys.argv[2]
            eps = sys.argv[3]
            input_1 = sys.argv[4]
            input_2 = sys.argv[5]
        elif len(sys.argv) == 5:
            eps = sys.argv[2]
            input_1 = sys.argv[3]
            input_2 = sys.argv[4]

        df1 = pd.read_csv(input_1, sep=",", header=None)
        df2 = pd.read_csv(input_2, sep=",", header=None)
        df_inner = pd.merge(df1, df2, on=0, how='inner')
        df_sorted = df_inner.sort_values(0)
        N = len(df_sorted.index)

        # check errors
        error = False
        if not str.isdigit(K):
            print("Invalid number of clusters!")
            error = True
        elif int(K) >= N or int(K) <= 1:
            print("Invalid number of clusters!")
            error = True

        if not str.isdigit(iter):
            print("Invalid maximum iteration!")
            error = True
        elif int(iter) >= 1000 or int(iter) <= 1:
            print("Invalid maximum iteration!")
            error = True

        if error:
            exit(1)

        K = int(K)
        iter = int(iter)

        kmeans_pp(df_sorted, N, K)



    except Exception as e:
        print("An Error Has Occurred")
        exit(1)
