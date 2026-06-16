"""Dimplex Opti-Myst Pro external component for ESPHome.

Turns the ESP32 into a virtual 6-button remote for the Dimplex Opti-Myst Pro
cassette family (CDFI500-PRO / CDFI1000-PRO).
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@victorgolfecho"]
DEPENDENCIES = ["esp32_ble"]
MULTI_CONF = True

dimplex_optimyst_ns = cg.esphome_ns.namespace("dimplex_optimyst")
DimplexOptimyst = dimplex_optimyst_ns.class_("DimplexOptimyst", cg.Component)

# Command bytes captured from a physical 6-button remote. Same on-air format
# for all six: 4F 70 74 69 6D 79 73 74 <cmd> 05 (ASCII "Optimyst" prefix).
DIMPLEX_COMMANDS = {
    "POWER": 0x29,
    "HEATER_OFF_LOW_HIGH": 0x28,
    "TEMP_UP": 0x04,
    "HEAT_DOWN": 0x08,
    "MIST_UP": 0x55,
    "MIST_DOWN": 0x44,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DimplexOptimyst),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
