<h1 align="center">Hello-ESP</h1>

<p align="center">
  Proyectos con ESP8266 y ESP32 para aprender a programar firmware y diseñar PCB<br>
  <sub>by <a href="https://oxdabit.com"><img src="assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

Repositorio de aprendizaje con proyectos basados en microcontroladores ESP8266 y ESP32. Cada proyecto vive en su propia carpeta dentro de [`projects/`](projects/) y tiene su propio README.

## Proyectos

| Proyecto | Placa | Descripción |
| --- | --- | --- |
| [LED integrado ESP32-S3-N16R8](projects/led-onboard-ESP32-S3-N16R8/) | ESP32-S3-N16R8 | Parpadeo en tres colores del LED RGB integrado (WS2812) con ESP-IDF |
| [Detección de MAC](projects/ESP-Now/src/mac-detection/) | ESP32-S3-N16R8 | Lee la MAC Wi-Fi de la placa, primer paso para comunicar placas con ESP-NOW |

## Entorno de trabajo

Todos los proyectos se programan en C++ con [ESP-IDF](https://docs.espressif.com/projects/esp-idf/), el framework oficial de Espressif, desde VS Code con la extensión **ESP-IDF** (`espressif.esp-idf-extension`). Están probados con ESP-IDF v6.1 y necesitan, como mínimo, la v5.5.

Para trabajar con un proyecto, abre su carpeta en VS Code (no la raíz del repositorio): así la extensión lo detecta como proyecto ESP-IDF. Después, **ESP-IDF: Select Port to Use** para elegir el puerto de la placa y **ESP-IDF: Build, Flash and Monitor** para compilar, grabar y ver la salida. Desde la terminal, lo mismo es:

```sh
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

## LED integrado ESP32-S3-N16R8

El LED de la placa es un **WS2812**, un LED RGB direccionable que recibe el color por un único pin con una señal de tiempos muy precisos. El proyecto la genera con el periférico RMT a través del componente oficial `espressif/led_strip` y repite en bucle 3 parpadeos en verde, 3 en rojo y 1 en azul, con 300 ms encendido y 500 ms apagado.

- **Placa:** ESP32-S3-N16R8.
- **Pin del LED:** GPIO48 por defecto (GPIO38 en la DevKitC-1 v1.1), configurable en `menuconfig` sin tocar el código.
- **Personalizable:** tiempos, brillo y secuencia, al principio de `main/main.cpp`.

[Documentación completa →](projects/led-onboard-ESP32-S3-N16R8/)

## Detección de MAC

ESP-NOW identifica cada placa por su **MAC Wi-Fi en modo estación (STA)**, así que antes de comunicar dos placas hay que conocer la MAC de cada una. Este proyecto la lee de los eFuses del chip, sin arrancar el Wi-Fi, y la imprime cada 5 segundos en el monitor serie, también como array de C listo para copiar en el código. Al arrancar muestra además el chip, la flash y la PSRAM, para comprobar que la configuración coincide con la placa.

- **Placa:** ESP32-S3-N16R8; el código sirve para cualquier ESP32 cambiando el *target*.
- **Ubicación:** forma parte del proyecto ESP-NOW, en `projects/ESP-Now/src/`, y comparte con el resto de programas de esa carpeta la configuración de la placa (`sdkconfig.n16r8`).

[Documentación completa →](projects/ESP-Now/src/mac-detection/)

## Licencia

El contenido de este repositorio está bajo licencia [MIT](LICENSE): puedes usarlo, modificarlo y redistribuirlo manteniendo el aviso de copyright.

Quedan fuera de esa licencia el nombre y el icono de **0xDA_bit** (`assets/oxdabit.png`).

---

<p align="center">
  <a href="https://oxdabit.com"><img src="assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
