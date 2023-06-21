import math
import sys
import pandas as pd
import numpy as np
import mykmeanssp

def is_float(string):
    try:
        float(string)
        return True
    except ValueError:
        return False


def calc_dx(i, vectors, chosen_indices):
    min_dist = math.inf
    vector1 = vectors[i]
    for j in chosen_indices:
        vector2 = vectors[j]
        dist = np.linalg.norm(vector1 - vector2)
        if dist < min_dist:
            min_dist = dist
    return min_dist


def kmeans_pp(vectors, N, k, iter, eps):
    # if K=N return vectors
    np.random.seed(0)
    rand_vector = np.random.choice([i for i in range(N - 1)])
    chosen_indices = [rand_vector]
    num_chosen = 1
    np_vectors = []
    for i in range(len(vectors)):
        np_vectors.append(np.array(vectors.iloc[i].drop(0)))

    while num_chosen < k:
        Dx = np.zeros(N)
        num_chosen += 1
        for i in range(N):
            Dx[i] = calc_dx(i, np_vectors, chosen_indices)
        Dx = Dx / np.sum(Dx)
        new_point = np.random.choice(N, 1, p=Dx)[0]
        chosen_indices.append(new_point)
    res_str = ""
    for i, ind in enumerate(chosen_indices):
        if i != len(chosen_indices) - 1:
            res_str += f"{ind},"
        else:
            res_str += f"{ind}\n"
    chosen_vectors = [np.array(vectors.iloc[i].drop(0)).tolist() for i in chosen_indices]
    vectors = [np_vectors[i].tolist() for i in range(vectors.shape[0])]
    K_array = mykmeanssp.fit(K, iter, chosen_vectors, vectors, len(vectors[0]), len(vectors), eps)

    for i in range(K):
        line = ""
        for j in range(len(K_array[0])):
            K_array[i][j] = round(K_array[i][j], 4)
            if j == len(K_array[0]) - 1 and i < K - 1:
                line += f"{K_array[i][j]}\n"
            elif j == len(K_array[0]) - 1:
                line += f"{K_array[i][j]}"
            else:
                line += f"{K_array[i][j]},"
        res_str += line
    print(res_str)


if __name__ == '__main__':

    try:
        K = sys.argv[1]
        iter = "300"
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

        if not is_float(eps):
            error = True

        if error:
            exit(1)

        K = int(K)
        iter = int(iter)
        eps = float(eps)

        kmeans_pp(df_sorted, N, K, iter, eps)



    except Exception as e:
        print("An Error Has Occurred")
        exit(1)
