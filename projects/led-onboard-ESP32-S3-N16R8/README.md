<h1 align="center">LED integrado ESP32-S3-N16R8</h1>

<p align="center">
  Parpadeo en tres colores del LED RGB integrado de la ESP32-S3-N16R8 con ESP-IDF<br>
  <sub>by <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

El LED de la placa no es un LED normal: es un **WS2812**, un LED RGB direccionable que recibe el color por un único pin de datos con un protocolo de tiempos muy estrictos. No basta con poner el pin a nivel alto; hay que generar esa señal, y en ESP-IDF eso lo hace el periférico **RMT** a través del componente oficial [`espressif/led_strip`](https://components.espressif.com/components/espressif/led_strip).

El programa repite en bucle esta secuencia:

| Color | Parpadeos |
| --- | --- |
| Verde | 3 |
| Rojo | 3 |
| Azul | 1 |

Cada parpadeo deja el LED **300 ms encendido** y **500 ms apagado**.

## Hardware

- Placa **ESP32-S3-N16R8** (16 MB de flash y 8 MB de PSRAM octal).
- Cable USB-C de datos.

El pin del LED cambia según la revisión de la placa: **GPIO48** en la ESP32-S3-DevKitC-1 v1.0 y en la mayoría de clones, y **GPIO38** en la v1.1. El proyecto usa el 48 por defecto y se puede cambiar sin tocar el código.

## Estructura

```text
led-onboard-ESP32-S3-N16R8/
├── CMakeLists.txt          proyecto ESP-IDF
├── sdkconfig.defaults      flash de 16 MB y PSRAM octal de la N16R8
└── main/
    ├── CMakeLists.txt      código y componentes que usa
    ├── idf_component.yml   dependencia de espressif/led_strip
    ├── Kconfig.projbuild   opción de menuconfig para el pin del LED
    └── main.cpp            el programa
```

## Compilar y grabar

Necesitas [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) v5.5 o superior (probado con v6.1). La primera compilación descarga `led_strip` del registro de componentes de Espressif, así que hace falta conexión a internet.

Desde VS Code con la extensión ESP-IDF: abre esta carpeta, elige el puerto con **ESP-IDF: Select Port to Use** y ejecuta **ESP-IDF: Build, Flash and Monitor**.

Desde la terminal:

```sh
idf.py set-target esp32s3
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

En el monitor verás qué color toca en cada momento:

```text
I (320) led-onboard: LED RGB en GPIO48
I (320) led-onboard: 3 x verde
I (2720) led-onboard: 3 x rojo
I (5120) led-onboard: 1 x azul
```

## Personalizar

- **Pin del LED:** `idf.py menuconfig` → *Ejemplo LED integrado* → *GPIO del LED RGB (WS2812)*.
- **Tiempos y brillo:** constantes `T_ENCENDIDO_MS`, `T_APAGADO_MS` y `BRILLO` al principio de `main/main.cpp`. El brillo va de 0 a 255; está en 32 porque a plena potencia el WS2812 deslumbra.
- **Secuencia:** array `SECUENCIA` en `main/main.cpp`, con un color y un número de parpadeos por paso.

## Si el LED no se enciende

| Síntoma | Causa probable |
| --- | --- |
| El monitor muestra la secuencia pero el LED no luce | La placa usa otro pin: prueba el GPIO38 en menuconfig. |
| Los colores salen cambiados (rojo en lugar de verde) | El LED no es GRB: cambia `LED_STRIP_COLOR_COMPONENT_FMT_GRB` por `..._FMT_RGB` en `crear_led()`. |
| Reinicios en bucle al arrancar | La placa no es N16R8: revisa `sdkconfig.defaults`. |

---

<p align="center">
  <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
