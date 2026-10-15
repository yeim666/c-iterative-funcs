/*
 * arctangente.c - Ejercicio 3: aproximacion de arctan(x), resultado en radianes.
 *
 *   arctan(u) = sum_{k>=0} (-1)^k u^(2k+1) / (2k+1),   |u| <= 1
 *
 * Recurrencia del sumando completo:
 *   t_0 = u,   t_{k+1} = -t_k * u^2 * (2k+1) / (2k+3)
 *
 * Para |x| > 1 se usa:  arctan(x) = sgn(x) * pi/2 - arctan(1/x),
 * de modo que la serie siempre se evalua con |u| <= 1.
 *
 * Como la serie es alternante con terminos decrecientes en magnitud, el
 * primer termino omitido acota el error de truncamiento. El while se detiene
 * cuando ese termino tiene magnitud <= eps.
 *
 * Entradas: |x| <= 100 y 1e-4 <= eps <= 1e-2.
 */
#include <stdio.h>
#include <math.h>

/* Maximo de terminos; se puede cambiar al compilar con -DMAX_TERMINOS=n */
#ifndef MAX_TERMINOS
#define MAX_TERMINOS 100000
#endif

int main(void) {
    double x, eps;

    printf("Ingrese x (|x| <= 100) y tolerancia (1e-4 a 1e-2): ");

    /* Valida lectura de 2 datos, valores finitos y rangos admitidos */
    if (scanf("%lf %lf", &x, &eps) != 2 || !isfinite(x) || !isfinite(eps) ||
        fabs(x) > 100.0 || eps < 1e-4 || eps > 1e-2) {
        fprintf(stderr, "Datos invalidos: use |x| <= 100 y 1e-4 <= eps <= 1e-2.\n");
        return 1;
    }

    const double pi = acos(-1.0);
    double resultado = 0.0;
    int m = 0;                 /* terminos sumados */
    double omitido = 0.0;      /* magnitud del primer termino omitido */

    /* x = 0 se trata aparte, antes de cualquier inversion 1/x */
    if (x == 0.0) {
        resultado = 0.0;
    } else {
        double u;              /* argumento que entra a la serie */
        int invertido = 0;     /* 1 si se uso la identidad para |x| > 1 */

        /* if/else: elige el argumento segun el tamano de |x| */
        if (fabs(x) > 1.0) {
            u = 1.0 / x;
            invertido = 1;
        } else {
            u = x;
        }

        double termino = u;    /* t_0, primer termino aun no sumado */
        double suma = 0.0;

        /* Suma mientras el termino pendiente supere eps y no se agote el maximo */
        while (fabs(termino) > eps && m < MAX_TERMINOS) {
            suma += termino;
            termino *= -u * u * (2.0 * m + 1.0) / (2.0 * m + 3.0);
            ++m;
        }
        omitido = fabs(termino);

        /* Si el maximo corto el ciclo antes de cumplir eps, se avisa */
        if (omitido > eps) {
            fprintf(stderr, "No se alcanzo la tolerancia con %d terminos (termino omitido %.3e).\n",
                    m, omitido);
            return 2;
        }

        /* Correccion para |x| > 1: sgn(x)*pi/2 - arctan(1/x) */
        if (invertido) {
            double signo = (x > 0.0) ? 1.0 : -1.0;
            resultado = signo * pi / 2.0 - suma;
        } else {
            resultado = suma;
        }
    }

    /* Referencia de math.h usada solo para verificar */
    double ref = atan(x);
    double ea = fabs(resultado - ref);

    printf("Terminos:        %d\n", m);
    printf("Serie (rad):     %.15g\n", resultado);
    printf("atan(x) ref.:    %.15g\n", ref);
    printf("Termino omitido: %.3e\n", omitido);
    printf("Error absoluto:  %.3e\n", ea);

    /* El error relativo no es informativo si la referencia es cero */
    if (fabs(ref) > 1e-12) {
        printf("Error relativo:  %.3e\n", ea / fabs(ref));
    } else {
        printf("Error relativo:  no informativo (referencia = 0)\n");
    }
    return 0;
}
