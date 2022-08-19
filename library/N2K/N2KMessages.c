#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "N2KMessages.h"
#include "N2KTypes.h"

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Unsigned byte
 */
uint8_t n2k_get_uint8(const n2k_act_message *n, size_t offset) {
	return n->data[offset];
}

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Signed byte
 */
int8_t n2k_get_int8(const n2k_act_message *n, size_t offset) {
	uint8_t u = n2k_get_uint8(n, offset);
	int8_t v = (u & 0x7F);
	if (u & 0x80) { v = -1 * ((1 << 7) - v); }
	return v;
}

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Signed 16-bit integer
 */
int16_t n2k_get_int16(const n2k_act_message *n, size_t offset) {
	uint16_t u = n2k_get_uint16(n, offset);
	int16_t v = (u & 0x7FFF);
	if (u & 0x8000) { v = -1 * ((1 << 15) - v); }
	return v;
}

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Unsigned 16-bit integer
 */
uint16_t n2k_get_uint16(const n2k_act_message *n, size_t offset) {
	return n->data[offset] + (n->data[offset + 1] << 8);
}

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Signed 32-bit integer
 */
int32_t n2k_get_int32(const n2k_act_message *n, size_t offset) {
	uint32_t u = n2k_get_uint32(n, offset);
	int32_t v = (u & 0x7FFFFFFFUL);
	if (u & 0x80000000UL) { v = -1 * ((1UL << 31) - v); }
	return v;
}

/*!
 * @param[in] n N2K message containing target daat
 * @param[in] offset Starting offset within n2k_act_message.data
 * @returns Unsigned 32-bit integer
 */
uint32_t n2k_get_uint32(const n2k_act_message *n, size_t offset) {
	uint32_t v = n->data[offset] + (uint32_t)(n->data[offset + 1] << 8) + (uint32_t)(n->data[offset + 2] << 16) +
	             (uint32_t)(n->data[offset + 3] << 24);
	return v;
}
/*!
 * @param[in] n Input message
 * @param[out] id ISO Identity
 * @param[out] mfr Manufacturer code
 * @param[out] inst Device Instance
 * @param[out] fn Device Function
 * @param[out] class Device Class
 * @param[out] sys System/Class Instance
 * @param[out] ind Industry
 * @param[out] cfg "Self-Configurable"
 * @returns True on success, false on error
 */
bool n2k_60928_values(const n2k_act_message *n, uint32_t *id, uint16_t *mfr, uint8_t *inst, uint8_t *fn, uint8_t *class,
                      uint8_t *sys, uint8_t *ind, bool *cfg) {
	if (!n || n->PGN != 60928 || !n->data || n->datalen < 8) { return false; }
	bool success = true;
	if (id) { (*id) = n2k_get_uint32(n, 0) & 0x1FFFFF; }
	if (mfr) { (*mfr) = n2k_get_uint16(n, 2) >> 5; }
	if (inst) { (*inst) = n2k_get_uint8(n, 4); }
	if (fn) { (*fn) = n2k_get_uint8(n, 5); }
	if (class) { (*class) = (n2k_get_uint8(n, 6) & 0xFE) >> 1; } // Discard single reserved bit
	uint8_t si = n2k_get_uint8(n, 7);
	if (cfg) { (*cfg) = (si & 0x80) == 0x80; }
	if (ind) { (*ind) = (si & 0x70) >> 4; }
	if (sys) { (*sys) = (si & 0x0F); }
	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] hdg Device heading, in degrees
 * @param[out] dev Heading deviation?, in degrees
 * @param[out] var Heading variation, in degrees
 * @param[out] ref True (0) or Magnetic (1) heading
 * @returns True on success, false on error
 */
