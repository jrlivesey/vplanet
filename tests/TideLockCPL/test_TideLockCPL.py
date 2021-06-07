from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.d.RotPer": {"value": 44.658583, "unit": u.days},
        "log.final.d.Obliquity": {"value": 0.328813, "unit": u.rad},
        "log.final.d.Pericenter": {"value": 3.49629559665e+10, "unit": u.m},
    }
)
class TestTideLockCPL(Benchmark):
    pass
