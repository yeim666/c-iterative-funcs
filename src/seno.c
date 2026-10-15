/*
 * seno.c - Ejercicio 1: aproximacion de sin(x) mediante su serie de Maclaurin.
 *
 *   sin(x) = sum_{k>=0} (-1)^k x^(2k+1) / (2k+1)!
 *
 * Recurrencia (sin calcular factoriales):
 *   t0 = r,   t_k = -t_{k-1} * r^2 / ((2k)(2k+1)),   k >= 1
 * donde r es el angulo reducido a [-pi, pi].
 *
 * Entradas: x en radianes con |x| <= 100 y n (numero de terminos) de 1 a 100.
 * Se usa for porque el usuario fija cuantos terminos se suman.
 */
#include <stdio.h>
#include <math.h>

int main(void) {
    double x;
    int n;

    printf("Ingrese x (radianes, |x| <= 100) y numero de terminos (1 a 100): ");

    /* Valida que se lean 2 datos, que x sea finito y que ambos esten en rango */
    if (scanf("%lf %d", &x, &n) != 2 || !isfinite(x) ||
        fabs(x) > 100.0 || n < 1 || n > 100) {
        fprintf(stderr, "Datos invalidos: use |x| <= 100 y 1 <= n <= 100.\n");
        return 1;
    }

    /* pi se obtiene con acos(-1.0) para no depender de la macro M_PI */
    const double pi = acos(-1.0);

    /* Reduccion por periodicidad: sin(x) = sin(r) con r en [-pi, pi] */
    double r = remainder(x, 2.0 * pi);

    /* Primer termino (k = 0): t0 = r; la suma arranca con ese termino */
    double termino = r;
    double suma = r;

    /* Se agregan los terminos k = 1 .. n-1 (n terminos en total) */
    for (int k = 1; k < n; ++k) {
        termino *= -r * r / ((2.0 * k) * (2.0 * k + 1.0));
        suma += termino;
    }

    /* Referencia de math.h usada solo para verificar, no para calcular */
    double ref = sin(x);
    double ea = fabs(suma - ref);

    printf("Argumento reducido: %.15g\n", r);
    printf("Serie (%d terminos): %.15g\n", n, suma);
    printf("sin(x) referencia:   %.15g\n", ref);
    printf("Error absoluto:      %.3e\n", ea);

    /* El error relativo solo tiene sentido si la referencia no es casi cero */
    if (fabs(ref) > 1e-12) {
        printf("Error relativo:      %.3e\n", ea / fabs(ref));
    } else {
        printf("Error relativo:      no informativo (referencia ~ 0)\n");
    }
    return 0;
}