bool n2k_127250_values(const n2k_act_message *n, uint8_t *seq, double *hdg, double *dev, double *var, uint8_t *ref) {
	if (!n || n->PGN != 127250 || !n->data || n->datalen < 8) { return false; }

	if (seq) { (*seq) = n2k_get_uint8(n, 0); }

	bool success = true;
	if (hdg) {
		uint16_t h = n2k_get_uint16(n, 1);
		if (h == UINT16_MAX) {
			(*hdg) = NAN;
			success = false;
		} else {
			(*hdg) = h * N2K_TO_DEGREES;
		}
	}

	if (dev) {
		int16_t d = n2k_get_int16(n, 3);
		if (d == INT16_MAX) {
			(*dev) = NAN;
			success = false;
		} else {
			(*dev) = d * N2K_TO_DEGREES;
		}
	}
	if (var) {
		int16_t v = n2k_get_int16(n, 5);
		if (v == INT16_MAX) {
			(*var) = NAN;
			success = false;
		} else {
			(*var) = v * N2K_TO_DEGREES;
		}
	}

	if (ref) { (*ref) = n2k_get_uint8(n, 7) & 0x03; }

	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] rate Rate of turn (units TBC)
 * @returns True on success, false on error
 */
bool n2k_127251_values(const n2k_act_message *n, uint8_t *seq, double *rate) {
	if (!n || n->PGN != 127251 || !n->data || n->datalen < 8) { return false; }

	if (seq) { (*seq) = n2k_get_uint8(n, 0); }

	if (rate) {
		int16_t r = n2k_get_int16(n, 1);
		if (r == INT16_MAX) {
			(*rate) = NAN;
			return false;
		} else {
			(*rate) = r * N2K_TO_DEGREES;
		}
	}

	return true;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] yaw Device yaw, in degrees
 * @param[out] pitch Device pitch, in degrees
 * @param[out] roll Device roll, in degrees
 * @returns True on success, false on error
 */
bool n2k_127257_values(const n2k_act_message *n, uint8_t *seq, double *yaw, double *pitch, double *roll) {
	if (!n || n->PGN != 127257 || !n->data || n->datalen < 7) { return false; }
	if (seq) { *seq = n2k_get_uint8(n, 0); }
	bool success = true;
	if (yaw) {
		int16_t y = n2k_get_int16(n, 1);
		if (y == INT16_MAX) {
			(*yaw) = NAN;
			success = false;
		} else {
			(*yaw) = y * N2K_TO_DEGREES;
		}
	}
	if (pitch) {
		int16_t p = n2k_get_int16(n, 3);
		if (p == INT16_MAX) {
			(*pitch) = NAN;
			success = false;
		} else {
			(*pitch) = p * N2K_TO_DEGREES;
		}
	}
	if (roll) {
		int16_t r = n2k_get_int16(n, 5);
		if (r == INT16_MAX) {
			(*roll) = NAN;
			success = false;
		} else {
			(*roll) = r * N2K_TO_DEGREES;
		}
	}
	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] depth Depth below sensor
 * @param[out] offset Distance from sensor to reference surface
 * @param[out] range Measurement range
 * @returns True on success, false on error
 */
bool n2k_128267_values(const n2k_act_message *n, uint8_t *seq, double *depth, double *offset, double *range) {
	if (!n || n->PGN != 128267 || !n->data || n->datalen < 8) { return false; }

	bool success = true;
	if (seq) { *seq = n2k_get_uint8(n, 0); }

	if (depth) {
		uint32_t d = n2k_get_uint32(n, 1);
		if (d == UINT32_MAX) {
			(*depth) = NAN;
			success = false;
		} else {
			(*depth) = d * 0.01;
		}
	}

	if (offset) {
		int16_t o = n2k_get_int16(n, 5);
		if (o == INT16_MAX) {
			(*offset) = NAN;
			success = false;
		} else {
			(*offset) = o * 0.01;
		}
	}
	if (range) {
		int8_t r = n2k_get_int8(n, 7);
		if (r == INT8_MAX) {
			(*range) = NAN;
			success = false;
		} else {
			(*range) = 10.0 * r;
		}
	}

	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] lat GPS Latitude
 * @param[out] lon GPS Longitude
 * @returns True on success, false on error
 */
