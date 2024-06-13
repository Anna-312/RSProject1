#define _USE_MATH_DEFINES
#include <cmath>
#include "RS.h"
int n1, n2;
float x[100];
float y[100];
float tau, h1, h2;
float C1, C2, a, b;
float t;

float f(float x, float y, float t)
{
	return C1 * t * exp(-(pow((x - 5*a/12), 2) + pow((y - 7*b/12), 2)));
}
float g1(float x, float t)
{
	return 0;
}
float g2(float y, float t)
{
	return C2 * t * sin(M_PI * y / b);
}

float g3(float x, float t)
{
	return 0;
}

float g4(float y, float t)
{
	return C2 * t * sin(M_PI * y / b);
}

void init_RS(float u[][100])
{
	h1 = 0.25;
	h2 = 0.25;
	tau = 0.5;
	n1 = a / h1 + 1;
	n2 = b / h2 + 1;
	t = 0;
	for (int i = 0; i < n1; i++)
		for (int j = 0; j < n2; j++)
			u[j][i] = 0;
	for (int i = 0; i < n1; i++)
		x[i] = i * h1;
	for (int j = 0; j < n2; j++)
		y[j] = j * h2;
}
float* progonka_X(float u[100][100], float u_temp[100][100], int j, float t)
{
	float k1 = -tau * h2 * h2;
	float k2 = 2 * tau * h2 * h2 + 2 * h1 * h1 * h2 * h2;
	float* A = new float[n1 - 2] {0};
	float* b = new float[n1 - 2] {0};
	for (int i = 1; i < n1 - 2 + 1; i++)
	{
		A[i - 1] = k2;
		b[i - 1] = tau * h1 * h1 * u[j - 1][i] + (-2 * tau * h1 * h1 + 2 * h1 * h1 * h2 * h2) * u[j][i] + tau * h1 * h1 * u[j + 1][i] + tau * h1 * h1 * h2 * h2 * f(x[i], y[j], t + tau / 2);
	}
	b[0] = b[0] - k1 * u_temp[j][0];
	b[n1 - 2 - 1] = b[n1 - 2 - 1] - k1 * u_temp[j][n1 - 1];
	float k_temp = 0;
	for (int i = 0; i < n1 - 2 - 1; i++)
	{
		k_temp = k1 / A[i];
		A[i + 1] -= k_temp * k1;
		b[i + 1] -= k_temp * b[i];
	}
	b[n1 - 2 - 1] /= A[n1 - 2 - 1];
	for (int i = n1 - 2 - 2; i >= 0; i--)
		b[i] = (b[i] - b[i + 1] * k1) / A[i];
	delete[] A;
	return b;
}

float* progonka_Y(float u[100][100], float u_temp[100][100], int i, float t)
{
	float k1 = -tau * h1 * h1;
	float k2 = 2 * tau * h1 * h1 + 2 * h1 * h1 * h2 * h2;
	float* A = new float[n2 - 2] {0};
	float* b = new float[n2 - 2] {0};
	for (int j = 1; j < n2 - 2 + 1; j++)
	{
		A[j - 1] = k2;
		b[j - 1] = tau * h2 * h2 * u_temp[j][i - 1] + (-2 * tau * h2 * h2 + 2 * h1 * h1 * h2 * h2) * u_temp[j][i] + tau * h2 * h2 * u_temp[j][i + 1] + tau * h1 * h1 * h2 * h2 * f(x[i], y[j], t + tau / 2);
	}
	b[0] = b[0] - k1 * u[0][i];
	b[n2 - 2 - 1] = b[n2 - 2 - 1] - k1 * u[n2 - 1][i];
	float k_temp = 0;
	for (int i = 0; i < n2 - 2 - 1; i++)
	{
		k_temp = k1 / A[i];
		A[i + 1] -= k_temp * k1;
		b[i + 1] -= k_temp * b[i];
	}
	b[n2 - 2 - 1] /= A[n2 - 2 - 1];
	for (int i = n2 - 2 - 2; i >= 0; i--)
		b[i] = (b[i] - b[i + 1] * k1) / A[i];
	delete[] A;
	return b;
}

void next_step(float u[100][100])
{
	float u_temp[100][100];
	for (int i = 0; i < n1; i++)
	{
		u_temp[0][i] = g1(x[i], t + tau / 2);
		u_temp[n2 - 1][i] = g3(x[i], t + tau / 2);
	}
	for (int j = 0; j < n2; j++)
	{
		u_temp[j][0] = g2(y[j], t + tau / 2);
		u_temp[j][n1 - 1] = g4(y[j], t + tau / 2);
	}
	float* bx;
	for (int j = 1; j < n2 - 1; j++)
	{
		bx = progonka_X(u, u_temp, j, t);
		for (int i = 0; i < n1 - 2; i++)
			u_temp[j][i + 1] = bx[i];
		delete[] bx;
	}

	for (int i = 0; i < n1; i++)
	{
		u[0][i] = g1(x[i], t + tau);
		u[n2 - 1][i] = g3(x[i], t + tau);
	}
	for (int j = 0; j < n2; j++)
	{
		u[j][0] = g2(y[j], t + tau);
		u[j][n1 - 1] = g4(y[j], t + tau);
	}
	for (int i = 1; i < n1 - 1; i++)
	{
		bx = progonka_Y(u, u_temp, i, t);
		for (int j = 0; j < n2 - 2; j++)
			u[j + 1][i] = bx[j];
		delete[] bx;
	}
	t += tau;
}