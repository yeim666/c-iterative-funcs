/*
 * logaritmo.c - Ejercicio 2: aproximacion de ln(x) para x > 0.
 *
 *   ln(x) = 2 * sum_{k>=0} z^(2k+1) / (2k+1),   z = (x-1)/(x+1),  |z| < 1
 *
 * Sumando completo a_k = 2 z^(2k+1) / (2k+1). Recurrencia:
 *   a_0 = 2z,   a_{k+1} = a_k * z^2 * (2k+1) / (2k+3)
 * En el codigo se mantiene la potencia zp = z^(2k+1) y se multiplica por z^2
 * en cada paso; el factor 2/(2k+1) se aplica al sumar.
 *
 * Cota de cola tras sumar M terminos:
 *   |R_M| <= 2 |z|^(2M+1) / ((2M+1)(1 - z^2))
 * El ciclo while se detiene cuando la cota es <= tolerancia.
 *
 * Entradas: 0.1 <= x <= 10 y 1e-12 <= eps <= 1e-3.
 */
#include <stdio.h>
#include <math.h>

/* Maximo de terminos; se puede cambiar al compilar con -DMAX_TERMINOS=n */
#ifndef MAX_TERMINOS
#define MAX_TERMINOS 100000
#endif

int main(void) {
    double x, eps;

    printf("Ingrese x (0.1 a 10) y tolerancia (1e-12 a 1e-3): ");

    /* Valida lectura de 2 datos, valores finitos y rangos admitidos */
    if (scanf("%lf %lf", &x, &eps) != 2 || !isfinite(x) || !isfinite(eps) ||
        x < 0.1 || x > 10.0 || eps < 1e-12 || eps > 1e-3) {
        fprintf(stderr, "Datos invalidos: use 0.1 <= x <= 10 y 1e-12 <= eps <= 1e-3.\n");
        return 1;
    }

    double z = (x - 1.0) / (x + 1.0);   /* variable auxiliar, |z| < 1 */
    double z2 = z * z;                  /* z^2, razon entre potencias */
    double zp = z;                      /* z^(2M+1) del proximo termino */
    double suma = 0.0;                  /* acumulado de la serie */
    int m = 0;                          /* terminos ya sumados */

    /* Cota de cola con M = 0 terminos sumados */
    double cota = 2.0 * fabs(zp) / ((2.0 * m + 1.0) * (1.0 - z2));

    /* Se suma mientras la cota supere la tolerancia y no se agote el maximo */
    while (cota > eps && m < MAX_TERMINOS) {
        suma += 2.0 * zp / (2.0 * m + 1.0);   /* agrega a_m */
        zp *= z2;                             /* pasa a z^(2(m+1)+1) */
        ++m;
        cota = 2.0 * fabs(zp) / ((2.0 * m + 1.0) * (1.0 - z2));
    }

    /* Si salio del ciclo por el maximo y no por la tolerancia, se avisa */
    if (cota > eps) {
        fprintf(stderr, "No se alcanzo la tolerancia con %d terminos (cota %.3e).\n",
                m, cota);
        return 2;
    }

    /* Referencia de math.h usada solo para verificar */
    double ref = log(x);
    double ea = fabs(suma - ref);

    printf("Terminos:       %d\n", m);
    printf("Serie:          %.15g\n", suma);
    printf("log(x) ref.:    %.15g\n", ref);
    printf("Cota de cola:   %.3e\n", cota);
    printf("Error absoluto: %.3e\n", ea);

    /* El error relativo no es informativo cerca de x = 1 (ln x ~ 0) */
    if (fabs(ref) > 1e-12) {
        printf("Error relativo: %.3e\n", ea / fabs(ref));
    } else {
        printf("Error relativo: no informativo (referencia ~ 0)\n");
    }
    return 0;
}
