# Copyright (C) 2023 Swansea University
#
# This file is part of the SELKIELogger suite of tools.
#
# SELKIELogger is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# SELKIELogger is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along
# with this SELKIELogger product.
# If not, see <http://www.gnu.org/licenses/>.

from dataclasses import dataclass
from typing import Optional, List, Dict

# Define some aliases here that could be specialised in the future

FilePath = str
FilePrefix = str
DevicePath = str
SourceType = str
MQTTTopic = str


@dataclass
class CoreOptions:
    prefix: str
    rotate: bool = True
    statefile: Optional[FilePath] = None
    savestate: bool = True
    verbose: int = 0
    frequency: int = 10


# Should not be instantiated directly
@dataclass
class _Source:
    tag: str
    name: str


@dataclass
class GPSSource(_Source):
    port: DevicePath
    sourcetype: SourceType = "GPS"
    sourcenum: Optional[int] = None
    baud: int = 115200
    initialbaud: int = 9600
    dumpall: bool = False


@dataclass
class SLSource(_Source):
    port: DevicePath
    sourcetype: SourceType = "SL"
    sourcenum: Optional[int] = None
    baud: int = 115200


@dataclass
class I2CSource(_Source):
    bus: DevicePath
    frequency: int = 5
    sourcetype: SourceType = "I2C"
    sourcenum: Optional[int] = None
    ads1015: Optional[List[str]] = None
    ina219: Optional[List[str]] = None


@dataclass
class NMEASource(_Source):
    port: DevicePath
    sourcetype: SourceType = "NMEA"
    sourcenum: Optional[int] = None
    baud: int = 115200


@dataclass
class N2KSource(_Source):
    port: DevicePath
    sourcetype: SourceType = "N2K"
    sourcenum: Optional[int] = None
    baud: int = 115200


@dataclass
class DatawellSource(_Source):
    host: str
    sourcetype: SourceType = "DW"
    sourcenum: Optional[int] = None
    timeout: int = 3600
    raw: bool = True
    spectrum: bool = False


@dataclass
class LPMSSource(_Source):
    port: DevicePath
    sourcetype: SourceType = "LPMS"
    sourcenum: Optional[int] = None
    baud: int = 921600
    unitID: int = 1
    frequency: int = 10


@dataclass
class MQTTSource(_Source):
    host: str
    topic: List[MQTTTopic]
    sourcetype: SourceType = "MQTT"
    sourcenum: Optional[int] = None
    port: int = 1883
    dumpall: bool = False


@dataclass
class VictronSource(_Source):
    host: str
    sysid: str
    topic: List[MQTTTopic]
    sourcetype: SourceType = "VICTRON"
    sourcenum: Optional[int] = None
    port: int = 1883
    dumpall: bool = False
    victron_keepalives: bool = False
    keepalive_interval: int = 20


@dataclass
class TimerSource(_Source):
    frequency: int
    sourcetype: SourceType = "VICTRON"
    sourcenum: Optional[int] = None


@dataclass
class SerialSource(_Source):
    port: DevicePath
    sourcetype: SourceType = "SERIAL"
    sourcenum: Optional[int] = None
    baud: int = 115200
    minbytes: int = 10
    maxbytes: int = 1024


@dataclass
class NetworkSource(_Source):
    host: str
    port: int
    sourcetype: SourceType = "TCP"
    sourcenum: Optional[int] = None
    baud: int = 115200
    minbytes: int = 10
    maxbytes: int = 1024
    timeout: int = 60


@dataclass
class UnknownSource(_Source):
    sourcetype: SourceType
    sourcenum: Optional[int] = None
    options: Optional[Dict] = None
