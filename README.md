# Tres funciones mediante series en C

Autores: Jacob Danioo Pérez Díaz, Iddan Josué Pineda Flores, Kener Armando Espinoza Amador, Emanuel Johamar Valle Amaya.
Grupo 1M3-ELK-S

Aproximación de `sin(x)`, `ln(x)` y `arctan(x)` con series, usando solo ciclos
(`for`, `while`) y condicionales (`if/else`). Las funciones de `math.h`
(`sin`, `log`, `atan`) se usan **únicamente como referencia** para verificar,
nunca para calcular la aproximación.

## Estructura

```
funcs/
├── src/
│   ├── seno.c           # Ejercicio 1 (for)
│   ├── logaritmo.c      # Ejercicio 2 (while + cota de cola)
│   └── arctangente.c    # Ejercicio 3 (if/else + while)
├── build/               # binarios ejecutables (se generan con make)
├── Makefile
└── README.md
```

## Compilación y ejecución

```bash
make          # compila todo en build/ (C11, -Wall -Wextra -Wpedantic, -lm al final)
make test     # ejecuta todas las pruebas de este informe
make clean    # borra build/
```

Comando equivalente para un programa (ejemplo):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/seno.c -o build/seno -lm
./build/seno
```

`make` también crea `logaritmo_max10` y `arctangente_max10`, compilados con
`-DMAX_TERMINOS=10` para probar el aviso de falta de convergencia.

Entradas (separadas por espacio, punto decimal):

| Programa | Entrada | Dominio admitido |
| --- | --- | --- |
| `seno` | `x n` | `\|x\| <= 100` (radianes), `1 <= n <= 100` |
| `logaritmo` | `x eps` | `0.1 <= x <= 10`, `1e-12 <= eps <= 1e-3` |
| `arctangente` | `x eps` | `\|x\| <= 100`, `1e-4 <= eps <= 1e-2` |

Código de salida: `0` correcto, `1` entrada inválida, `2` no se alcanzó la
tolerancia antes del máximo de términos (100000).

Todas las salidas de abajo se obtuvieron con `make test` (gcc 13.3, Ubuntu 24.04).
El error absoluto es `Ea = |S - f_ref|`; el relativo `Er = Ea / |f_ref|` solo se
informa si la referencia no es casi cero.

---

## Ejercicio 1: seno

**Serie.** `sin x = Σ (-1)^k x^(2k+1) / (2k+1)!`, convergente para todo `x` real
(el cociente `x² / ((2k)(2k+1))` tiende a 0).

**Recurrencia.** Dividiendo términos consecutivos:

```
t_k / t_(k-1) = -x² / ((2k)(2k+1))
t_0 = x,   t_k = -t_(k-1) · x² / ((2k)(2k+1)),   k >= 1
```

No se calculan factoriales (se desbordarían). Se usa `for` porque el usuario
indica cuántos términos `n` sumar: el cuerpo corre con `k = 1..n-1` y, junto al
término inicial, suma `n` términos.

**Reducción.** Como `sin` tiene periodo `2π`, se calcula `r = remainder(x, 2π)`
y se evalúa la serie en `r ∈ [-π, π]`. Así los términos no crecen para ángulos
grandes. `π` se obtiene con `acos(-1.0)`.

**Pruebas con 12 términos**

| x | Serie | sin(x) ref. | Ea |
| --- | --- | --- | --- |
| 0 | 0 | 0 | 0 |
| π/6 | 0.5 | 0.5 | 0 |
| −π/2 | −1 | −1 | 2.220e-16 |
| 2π | 0 | −2.449e-16 | 2.449e-16 |

**Efecto del número de términos** (error absoluto)

| x | 3 términos | 6 términos | 12 términos |
| --- | --- | --- | --- |
| π/6 | 2.133e-06 | 3.564e-14 | 0 |
| −π/2 | 4.525e-03 | 5.626e-08 | 2.220e-16 |
| 2π | 2.449e-16 | 2.449e-16 | 2.449e-16 |

Con pocos términos el error de truncamiento domina y crece con `|r|`: para
`π/2` los términos decrecen más despacio que para `π/6`. Con 12 términos el
truncamiento ya es menor que el redondeo (~1e-16) y añadir más no mejora nada.

**Caso 2π.** La serie da exactamente 0 porque `remainder(x, 2π)` es 0 para el
mismo `double`. La referencia `sin(6.283185307179586) = -2.449e-16` no es cero
porque ese `double` es ligeramente distinto del `2π` real. La diferencia viene
de la representación de `π`, no de la serie.

**Argumento grande.** Para `x = 100` el error es 3.331e-15 y no baja al pasar de
12 a 100 términos: no es truncamiento sino error de reducción (`2π` en `double`
multiplicado por 16 vueltas). La biblioteca reduce el argumento con más
precisión que `remainder(x, 2*pi)`.

**Entradas inválidas** (código 1): `101 10` (x fuera de dominio), `1 0` y
`1 101` (n fuera de rango), `hola 5` (no numérica).

**Aplicación:** `v(t) = 10 sin(2π·50·t)` V, con `f = 50 Hz`. El argumento en
radianes es `x = 2π f t`: `f` en Hz (1/s) por `t` en s es adimensional, y `2π`
convierte ciclos a radianes. El periodo es 20 ms.

| t (s) | x = 2π·50·t (rad) | sin x (serie, 12 términos) | v(t) |
| --- | --- | --- | --- |
| 0 | 0 | 0 | 0 V |
| 0.005 | π/2 ≈ 1.5707963267948966 | 1 | 10 V |
| 0.015 | 3π/2 ≈ 4.71238898038469 (reducido a −π/2) | −1 | −10 V |

En `t = 5 ms` la señal está en su máximo (un cuarto de periodo) y en
`t = 15 ms` en su mínimo (tres cuartos de periodo).

---

## Ejercicio 2: logaritmo natural

**Serie.** Con `z = (x-1)/(x+1)` se tiene `x = (1+z)/(1-z)`, por lo que
`ln x = ln((1+z)/(1-z)) = 2·Σ z^(2k+1)/(2k+1)`. Para `x > 0`, `|x-1| < x+1` y
entonces `|z| < 1`: la serie converge (cociente de términos → `z² < 1`).

**Recurrencia del sumando completo** `a_k = 2 z^(2k+1) / (2k+1)`:

```
a_0 = 2z,    a_(k+1) = a_k · z² · (2k+1) / (2k+3)
```

En el código se guarda la potencia `zp = z^(2k+1)` (se multiplica por `z²` en
cada paso) y el factor `2/(2k+1)` se aplica al sumar.

**Cota de cola** tras `M` términos (serie geométrica de razón `z²`):

```
|R_M| <= 2 |z|^(2M+1) / ((2M+1)(1 - z²))
```

El `while` suma mientras `cota > eps` y `M < 100000`. Si sale por el máximo, el
programa avisa y termina con código 2.

**Pruebas con ε = 1e-12**

| x | z | Términos | Ea | Cota |
| --- | --- | --- | --- | --- |
| 0.1 | −9/11 | 61 | 9.104e-13 | 9.384e-13 |
| 0.5 | −1/3 | 12 | 1.054e-13 | 1.062e-13 |
| 1 | 0 | 0 | 0 | 0 |
| 2 | 1/3 | 12 | 1.054e-13 | 1.062e-13 |
| 10 | 9/11 | 61 | 9.099e-13 | 9.384e-13 |

**Efecto de la tolerancia en x = 10**

| eps | Términos | Ea |
| --- | --- | --- |
| 1e-3 | 13 | 8.821e-04 |
| 1e-6 | 29 | 6.959e-07 |
| 1e-12 | 61 | 9.099e-13 |

**Por qué x lejos de 1 requiere más términos.** El error decae como
`|z|^(2M)`. Cerca de `x = 1`, `z ≈ 0` y la serie converge rápido (en `x = 1`
no hace falta ningún término). Para `x = 0.1` o `x = 10`, `|z| = 9/11 ≈ 0.82`:
los términos decrecen lento y hacen falta 61 términos en vez de 12. El número
de términos es aproximadamente `ln(eps) / (2 ln|z|)`.

**Simetría.** `ln(1/x) = −ln x`: `x = 0.5` y `x = 2` (y `0.1` con `10`) dan el
mismo número de términos y errores iguales salvo diferencias de redondeo
(9.104e-13 contra 9.099e-13).

**Casos inválidos** (código 1): `0 1e-6`, `-2 1e-6`, `texto 1e-6`,
`2 1e-13` (eps muy pequeño) y `2 0.01` (eps muy grande).

**Falta de convergencia.** Con `-DMAX_TERMINOS=10` y entrada `10 1e-12` aparece
`No se alcanzo la tolerancia con 10 terminos (cota 4.260e-03).` y el código de
salida es 2.

**Interpretación.** En todas las pruebas `Ea <= cota`, como se espera. Aun así
la cota controla solo el truncamiento: no incluye el redondeo (~1e-16), que aquí
es despreciable frente a 1e-12.

---

## Ejercicio 3: arctangente

**Serie.** `arctan u = Σ (-1)^k u^(2k+1)/(2k+1)` para `|u| <= 1`. Converge para
`|u| < 1` por el cociente. En `|u| = 1` converge por el criterio de Leibniz
(alternante, con términos `1/(2k+1)` que decrecen a 0).

**Recurrencia:**

```
t_0 = u,    t_(k+1) = -t_k · u² · (2k+1) / (2k+3)
```

**Reducción de argumento (if/else).** Si `|x| > 1` se usa
`arctan x = sgn(x)·π/2 − arctan(1/x)`, de modo que la serie siempre se evalúa
con `|u| <= 1`. `x = 0` se trata antes del `if`, antes de cualquier inversión
`1/x`.

**Criterio de parada (while).** Como la serie es alternante con términos
decrecientes en magnitud, el primer término omitido acota el truncamiento. El
ciclo suma mientras `|t| > eps` y `M < 100000`.

**Pruebas con ε = 1e-4** (resultado en radianes)

| x | u usado | Términos | Serie | Ea |
| --- | --- | --- | --- | --- |
| 0 | — | 0 | 0 | 0 |
| 0.5 | 0.5 | 5 | 0.463684275793651 | 3.667e-05 |
| 1 | 1 | 5000 | 0.785348163397947 | 5.000e-05 |
| −1 | −1 | 5000 | −0.785348163397947 | 5.000e-05 |
| 2 | 0.5 | 5 | 1.10711205100125 | 3.667e-05 |

**Número de términos en x = 1**

| eps | Términos | Término omitido | Ea |
| --- | --- | --- | --- |
| 1e-2 | 50 | 9.901e-03 | 5.000e-03 |
| 1e-3 | 500 | 9.990e-04 | 5.000e-04 |
| 1e-4 | 5000 | 9.999e-05 | 5.000e-05 |

**Por qué es lenta en |x| = 1.** El cociente entre términos consecutivos es
`(2k+1)/(2k+3) → 1`: los términos decrecen como `1/(2k+1)` (comportamiento tipo
serie armónica), no como una geométrica. El término omitido es ≈ `1/(2M+1)`, así
que `M ≈ 1/(2·eps)` y cada década de tolerancia cuesta 10 veces más términos.
En cambio, para `|u| = 0.5` bastan 5 términos. El error es casi la mitad del
término omitido, propio de las series alternantes.

**Efecto de la reducción.** `x = 2` usa `u = 0.5` (5 términos) y da el mismo
`Ea` que `x = 0.5`: el error de la serie se traslada igual a `π/2 − arctan(1/x)`.
Con `x = 100` basta 1 término (`u = 0.01`) y `Ea = 3.333e-07`.

**Simetría impar.** `arctan(−x) = −arctan(x)`: los resultados para `−0.5`, `−1`,
`−2` y `−100` son idénticos a los de `0.5`, `1`, `2` y `100` con signo opuesto
(mismos términos y mismo `Ea`).

**Casos inválidos** (código 1): `101 1e-3`, `1 1e-5`, `1 0.1`, `texto 1e-3`.

**Falta de convergencia.** Con `-DMAX_TERMINOS=10` y entrada `1 1e-4`:
`No se alcanzo la tolerancia con 10 terminos (termino omitido 4.762e-02).` y
código de salida 2.

---

## Limitaciones

* `double` guarda unos 15–16 dígitos: no se puede pedir una exactitud mayor, y
  por eso `eps` tiene límites inferiores.
* El criterio de parada controla el **truncamiento**, no el redondeo ni el
  error de reducción del argumento (visible en `sin(100)` y `sin(2π)`).
* Los límites de dominio son didácticos, no matemáticos: las tres series
  convergen en dominios mayores (la de `ln x` para todo `x > 0`).
* Errores relativos cerca de cero (como `sin 2π` o `ln 1`) no son informativos:
  allí se usa el error absoluto.
