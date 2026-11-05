<h1 align="center">Detección de MAC</h1>

<p align="center">
  Lee la dirección MAC Wi-Fi de una placa ESP32, la que usa ESP-NOW para identificar cada nodo<br>
  <sub>by <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

ESP-NOW no usa direcciones IP ni nombres: cada placa se identifica por su **MAC Wi-Fi en modo estación (STA)**. Antes de enviar nada de una placa a otra hay que conocer la MAC del destino, y este programa es el primer paso: la lee y la imprime en el monitor serie.

Es un proyecto independiente: se usa para preparar el proyecto [ESP-NOW](../ESP-Now/), pero no depende de él y sirve para cualquier otro que necesite la MAC de una placa.

La MAC STA se calcula a partir de la MAC base que Espressif graba de fábrica en los eFuses del chip, así que se puede leer sin arrancar el Wi-Fi.

Al arrancar, el programa muestra además los datos de la placa (chip, revisión, núcleos, flash y PSRAM). Sirven para comprobar que la configuración de memoria coincide con el hardware. Después repite la MAC cada 5 segundos, para que no te la pierdas si abres el monitor tarde.

## Hardware

- Placa **ESP32-S3-N16R8** (16 MB de flash y 8 MB de PSRAM octal).
- Cable USB-C de datos.

El código funciona en cualquier chip de la familia ESP32. Para otra placa, cambia el *target* con `idf.py set-target <chip>` y ajusta o borra `sdkconfig.defaults`, que tiene la configuración de memoria de la N16R8.

## Estructura

```text
mac-detection/
├── CMakeLists.txt          proyecto ESP-IDF
├── sdkconfig.defaults      configuración de la placa (chip, flash y PSRAM)
└── main/
    ├── CMakeLists.txt      código y componentes que usa
    └── main.cpp            el programa
```

La configuración de la placa está en [`sdkconfig.defaults`](sdkconfig.defaults). Solo se aplica al generar el `sdkconfig`: si lo cambias, haz un **Full Clean** para que se vuelva a aplicar.

## Compilar y grabar

Necesitas [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) v5.5 o superior (probado con v6.1).

Desde VS Code con la extensión ESP-IDF: abre esta carpeta, elige el puerto con **ESP-IDF: Select Port to Use** y ejecuta **ESP-IDF: Build, Flash and Monitor**.

Desde la terminal:

```sh
idf.py set-target esp32s3
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

El puerto aparece como `/dev/cu.usbmodem…` si conectas el USB nativo de la placa, y como `/dev/cu.usbserial-…` o `/dev/cu.wchusbserial-…` si usas el USB con conversor serie. Para salir del monitor, `Ctrl+]`.

## Salida esperada

```text
I (310) mac-detection: Chip: esp32s3 (revisión v0.2, 2 núcleos)
I (310) mac-detection: Flash: 16 MB | PSRAM disponible: 8192 KB
I (315) mac-detection: MAC Wi-Fi STA (la que usa ESP-NOW): 14:c1:9f:26:d1:38
I (320) mac-detection: Para el código: {0x14, 0xC1, 0x9F, 0x26, 0xD1, 0x38}
```

La última línea está en formato de array de C, lista para copiarla en el código que envíe datos a esa placa. Si tienes varias placas, anota la MAC de cada una y etiquétalas.

## Si algo falla

| Síntoma | Causa probable |
| --- | --- |
| No aparece ningún puerto nuevo al conectar la placa | Cable de solo carga, o prueba el otro USB-C. |
| «Failed to connect» al grabar | Pon la placa en modo descarga: mantén BOOT, pulsa y suelta RST, suelta BOOT. |
| `Flash` o `PSRAM` no coinciden con tu placa | Revisa `sdkconfig.defaults` y haz un **Full Clean** para que se vuelva a aplicar. |
| El monitor no muestra nada | Pulsa RST con el monitor abierto. |

---

<p align="center">
  <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