bool n2k_129025_values(const n2k_act_message *n, double *lat, double *lon) {
	if (!n || n->PGN != 129025 || !n->data || n->datalen < 8) { return false; }
	if (lat) {
		int32_t l = n2k_get_int32(n, 0);
		if (l == INT32_MAX) {
			(*lat) = NAN;
		} else {
			(*lat) = l * 1E-7;
		}
	}
	if (lon) {
		int32_t l = n2k_get_int32(n, 4);
		if (l == INT32_MAX) {
			(*lon) = NAN;
		} else {
			(*lon) = n2k_get_int32(n, 4) * 1E-7;
		}
	}
	return !(isnan(*lat) || isnan(*lon));
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] mag Orientation reference flag (2 bits)
 * @param[out] course Device course, in degrees
 * @param[out] speed Device speed in m/s
 * @returns True on success, false on error
 */
bool n2k_129026_values(const n2k_act_message *n, uint8_t *seq, uint8_t *mag, double *course, double *speed) {
	if (!n || n->PGN != 129026 || !n->data || n->datalen < 8) { return false; }
	if (seq) { *seq = n2k_get_uint8(n, 0); }
	if (mag) { (*mag) = n2k_get_uint8(n, 1) & 0x03; }

	bool success = true;
	if (course) {
		int16_t c = n2k_get_int16(n, 2);
		if (c == INT16_MAX) {
			(*course) = NAN;
			success = false;
		} else {
			(*course) = c * N2K_TO_DEGREES;
		}
	}
	if (speed) {
		int16_t s = n2k_get_int16(n, 4);
		if (s == INT16_MAX) {
			(*speed) = NAN;
			success = false;
		} else {
			(*speed) = s * 0.01;
		}
	}
	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] epochDays Days since January 1st 1970
 * @param[out] seconds Seconds since local midnight
 * @param[out] utcMins Offset from UTC in minutes
 * @returns True on success, false on error
 */
bool n2k_129033_values(const n2k_act_message *n, uint16_t *epochDays, double *seconds, int16_t *utcMins) {
	if (!n || n->PGN != 129033 || !n->data || n->datalen < 8) { return false; }

	if (epochDays) { *epochDays = n2k_get_uint16(n, 0); }
	if (seconds) { *seconds = 0.0001 * n2k_get_uint32(n, 2); }
	if (utcMins) { *utcMins = n2k_get_int16(n, 6); }
	if ((*utcMins >= 1440) || (*utcMins <= -1440)) {
		*utcMins = 0;
		return false;
	}

	return true;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] ref Reference frame flag (3 bits)
 * @param[out] speed Wind speed in m/s
 * @param[out] angle Wind direction, in degrees
 * @returns True on success, false on error
 */
bool n2k_130306_values(const n2k_act_message *n, uint8_t *seq, uint8_t *ref, double *speed, double *angle) {
	if (!n || n->PGN != 130306 || !n->data || n->datalen < 8) { return false; }

	if (seq) { *seq = n2k_get_uint8(n, 0); }

	if (ref) { *ref = n2k_get_uint8(n, 5) & 0x07; }

	bool success = true;
	if (speed) {
		int16_t s = n2k_get_int16(n, 1);
		if (s == INT16_MAX) {
			(*speed) = NAN;
			success = false;
		} else {
			(*speed) = s * 0.01;
		}
	}

	if (angle) {
		int16_t a = n2k_get_int16(n, 3);
		if (a == INT16_MAX) {
			(*angle) = NAN;
			success = false;
		} else {
			(*angle) = a * N2K_TO_DEGREES;
		}
	}
	return success;
}

/*!
 * @param[in] n Input message
 * @param[out] seq Sequence number
 * @param[out] tid Temperature source
 * @param[out] hid Humidity Source
 * @param[out] temp Temperature
 * @param[out] humid Humidity
 * @param[out] press Atmospheric Pressure
 * @returns True on success, false on error
 */

