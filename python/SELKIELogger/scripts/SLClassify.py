#!/usr/bin/python3
import logging
import msgpack
from ..SLMessages import SLMessageSink

def process_arguments():
    import argparse
    options = argparse.ArgumentParser(description="Read messages from SELKIE Logger data file and classify unknown/raw UBX and NMEA messages found", epilog="Created as part of the SELKIE project")
    options.add_argument("file",  help="Input file name")
    options.add_argument("-v, --verbose", dest="verbose", action="store_true", default=False, help="Include timing and source/channel information messages")
    return options.parse_args()

def SLClassify():
    logging.basicConfig(level=logging.INFO)
    args = process_arguments()
    inFile = open(args.file, "rb")
    unpacker = msgpack.Unpacker(inFile, unicode_errors='ignore')
    out = SLMessageSink(msglogger=logging.getLogger("Messages"))

    # Dict of Dicts gpsSeen[class][msg] = count and similar for NMEA
    gpsSeen = {}
    nmeaSeen = {}
    gpsProcessed = 0
    nmeaProcessed = 0
    for msg in unpacker:
        msg = out.Process(msg, output="raw")
        if msg is None:
            continue

        if msg.SourceID >= 0x10 and msg.SourceID < 0x20:
            # Assume GPS
            if msg.ChannelID != 0x03:
                # We're only looking at raw messages on Ch 3
                if not msg.ChannelID in [0,1,2]:
                    # But we will count messages that have been converted to other types
                    gpsProcessed = gpsProcessed + 1
                continue
            header = int.from_bytes(msg.Data[0:2], byteorder='big', signed=False)
            ubxClass = msg.Data[2]
            ubxMsg = msg.Data[3]

            if ubxClass in gpsSeen:
                if ubxMsg in gpsSeen[ubxClass]:
                    gpsSeen[ubxClass][ubxMsg] = gpsSeen[ubxClass][ubxMsg] + 1
                else:
                    gpsSeen[ubxClass][ubxMsg] = 1
            else:
                gpsSeen[ubxClass] = {}
                gpsSeen[ubxClass][ubxMsg] = 1

        elif msg.SourceID >= 0x30 and msg.SourceID < 0x40:
            # Assume NMEA
            if msg.ChannelID != 0x03:
                if not msg.ChannelID in [0,1,2]:
                    nmeaProcessed = nmeaProcessed + 1
                continue
            nmeaHead = msg.Data[0:10].decode('ascii', 'replace')
            if nmeaHead[1] == "P":
                talker = nmeaHead[1:5]
                message = nmeaHead[5:8]
            else:
                talker = nmeaHead[1:3]
                message = nmeaHead[3:6]

            if talker in nmeaSeen:
                if message in nmeaSeen[talker]:
                    nmeaSeen[talker][message] = nmeaSeen[talker][message] + 1
                else:
                    nmeaSeen[talker][message] = 1
            else:
                nmeaSeen[talker] = {}
                nmeaSeen[talker][message] = 1


    print("===== GPS Messages Seen =====")
    print(f"Converted:\t\t{gpsProcessed:d}")
    print(f"Not Converted:")
    for uC in gpsSeen:
        for uM in gpsSeen[uC]:
            print(f"\t0x{uC:02x}\t0x{uM:02x}\t{gpsSeen[uC][uM]:d}")
    print("\n\n===== NMEA Messages Seen ====")
    print(f"Converted:\t\t{nmeaProcessed:d}")
    print(f"Not Converted:")
    for t in nmeaSeen:
        for m in nmeaSeen[t]:
            print(f"\t{t:3s}\t{m:3s}\t{nmeaSeen[t][m]:d}")
    inFile.close()

if __name__ == "__main__":
    SLDump()
