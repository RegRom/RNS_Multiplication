// RNS_Multiplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//Obliczanie odwrotności multiplikatywnej
int invMultiplication(int a, int m)
{
	int m0 = m, t, q;
	int x0 = 0, x1 = 1;

	if (m == 1)
		return 0;

	while (a > 1)
	{
		q = a / m;
		t = m;

		m = a % m;
		a = t;

		t = x0;

		x0 = x1 - q * x0;
		x1 = t;
	}

	if (x1 < 0)
		x1 += m0;

	return x1;
}

//Konwersja do systemu pozycyjnego
int convToPositional(int base[], int x[], int mods)
{
	// Obliczamy iloczyn modułów
	int M = 1;
	for (int i = 0; i < mods; i++)
		M *= base[i];

	int result = 0;

	// Wdrażamy formułę CRT sumując kolejne projekcje ortogonalne
	for (int i = 0; i < mods; i++)
	{
		int T = M / base[i];
		result += x[i] * invMultiplication(T, base[i]) * T;     //projekcja ortogonalna
	}

	return result % M;          //Upewniamy się, że końcowy wynik mieści się w zakresie
}

//Konwersja do RNS
int *convToRNS(int base[], int x, int mods)
{
	int *x_RNS = new int[mods];
	for (size_t i = 0; i < mods; i++)
	{
		x_RNS[i] = x % base[i];
	}
	return x_RNS;
}

//Rozszerzenie bazy
int *baseExtension(int base1[], int base2[], int mods1, int mods2, int x_RNS[])
{

	int x = convToPositional(base1, x_RNS, mods1);
	int *converted = new int[mods2];

	converted = convToRNS(base2, x, mods2);

	return converted;
}

//Obliczanie odwrotności multiplikatywnej dla liczby zapisanej w systemie RNS
int *invRNS(int base[], int size, int xRNS)
{
	int *inverted = new int[size];
	for (size_t i = 0; i < size; i++)
	{
		inverted[i] = invMultiplication(xRNS, base[i]);
	}
	return inverted;
}

//Redukcja Montgomery'ego
int *rnsMR(int base1[], int base2[], int mods1, int mods2, int x_RNS1[], int x_RNS2[], int P)
{
	int *Pa = convToRNS(base1, P, mods1);
	int *Pb = convToRNS(base2, P, mods2);
	int Ma = 1, *invM;
	int *Qa = new int[mods1];
	int *Qb = new int[mods2];
	int *Rb = new int[mods2];
	int *Sb = new int[mods2];

	for (size_t i = 0; i < mods1; i++)
	{
		Ma *= base1[i];
	}
	
	invM = invRNS(base2, mods2, Ma);
	/*
	int tmpInvM = invMultiplication(Ma, P);
	int tmpX = convToPositional(base1, x_RNS1, mods1);
	int tmpResult = (tmpX * tmpInvM) % P;
	std::cout << "\n" << tmpResult << "\n";
	*/
	int *invP = invRNS(base1, mods1, P);
	for (size_t i = 0; i < mods1; i++)
	{
		invP[i] = (base1[i] - invP[i] % base1[i]) % base1[i];
	}

	for (size_t i = 0; i < mods1; i++)
	{
		Qa[i] = (x_RNS1[i] * invP[i]) % base1[i];
	}

	Qb = baseExtension(base1, base2, mods1, mods2, Qa);

	for (size_t i = 0; i < mods2; i++)
	{
		//Rb[i] = (((x_RNS2[i] + Qb[i]) % base2[i]) * Pb[i]) % base2[i];
		Rb[i] = (x_RNS2[i] + ((Qb[i] * Pb[i]) % base2[i])) % base2[i];
	}

	for (size_t i = 0; i < mods2; i++)
	{
		Sb[i] = (Rb[i] * invM[i]) % base2[i];
	}

	return Sb;


}

//Wprowadzanie bazy 
int *baseInput(int size)
{
	int *base = new int[size];
	for (size_t i = 0; i < size; i++)
	{
		std::cout << "Input a module: ";		
		std::cin >> base[i];				//Podajemy kolejne moduły
	}
	return base;
}

//Wyświetlanie liczby w systemie RNS
void rnsDisplay(int size, int *num)
{
	std::cout << "< ";
	for (size_t i = 0; i < size; i++)
	{
		std::cout << num[i] << " ";
	}
	std::cout << ">";
}

int main()
{
	char choice;
	int X, P, size1 = 3, size2 = 3;
	int base1[] = { 3,5,7 };
	int base2[] = { 11,13,17 };
	int *customBase1, *customBase2;

	std::cout << "Welcome in RNS multiplication program\n";
	std::cout << "Input integers you want to multiply:\n";
	std::cout << "Input X: ";
	std::cin >> X;
	std::cout << "Input P, remember that it has to be a prime: ";
	std::cin >> P;
	std::cout << "Do you want to use default parameters? y/n: ";
	std::cin >> choice;
	if (choice == 'n')
	{
		std::cout << "Input first base size: ";
		std::cin >> size1;
		std::cout << "Input second base size: ";
		std::cin >> size2;
		std::cout << "Input first base moduli:\n";
		customBase1 = baseInput(size1);
		std::cout << "Input first base moduli:\n";
		customBase2 = baseInput(size2);

		int *X_RNSa = new int[size1];
		X_RNSa = convToRNS(customBase1, X, size1);
		int *X_RNSb = new int[size2];
		X_RNSb = convToRNS(customBase2, X, size2);

		int *Sb = rnsMR(customBase1, customBase2, size1, size2, X_RNSa, X_RNSb, P);

		int *Sa = new int[size1];
		Sa = baseExtension(customBase2, customBase1, size2, size1, Sb);

		std::cout << "\nThe result of Montgomery Reduction is: ";
		rnsDisplay(size1, Sa);
		std::cout << "\nThe result of Montgomery Reduction is: ";
		rnsDisplay(size1, Sb);

		delete customBase1, customBase2, X_RNSa, X_RNSb, Sa, Sb;
	}
	else
	{
		int *X_RNSa = new int[size1];
		X_RNSa = convToRNS(base1, X, size1);
		int *X_RNSb = new int[size2];
		X_RNSb = convToRNS(base2, X, size2);

		int *Sb = rnsMR(base1, base2, size1, size2, X_RNSa, X_RNSb, P);

		int *Sa = new int[size1];
		Sa = baseExtension(base2, base1, size2, size1, Sb);

		std::cout << "\nThe result of Montgomery Reduction is: ";
		std::cout << "\nIn base A: ";
		rnsDisplay(size1, Sa);
		std::cout << "\nIn base B: ";
		rnsDisplay(size2, Sb);
		int posResult = convToPositional(base2, Sb, size2);
		std::cout << "\nIn positional system: " << posResult;

		delete X_RNSa, X_RNSb, Sa, Sb;
	}

	std::cout << "\n\n";

	return 0;
}