bool n2k_130311_values(const n2k_act_message *n, uint8_t *seq, uint8_t *tid, uint8_t *hid, double *temp, double *humid,
                       double *press) {
	if (!n || n->PGN != 130311 || !n->data || n->datalen < 8) { return false; }

	if (seq) { (*seq) = n2k_get_uint8(n, 0); }

	bool success = true;
	uint8_t ids = n2k_get_uint8(n, 1);
	if (tid) { (*tid) = ids & 0x3F; }
	if (hid) { (*hid) = (ids & 0xC0) >> 6; }
	if (temp) {
		uint16_t t = n2k_get_uint16(n, 2);
		if (t == UINT16_MAX) {
			(*temp) = NAN;
			success = false;
		} else {
			(*temp) = t * 0.01 - 273.15;
		}
	}
	if (humid) {
		int16_t h = n2k_get_uint16(n, 4);
		if (h == INT16_MAX) {
			(*humid) = NAN;
			success = false;
		} else {
			(*humid) = h * 0.004;
		}
	}
	if (press) {
		uint16_t p = n2k_get_uint16(n, 6);
		if (p == UINT16_MAX) {
			(*press) = NAN;
			success = false;
		} else {
			(*press) = p;
		}
	}

	return success;
}

void n2k_60928_print(const n2k_act_message *n) {
	if (!n) { return; }
	uint32_t id = 0;  // Really 21 bits
	uint16_t mfr = 0; // Really 11 bits
	uint8_t instance = 0;
	uint8_t function = 0;
	uint8_t class = 0;
	uint8_t system = 0;
	uint8_t industry = 0;
	bool configurable = false;

	fprintf(stdout, "%.3f\t", (float)(n->timestamp / 1000.0));
	if (!n2k_60928_values(n, &id, &mfr, &instance, &function, &class, &system, &industry, &configurable)) {
		fprintf(stdout, "[!] ");
	}
	fprintf(stdout, "Address claim - ID: %08u, Manufacturer: %05u\n", id, mfr);
}

/*!
 * @param[in] n Input message
 * @param[in] d Delimiter - appended to output.
 */
void n2k_header_print(const n2k_act_message *n, const char d) {
	char delim = '\n';
	if (d) { delim = d; }
	fprintf(stdout, "%.3f\t", (float)(n->timestamp / 1000.0));
	if (n->dst == 255) {
		fprintf(stdout, "PGN %06d broadcast from %03d%c", n->PGN, n->src, delim);
	} else {
		fprintf(stdout, "PGN %06d sent from %03d to %03d%c", n->PGN, n->src, n->dst, delim);
	}
}

/*!
 * @param[in] n Input message
 */
void n2k_basic_print(const n2k_act_message *n) {
	if (!n) { return; }
	n2k_header_print(n, '\t');
	fprintf(stdout, "-- Not parsed --\n");
}

/*!
 * @param[in] n Input message
 */
