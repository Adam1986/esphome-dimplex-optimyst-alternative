# esphome-dimplex-optimyst-alternative

This was forked from this fantastic repository https://github.com/victorgolfecho/esphome-dimplex-optimyst to serve as an additional solution for other Dimplex Fireplace Units/Remote Controls.

I updated this code to replicate another remote control supplied with Dimplex Opti-Myst Fires (Pictured).
The remote buttons do slightly different BLE Advertisements compared to the original repo.

ESPHome external component that turns an ESP32 into a virtual Dimplex
Opti-Myst remote.  Bluetooth-advertises the same packets the physical
6-button remote sends, so the cassette can't tell the difference.

Confirmed working with the **Penngrove/PNN20** Inset Electric Fire.

Not affiliated with Dimplex. Use at your own risk.

## Usage

Add to any ESPHome config:

```yaml
external_components:
  - source: github://Adam1986/esphome-dimplex-optimyst-alternative@main
    components: [dimplex_optimyst]

esp32_ble:
  io_capability: none

dimplex_optimyst:
  - id: cassette

button:
  - platform: dimplex_optimyst
    dimplex_id: cassette
    name: "Fireplace Power"
    cmd: POWER
  - platform: dimplex_optimyst
    dimplex_id: cassette
    name: "Sync to Cassette"
    cmd: SOUND
    duration: 25s
```

That's it. The component handles the raw BLE advertising; the button
platform exposes a press that triggers it. See
[`example.yaml`](example.yaml) for a full ready-to-flash config.

### Configuration

```yaml
dimplex_optimyst:
  - id: <required, used by button.dimplex_id>
```

The component has no further options today. It uses the ESP32's public
MAC for all broadcasts.

### Button platform

```yaml
button:
  - platform: dimplex_optimyst
    dimplex_id: <id of a dimplex_optimyst entry>
    name: <button name>
    cmd: <see commands table below>
    duration: 250ms  # optional, default 250ms
```

Available `cmd` values:

| `cmd`       | Button on the physical remote |
|-------------|--------------------------------|
| `POWER`     | Power toggle |
| `SOUND`     | Sound on/off (also used during pairing) |
| `HEAT_ON`   | Turn on heater accessory |
| `HEAT_OFF`  | Turn off heater accessory |
| `MIST_UP`   | Bigger mist / brighter flame |
| `MIST_DOWN` | Smaller mist / dimmer flame |

For pairing, expose a button with `cmd: SOUND` and `duration: 25s`. Press
the cassette's Bluetooth-icon button (lights blink every 2.5 s), then
press the long-Sound button in ESPHome's web UI within 20 seconds. The
cassette beeps 4 times when it's accepted you as a remote.

## Protocol

Each press emits an `ADV_NONCONN_IND` packet whose manufacturer-specific
AD field is exactly 10 bytes:

```
4F 70   74 69 6D 79 73 74   <cmd>   08
"O p"   "t i m y s t"
↑       ↑                   ↑       ↑
└─ Company ID 0x704F (LE)   │       └─ trailer (always 0x08)
   placed by the BLE        │
   manuf-data AD field;     └─ command byte
   the rest is data
```

The cassette keeps a whitelist of remote MAC addresses, populated via its
on-board sync mode. While in sync mode the cassette advertises
`Ooptimyst01<MAC>` under company ID `0x4345` — handy to confirm sync mode
is active from any BLE scanner.

One cassette accepts up to 5 remotes; one remote can talk to up to 5
cassettes. Because this component currently uses the ESP32's public MAC
for every broadcast, all cassettes synced to it will react to every
button press in lockstep (i.e. behaves as an "all on / all off" master).
For independent per-cassette control, switch the address per command —
not yet implemented in this component.

## Hardware tested

- Cassette: Dimplex CDFI500-PRO (`00B550`), CDFI1000-PRO (`00B510`).
- Tethered controller: BLE name prefix `FI` (e.g. `FI3919<Dimplex>`).
- 6-button Opti-Myst Pro remote (power / sound / heat on / heat off /
  mist up / mist down).
- Firmware H:7.0 / S:6.1.

If your cassette is a newer Flame Connect-app variant which uses BLE GATT
(not raw advertisements), this component will not work as-is. The
[community thread on Home Assistant discussions][ha977] has notes on
related models.

## ⚠️ Important: ESP32 chip revision matters

**Use an ESP32 with revision v1.0 silicon.** ESP32-D0WD revision v3.0
(also marketed as "ESP32-ECO3") boards have a bug in this BLE
advertisement code path: `esp_ble_gap_start_advertising` returns success
and the chip transmits scan requests fine, but advertisement packets
either never hit the air or transmit at near-zero power. Cassettes don't
see them. Verified across two different V3.0 modules from the same batch;
both fail identically while V1.0 modules nearby broadcast strongly.

**How to check what you have**, plug the board into USB and run:

```bash
esptool.py --port /dev/ttyUSB0 chip_id
```

You're looking for:

- ✅ `ESP32-D0WDQ6 (revision v1.0)` — works
- ✅ `ESP32-D0WD (revision v1.0)` — works
- ❌ `ESP32-D0WD-V3 (revision v3.0)` — does NOT reliably broadcast

**Sourcing V1.0 silicon** is harder than it should be in 2026 — Espressif
has shifted production to V3.0, and generic listings (e.g., Amazon
"ESP-WROOM-32" / "ESP32 CH340C TYPE-C" boards) almost always ship V3.0
now. The boards look physically identical; the difference is only on the
silkscreen on the chip itself.

To get V1.0 reliably:

- Buy through **DigiKey/Mouser** with explicit part numbers
  `ESP32-WROOM-32D` or `ESP32-D0WDQ6`.
- Or **buy in bulk** from cheap sources and screen with `esptool.py` on
  receipt; expect ~50% V3.0 to discard.

Alternatives worth trying if you want newer silicon and can iterate:

- **ESP32-S3** modules — different chip family entirely, untested with
  this component but may sidestep the V3.0 BLE bug. Would also need
  framework adjustments (the YAML's `board: esp32dev` would change).

If your boards aren't broadcasting and `esptool` reports V3.0, the
component code isn't the problem — it's the silicon.

## Acknowledgements

The `pOptimyst` magic prefix and several command bytes were originally
documented by the Home Assistant community
([discussion #977][ha977]) and an [earlier ESPHome gist][gist]. This
repository verifies them against the CDFI-PRO line, fills in the sync
button (the cassette's Sound icon — silent in the manual due to a lost
glyph), and packages everything as a drop-in ESPHome external component.

[ha977]: https://github.com/orgs/home-assistant/discussions/977
[gist]: https://gist.github.com/gjongenelen/62d80815bd518841db55f64bb7ed4ec2
