"""Button platform for the Dimplex Opti-Myst component."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID

from .. import (
    DIMPLEX_COMMANDS,
    DimplexOptimyst,
    dimplex_optimyst_ns,
)

CONF_DIMPLEX_ID = "dimplex_id"
CONF_CMD = "cmd"
CONF_DURATION = "duration"

DimplexButton = dimplex_optimyst_ns.class_(
    "DimplexButton", button.Button, cg.Component
)

CONFIG_SCHEMA = (
    button.button_schema(DimplexButton)
    .extend(
        {
            cv.GenerateID(CONF_DIMPLEX_ID): cv.use_id(DimplexOptimyst),
            cv.Required(CONF_CMD): cv.one_of(*DIMPLEX_COMMANDS, upper=True),
            cv.Optional(
                CONF_DURATION, default="250ms"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_DIMPLEX_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_cmd(DIMPLEX_COMMANDS[config[CONF_CMD]]))
    cg.add(var.set_duration_ms(config[CONF_DURATION].total_milliseconds))
