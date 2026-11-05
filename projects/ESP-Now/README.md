<h1 align="center">ESP-NOW</h1>

<p align="center">
  Comunicación directa entre dos placas ESP32-S3, sin router ni red Wi-Fi<br>
  <sub>by <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

**ESP-NOW** es un protocolo de Espressif que envía paquetes cortos (hasta 250 bytes en su primera versión) directamente de una placa a otra usando la radio Wi-Fi. No necesita router, ni conectarse a una red, ni direcciones IP: cada placa se identifica por su **MAC Wi-Fi en modo estación (STA)**.

En este proyecto, una placa envía un mensaje cada 5 segundos y la otra lo señala con el LED integrado. Cada placa lleva su propio programa:

| Programa | Placa | MAC Wi-Fi STA | Qué hace |
| --- | --- | --- | --- |
| [Emisor](src/emitter/) | ESP 01 | `14:C1:9F:26:D1:38` | Cada 5 segundos envía a la ESP 02 un mensaje con un número de secuencia y anota en el monitor si el receptor confirmó la recepción. |
| [Receptor](src/receiver/) | ESP 02 | `E8:3D:C1:F2:AC:3C` | Acepta solo los mensajes de la ESP 01 y, por cada uno, hace parpadear el LED RGB integrado una vez en rojo, otra en verde y otra en azul. |

## Hardware

- Dos placas **ESP32-S3-N16R8** (16 MB de flash y 8 MB de PSRAM octal).
- Dos cables USB-C de datos.

## Estructura

```text
ESP-Now/
└── src/
    ├── sdkconfig.n16r8   configuración de la placa, compartida por los dos programas
    ├── emitter/          emisor (placa ESP 01)
    └── receiver/         receptor (placa ESP 02)
```

Cada programa es un proyecto ESP-IDF independiente: se abre, compila y graba por separado.

## Puesta en marcha

1. **Averigua la MAC de cada placa** con el proyecto [Detección de MAC](../mac-detection/) y etiqueta las placas para no confundirlas.
2. **Pon las MAC en el código** si tus placas no son las de la tabla: `MAC_RECEPTOR` en `src/emitter/main/emitter.cpp` y `MAC_EMISOR` en `src/receiver/main/receiver.cpp`.
3. **Graba el receptor** en la placa ESP 02 y abre su monitor.
4. **Graba el emisor** en la placa ESP 01 y abre su monitor en otro terminal.

Con las dos placas conectadas al mismo ordenador, cada una tiene su puerto (`/dev/cu.usbmodem…`). Comprueba cuál es cuál antes de grabar. Para salir de cada monitor, `Ctrl+]`, o `Ctrl+T` y después `Ctrl+X`.

Si todo va bien, cada 5 segundos el emisor muestra «Enviado mensaje #N» y «Entregado a …», el receptor muestra «Recibido mensaje #N» y el LED parpadea en rojo, verde y azul.

## Requisitos comunes

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) v5.5 o superior (probado con v6.1).
- Los dos programas usan el **canal Wi-Fi 1**. Si cambias `CANAL_WIFI` en uno, cámbialo también en el otro.
- El formato del mensaje (`struct Mensaje`) está definido en los dos archivos y tiene que ser idéntico.

---

<p align="center">
  <a href="https://oxdabit.com"><img src="../../assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
