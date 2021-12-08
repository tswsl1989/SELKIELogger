#ifndef SELKIELoggerBase_Sources
#define SELKIELoggerBase_Sources

/*!
 * @file sources.h Predefined message source ID information.
 * @ingroup SELKIELoggerBase
 * @sa loggerSources
 * @sa loggerChannels
 */

/*!
 * @addtogroup loggerSources Logger Source IDs
 * @ingroup MPMessages
 *
 * The protocol used for data transmission requires each message to be tagged
 * with a source ID, that should be unique on any given system
 *
 * Source IDs range from 0 to 127, but are best represented as two digit
 * hexadecimal numbers, with the first number indicating a device or sensor
 * type and the second digit free to be used as a local device ID. In current
 * implementations these IDs are baked into device software or firmware, but
 * may be configurable for future devices.
 *
 * IDs in the 0x00 - 0x0F range are an exception, and each value represents a
 * different software source type.
 *
 * Any ID number or range not allocated here is considered reserved, although
 * software should still process data with unknown source ID numbers by
 * default.
 *
 * @{
 */

#define SLSOURCE_LOCAL	0x00 //!< Messages generated by the logging software
#define SLSOURCE_CONV	0x01 //!< Messages generated by data conversion tools
#define SLSOURCE_TIMER	0x02 //!< Local/Software timers

#define SLSOURCE_TEST1	0x05 //!< Test data source ID (1/3)
#define SLSOURCE_TEST2	0x06 //!< Test data source ID (2/3)
#define SLSOURCE_TEST3	0x07 //!< Test data source ID (3/3)

#define SLSOURCE_GPS	0x10 //!< GPS (or other satellite navigation) sources

#define SLSOURCE_ADC	0x20 //!< Generic analogue inputs

#define SLSOURCE_NMEA	0x30 //!< NMEA Bus

#define SLSOURCE_I2C	0x40 //!< I2C Bus

#define SLSOURCE_EXT	0x60 //!< External data sources recorded but not interpreted by the logging software

#define SLSOURCE_MP	0x70 //!< Devices with suitable MessagePack output returning single value channels
#define SLSOURCE_IMU	SLSOURCE_MP //!< IMUs (e.g. SURFTEC IMU devices). Alias for SLSOURCE_MP

//! @}

/*!
 * @addtogroup loggerChannels Logger Channel IDs
 * @ingroup MPMessages
 *
 * The protocol used for data transmission requires each message to be tagged
 * with a channel and source ID. While source IDs are required to be unique on
 * a given system, some channel numbers are common to all sources.
 *
 * As with source IDs, channel IDs range between 0 and 127. The mandatory
 * channels are described here, with the remaining numbers available for use as
 * required by any given source.
 *
 * The three log message types would not normally be included in a sources
 * channel map, but are available for use by any device.
 *
 * @{
 */

#define SLCHAN_NAME	0x00 //!< Name of source device
#define SLCHAN_MAP	0x01 //!< Channel name map (excludes log channels)
#define SLCHAN_TSTAMP	0x02 //!< Source timestamp (milliseconds, arbitrary epoch)
#define SLCHAN_RAW	0x03 //!< Raw device data (Not mandatory)
#define SLCHAN_LOG_INFO	0x7D //!< Information messages
#define SLCHAN_LOG_WARN	0x7E //!< Warning messages
#define SLCHAN_LOG_ERR	0x7F //!< Error messages

//! @}
#endif