void n2k_127250_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	double hdg = 0;
	double dev = 0;
	double var = 0;
	uint8_t ref = 0;

	n2k_header_print(n, '\t');
	if (!n2k_127250_values(n, &seq, &hdg, &dev, &var, &ref)) { fprintf(stdout, "[!] "); }

	char *magStr = NULL;
	switch (ref) {
		case 0:
			magStr = "True";
			break;
		case 1:
			magStr = "Magnetic";
			break;
		default:
			magStr = "Unknown Reference";
			break;
	}

	fprintf(stdout, "Heading: %.3lf [%s], Deviation: %+.3lf, Variation: %+.3lf. Seq. ID %03d\n", hdg, magStr, dev,
	        var, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_127251_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	double rot = 0;

	n2k_header_print(n, '\t');
	if (!n2k_127251_values(n, &seq, &rot)) { fprintf(stdout, "[!] "); }
	fprintf(stdout, "Rate of turn: %+.3lf. Seq. ID %03d\n", rot, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_127257_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	n2k_header_print(n, '\t');
	if (!n2k_127257_values(n, &seq, &yaw, &pitch, &roll)) { fprintf(stdout, "[!] "); }

	fprintf(stdout, "Pitch: %.3lf, Roll: %.3lf, Yaw: %.3lf. Seq. ID: %03d\n", pitch, roll, yaw, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_128267_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	double depth = 0;
	double offset = 0;
	double range = 0;

	n2k_header_print(n, '\t');
	if (!n2k_128267_values(n, &seq, &depth, &offset, &range)) { fprintf(stdout, "[!] "); }

	fprintf(stdout, "Water Depth: %.2lfm (Offset: %.2lf, Range: %.0lf) Seq. ID %03d\n", depth, offset, range, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_129025_print(const n2k_act_message *n) {
	if (!n) { return; }

	double lat = 0;
	double lon = 0;

	n2k_header_print(n, '\t');
	if (!n2k_129025_values(n, &lat, &lon)) { fprintf(stdout, "[!] "); }

	fprintf(stdout, "GPS Position: %lf, %lf\n", lat, lon);
}

/*!
 * @param[in] n Input message
 */
void n2k_129026_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	uint8_t magnetic = 0;
	double course = 0;
	double speed = 0;

	n2k_header_print(n, '\t');
	if (!n2k_129026_values(n, &seq, &magnetic, &course, &speed)) { fprintf(stdout, "[!] "); }

	char *magStr = NULL;
	switch (magnetic) {
		case 0:
			magStr = "True";
			break;
		case 1:
			magStr = "Magnetic";
			break;
		default:
			magStr = "Unknown Reference";
			break;
	}

	fprintf(stdout, "Speed: %.2lf @ %.3lf degrees [%s]. Seq. ID %03d\n", speed, course, magStr, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_129033_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint16_t days = 0;
	double secs = 0;
	int16_t utcOff = 0;

	n2k_header_print(n, '\t');
	if (!n2k_129033_values(n, &days, &secs, &utcOff)) { fprintf(stdout, "[!] "); }

	long timetemp = (long)(86400 * days + secs);
	struct tm dt = {0};
	gmtime_r(&timetemp, &dt);

	char timeS[30] = {0};
	strftime(timeS, sizeof(timeS), "%F %T", &dt);
	fprintf(stdout, "%s %+02.2f\n", timeS, utcOff / 60.0);
}

/*!
 * @param[in] n Input message
 */
void n2k_130306_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	double speed = 0;
	double angle = 0;
	uint8_t ref = 0;

	n2k_header_print(n, '\t');
	if (!n2k_130306_values(n, &seq, &ref, &speed, &angle)) { fprintf(stdout, "[!] "); }

	char *refStr = NULL;
	switch (ref) {
		case 0:
			refStr = "Relative to North";
			break;
		case 1:
			refStr = "Magnetic";
			break;
		case 2:
			refStr = "Apparent";
			break;
		case 3:
			refStr = "Relative to boat";
			break;
		default:
			refStr = "Unknown reference";
			break;
	}

	fprintf(stdout, "Wind Speed: %.2lf @ %.3lf degrees [%s]. Seq. ID %03d\n", speed, angle, refStr, seq);
}

/*!
 * @param[in] n Input message
 */
void n2k_130311_print(const n2k_act_message *n) {
	if (!n) { return; }

	uint8_t seq = 0;
	uint8_t tid = 0;
	uint8_t hid = 0;
	double t = 0;
	double h = 0;
	double p = 0;

	n2k_header_print(n, '\t');
	if (!n2k_130311_values(n, &seq, &tid, &hid, &t, &h, &p)) { fprintf(stdout, "[!] "); }

	char *tSrc = NULL;
	switch (tid) {
		case 0:
			tSrc = "Sea Water";
			break;
		case 1:
			tSrc = "External";
			break;
		case 2:
			tSrc = "Internal";
			break;
		case 3:
			tSrc = "Engine Room";
			break;
		case 4:
			tSrc = "Cabin";
			break;
		default:
			tSrc = "Unknown";
			break;
	}

	char *hSrc = NULL;
	switch (hid) {
		case 0:
			hSrc = "Internal";
			break;
		case 1:
			hSrc = "External";
			break;
		default:
			hSrc = "Unknown";
			break;
	}

	fprintf(stdout, "Environmental data: %+.2lfC (%s), %+.3lf%% RH (%s), %.0lf bar[?]. Seq ID %03d\n", t, tSrc, h,
	        hSrc, p, seq);
}
