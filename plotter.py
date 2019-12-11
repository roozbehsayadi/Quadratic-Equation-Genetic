
import sys
import matplotlib.pyplot as plt
import numpy as np

def plotQuadraticFunction(a, b, c):
	x = np.arange(-500, 500, 1, dtype=np.float64)
	y = a*(x**2) + b*x + c
	plt.scatter(x=x, y=y, marker='o', edgecolors='red')


x = np.genfromtxt('x_train.csv', delimiter=',')
y = np.genfromtxt('y_train.csv', delimiter=',')

plt.scatter(x=x, y=y, marker='o', edgecolors='blue')

a, b, c = sys.argv[1:4]
a = float(a)
b = float(b)
c = float(c)

plotQuadraticFunction(a, b, c)

plt.show()
