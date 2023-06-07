import sys
import pandas as pd

if __name__ == '__main__':

    try:
        K = sys.argv[1]
        if len(sys.argv) == 6:
            iter = sys.argv[2]
            eps = sys.argv[3]
            input_1 = sys.argv[4]
            input_2 = sys.argv[5]
        elif len(sys.argv) == 5:
            iter = 300
            eps = sys.argv[2]
            input_1 = sys.argv[3]
            input_2 = sys.argv[4]

        df1 = pd.read_csv(input_1, sep=",", header=None)
        df2 = pd.read_csv(input_2, sep=",", header=None)
        df_inner = pd.merge(df1, df2, on=0, how='inner')
        df_sorted = df_inner.sort_values(0)
        # check errors
        error = False
        if not str.isdigit(K):
            print("Invalid number of clusters!")
            error = True
        # elif int(K) >= len(vectors) or int(K) <= 1:
        #     print("Invalid number of clusters!")
        #     error = True

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

        # find_kmeans(K, iter, vectors)

    except Exception as e:
        print("An Error Has Occurred")
        exit(1)
