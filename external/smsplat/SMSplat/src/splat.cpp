/****************************************************************************\
*	   SPLAT!: An RF Signal Path Loss And Terrain Analysis Tool          *
******************************************************************************
*	     Project started in 1997 by John A. Magliacane, KD2BD 	     *
*			  Last update: 07-Feb-2014			     *
******************************************************************************
*         Please consult the documentation for a complete list of	     *
*	     individuals who have contributed to this project. 		     *
******************************************************************************
*                                                                            *
*  This program is free software; you can redistribute it and/or modify it   *
*  under the terms of the GNU General Public License as published by the     *
*  Free Software Foundation; either version 2 of the License or any later    *
*  version.								     *
* 									     *
*  This program is distributed in the hope that it will useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
*  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License     *
*  for more details.							     *
*									     *
\****************************************************************************/

#include "splat.h"
#include "itwom3.0.hpp"
#include <opencv2/opencv.hpp>
#include <chrono>
#define GAMMA 2.5
#define BZBUFFER 65536

#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef TWOPI
#define TWOPI 6.283185307179586
#endif

#ifndef HALFPI
#define HALFPI 1.570796326794896
#endif

#define DEG2RAD 1.74532925199e-02
#define EARTHRADIUS 20902230.97
#define METERS_PER_MILE 1609.344
#define METERS_PER_FOOT 0.3048
#define KM_PER_MILE 1.609344
#define FOUR_THIRDS 1.3333333333333

SplatProcessor::SplatProcessor()
    : opened_(0),
      gpsav_(0),
      max_range_(0.0),
      forced_erp_(-1.0),
      fzone_clearance_(0.6),
      min_north_(90),
      max_north_(-90),
      min_west_(360),
      max_west_(-1),
      max_elevation_(-32768),
      min_elevation_(32768),
      metric_(0),
      dbm_(0),
      smooth_contours_(0),

      start_angle_(0.0),  // Start angle in degrees
      end_angle_(360.0),  // End angle in degrees
      specified_angle_mode_(0),
      transparent_mode_(0) {
        homeDir_ = std::getenv("HOME") ? std::getenv("HOME") : "";
        mapFilePath_ = homeDir_ + "/.cache/splat/splat_output.ppm";
        lrpFilePathInput_ = "";
        lrpFilePathCache_ = homeDir_ + "/.cache/splat/splat.lrp";
        elevFilePath_ = homeDir_ + "/.cache/splat/elev_data/";
      }

int SplatProcessor::interpolate(int y0, int y1, int x0, int x1, int n) {
    /* Perform linear interpolation between quantized contour
       levels displayed in field strength and path loss maps.
       If signal level x0 corresponds to color level y0, signal
       level x1 corresponds to color level y1, and signal level
       n falls somewhere between x0 and x1, determine what
       color value n corresponds to between y0 and y1. */

    int result = 0;
    double delta_x, delta_y;

    if (n <= x0) return y0;

    if (n >= x1) return y1;

    if (y0 == y1) return y0;

    if (x0 == x1) return y0;

    delta_y = (double)(y0 - y1);
    delta_x = (double)(x0 - x1);

    result = y0 + (int)ceil((delta_y / delta_x) * (n - x0));

    return result;
}

double SplatProcessor::arccos(double x, double y) {
    /* This function implements the arc cosine function,
       returning a value between 0 and TWOPI. */

    double result = 0.0;

    if (y > 0.0) result = acos(x / y);

    if (y < 0.0) result = PI + acos(x / y);

    return result;
}

int SplatProcessor::ReduceAngle(double angle) {
    /* This function normalizes the argument to
       an integer angle between 0 and 180 degrees */

    double temp;

    temp = acos(cos(angle * DEG2RAD));

    return (int)rint(temp / DEG2RAD);
}

double SplatProcessor::LonDiff(double lon1, double lon2) {
    /* This function returns the short path longitudinal
       difference between longitude1 and longitude2
       as an angle between -180.0 and +180.0 degrees.
       If lon1 is west of lon2, the result is positive.
       If lon1 is east of lon2, the result is negative. */

    double diff;

    diff = lon1 - lon2;

    if (diff <= -180.0) diff += 360.0;

    if (diff >= 180.0) diff -= 360.0;

    return diff;
}

char *SplatProcessor::dec2dms(double decimal) {
    /* Converts decimal degrees to degrees, minutes, seconds,
       (DMS) and returns the result as a character string. */

    char sign;
    int degrees, minutes, seconds;
    double a, b, c, d;

    if (decimal < 0.0) {
        decimal = -decimal;
        sign = -1;
    }

    else
        sign = 1;

    a = floor(decimal);
    b = 60.0 * (decimal - a);
    c = floor(b);
    d = 60.0 * (b - c);

    degrees = (int)a;
    minutes = (int)c;
    seconds = (int)d;

    if (seconds < 0) seconds = 0;

    if (seconds > 59) seconds = 59;

    string_[0] = 0;
    snprintf(string_, 250, "%d%c %d\' %d\"", degrees * sign, 176, minutes, seconds);
    return (string_);
}

int SplatProcessor::PutMask(double lat, double lon, int value) {
    /* Lines, text, markings, and coverage areas are stored in a
       mask that is combined with topology data when topographic
       maps are generated by SPLAT!.  This function sets and resets
       bits in the mask based on the latitude and longitude of the
       area pointed to. */

    int x, y, indx;
    char found;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found) {
        dem_[indx].mask[x][y] = value;
        return ((int)dem_[indx].mask[x][y]);
    }

    else
        return -1;
}

int SplatProcessor::OrMask(double lat, double lon, int value) {
    /* Lines, text, markings, and coverage areas are stored in a
       mask that is combined with topology data when topographic
       maps are generated by SPLAT!.  This function sets bits in
       the mask based on the latitude and longitude of the area
       pointed to. */

    int x, y, indx;
    char found;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found) {
        dem_[indx].mask[x][y] |= value;
        return ((int)dem_[indx].mask[x][y]);
    }

    else
        return -1;
}

int SplatProcessor::GetMask(double lat, double lon) {
    /* This function returns the mask bits based on the latitude
       and longitude given. */

    return (OrMask(lat, lon, 0));
}

int SplatProcessor::PutSignal(double lat, double lon, unsigned char signal) {
    /* This function writes a signal level (0-255)
       at the specified location for later recall. */

    int x, y, indx;
    char found;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found) {
        dem_[indx].signal[x][y] = signal;
        return (dem_[indx].signal[x][y]);
    }

    else
        return 0;
}

unsigned char SplatProcessor::GetSignal(double lat, double lon) {
    /* This function reads the signal level (0-255) at the
       specified location that was previously written by the
       complimentary PutSignal() function. */

    int x, y, indx;
    char found;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found)
        return (dem_[indx].signal[x][y]);
    else
        return 0;
}

double SplatProcessor::GetElevation(struct site location) {
    /* This function returns the elevation (in feet) of any location
       represented by the digital elevation model data in memory.
       Function returns -5000.0 for locations not found in memory. */

    char found;
    int x, y, indx;
    double elevation;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (location.lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, location.lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found)
        elevation = 3.28084 * dem_[indx].data[x][y];
    else
        elevation = -5000.0;

    return elevation;
}

int SplatProcessor::AddElevation(double lat, double lon, double height) {
    /* This function adds a user-defined terrain feature
       (in meters AGL) to the digital elevation model data
       in memory.  Does nothing and returns 0 for locations
       not found in memory. */

    char found;
    int x, y, indx;

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
        x = (int)rint(ppd_ * (lat - dem_[indx].min_north));
        y = mpi_ - (int)rint(ppd_ * (LonDiff(dem_[indx].max_west, lon)));

        if (x >= 0 && x <= mpi_ && y >= 0 && y <= mpi_)
            found = 1;
        else
            indx++;
    }

    if (found) dem_[indx].data[x][y] += (short)rint(height);

    return found;
}

double SplatProcessor::Distance(struct site site1, struct site site2) {
    /* This function returns the great circle distance
       in miles between any two site locations. */

    double lat1, lon1, lat2, lon2, distance;

    lat1 = site1.lat * DEG2RAD;
    lon1 = site1.lon * DEG2RAD;
    lat2 = site2.lat * DEG2RAD;
    lon2 = site2.lon * DEG2RAD;

    distance = 3959.0 * acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos((lon1) - (lon2)));

    return distance;
}

double SplatProcessor::Azimuth(struct site source, struct site destination) {
    /* This function returns the azimuth (in degrees) to the
       destination as seen from the location of the source. */

    double dest_lat, dest_lon, src_lat, src_lon, beta, azimuth, diff, num, den, fraction;

    dest_lat = destination.lat * DEG2RAD;
    dest_lon = destination.lon * DEG2RAD;

    src_lat = source.lat * DEG2RAD;
    src_lon = source.lon * DEG2RAD;

    /* Calculate Surface Distance */

    beta =
        acos(sin(src_lat) * sin(dest_lat) + cos(src_lat) * cos(dest_lat) * cos(src_lon - dest_lon));

    /* Calculate Azimuth */

    num = sin(dest_lat) - (sin(src_lat) * cos(beta));
    den = cos(src_lat) * sin(beta);
    fraction = num / den;

    /* Trap potential problems in acos() due to rounding */

    if (fraction >= 1.0) fraction = 1.0;

    if (fraction <= -1.0) fraction = -1.0;

    /* Calculate azimuth */

    azimuth = acos(fraction);

    /* Reference it to True North */

    diff = dest_lon - src_lon;

    if (diff <= -PI) diff += TWOPI;

    if (diff >= PI) diff -= TWOPI;

    if (diff > 0.0) azimuth = TWOPI - azimuth;

    return (azimuth / DEG2RAD);
}

double SplatProcessor::ElevationAngle(struct site source, struct site destination) {
    /* This function returns the angle of elevation (in degrees)
       of the destination as seen from the source location.
       A positive result represents an angle of elevation (uptilt),
       while a negative result represents an angle of depression
       (downtilt), as referenced to a normal to the center of
       the earth. */

    register double a, b, dx;

    a = GetElevation(destination) + destination.alt + earthradius_;
    b = GetElevation(source) + source.alt + earthradius_;

    dx = 5280.0 * Distance(source, destination);

    /* Apply the Law of Cosines */

    return ((180.0 * (acos(((b * b) + (dx * dx) - (a * a)) / (2.0 * b * dx))) / PI) - 90.0);
}

void SplatProcessor::ReadPath(struct site source, struct site destination) {
    /* This function generates a sequence of latitude and
       longitude positions between source and destination
       locations along a great circle path, and stores
       elevation and distance information for points
       along that path in the "path" structure. */

    int c;
    double azimuth, distance, lat1, lon1, beta, den, num, lat2, lon2, total_distance, dx, dy,
        path_length, miles_per_sample, samples_per_radian = 68755.0;
    struct site tempsite;

    lat1 = source.lat * DEG2RAD;
    lon1 = source.lon * DEG2RAD;

    lat2 = destination.lat * DEG2RAD;
    lon2 = destination.lon * DEG2RAD;

    if (ppd_ == 1200.0) samples_per_radian = 68755.0;

    if (ppd_ == 3600.0) samples_per_radian = 206265.0;

    azimuth = Azimuth(source, destination) * DEG2RAD;

    total_distance = Distance(source, destination);

    if (total_distance > (30.0 / ppd_)) /* > 0.5 pixel distance */
    {
        dx = samples_per_radian * acos(cos(lon1 - lon2));
        dy = samples_per_radian * acos(cos(lat1 - lat2));

        path_length = sqrt((dx * dx) + (dy * dy)); /* Total number of samples */

        miles_per_sample = total_distance / path_length; /* Miles per sample */
    }

    else {
        c = 0;
        dx = 0.0;
        dy = 0.0;
        path_length = 0.0;
        miles_per_sample = 0.0;
        total_distance = 0.0;

        lat1 = lat1 / DEG2RAD;
        lon1 = lon1 / DEG2RAD;

        path_.lat[c] = lat1;
        path_.lon[c] = lon1;
        path_.elevation[c] = GetElevation(source);
        path_.distance[c] = 0.0;
    }

    for (distance = 0.0, c = 0;
         (total_distance != 0.0 && distance <= total_distance && c < ARRAYSIZE);
         c++, distance = miles_per_sample * (double)c) {
        beta = distance / 3959.0;
        lat2 = asin(sin(lat1) * cos(beta) + cos(azimuth) * sin(beta) * cos(lat1));
        num = cos(beta) - (sin(lat1) * sin(lat2));
        den = cos(lat1) * cos(lat2);

        if (azimuth == 0.0 && (beta > HALFPI - lat1))
            lon2 = lon1 + PI;

        else if (azimuth == HALFPI && (beta > HALFPI + lat1))
            lon2 = lon1 + PI;

        else if (fabs(num / den) > 1.0)
            lon2 = lon1;

        else {
            if ((PI - azimuth) >= 0.0)
                lon2 = lon1 - arccos(num, den);
            else
                lon2 = lon1 + arccos(num, den);
        }

        while (lon2 < 0.0) lon2 += TWOPI;

        while (lon2 > TWOPI) lon2 -= TWOPI;

        lat2 = lat2 / DEG2RAD;
        lon2 = lon2 / DEG2RAD;

        path_.lat[c] = lat2;
        path_.lon[c] = lon2;
        tempsite.lat = lat2;
        tempsite.lon = lon2;
        path_.elevation[c] = GetElevation(tempsite);
        path_.distance[c] = distance;
    }

    /* Make sure exact destination point is recorded at path.length-1 */

    if (c < ARRAYSIZE) {
        path_.lat[c] = destination.lat;
        path_.lon[c] = destination.lon;
        path_.elevation[c] = GetElevation(destination);
        path_.distance[c] = total_distance;
        c++;
    }

    if (c < ARRAYSIZE)
        path_.length = c;
    else
        path_.length = ARRAYSIZE - 1;
}

double SplatProcessor::ElevationAngle2(struct site source, struct site destination, double er) {
    /* This function returns the angle of elevation (in degrees)
       of the destination as seen from the source location, UNLESS
       the path between the sites is obstructed, in which case, the
       elevation angle to the first obstruction is returned instead.
       "er" represents the earth radius. */

    int x;
    char block = 0;
    double source_alt, destination_alt, cos_xmtr_angle, cos_test_angle, test_alt, elevation,
        distance, source_alt2, first_obstruction_angle = 0.0;
    struct path temp;

    temp = path_;

    ReadPath(source, destination);

    distance = 5280.0 * Distance(source, destination);
    source_alt = er + source.alt + GetElevation(source);
    destination_alt = er + destination.alt + GetElevation(destination);
    source_alt2 = source_alt * source_alt;

    /* Calculate the cosine of the elevation angle of the
       destination (receiver) as seen by the source (transmitter). */

    cos_xmtr_angle = ((source_alt2) + (distance * distance) - (destination_alt * destination_alt)) /
                     (2.0 * source_alt * distance);

    /* Test all points in between source and destination locations to
       see if the angle to a topographic feature generates a higher
       elevation angle than that produced by the destination.  Begin
       at the source since we're interested in identifying the FIRST
       obstruction along the path between source and destination. */

    for (x = 2, block = 0; x < path_.length && block == 0; x++) {
        distance = 5280.0 * path_.distance[x];

        test_alt = earthradius_ +
                   (path_.elevation[x] == 0.0 ? path_.elevation[x] : path_.elevation[x] + clutter_);

        cos_test_angle = ((source_alt2) + (distance * distance) - (test_alt * test_alt)) /
                         (2.0 * source_alt * distance);

        /* Compare these two angles to determine if
           an obstruction exists.  Since we're comparing
           the cosines of these angles rather than
           the angles themselves, the sense of the
           following "if" statement is reversed from
           what it would be if the angles themselves
           were compared. */

        if (cos_xmtr_angle >= cos_test_angle) {
            block = 1;
            first_obstruction_angle = ((acos(cos_test_angle)) / DEG2RAD) - 90.0;
        }
    }

    if (block)
        elevation = first_obstruction_angle;

    else
        elevation = ((acos(cos_xmtr_angle)) / DEG2RAD) - 90.0;

    path_ = temp;

    return elevation;
}

double SplatProcessor::AverageTerrain(struct site source, double azimuthx, double start_distance,
                                      double end_distance) {
    /* This function returns the average terrain calculated in
       the direction of "azimuth" (degrees) between "start_distance"
       and "end_distance" (miles) from the source location.  If
       the terrain is all water (non-critical error), -5000.0 is
       returned.  If not enough SDF data has been loaded into
       memory to complete the survey (critical error), then
       -9999.0 is returned. */

    int c, samples, endpoint;
    double beta, lat1, lon1, lat2, lon2, num, den, azimuth, terrain = 0.0;
    struct site destination;

    lat1 = source.lat * DEG2RAD;
    lon1 = source.lon * DEG2RAD;

    /* Generate a path of elevations between the source
       location and the remote location provided. */

    beta = end_distance / 3959.0;

    azimuth = DEG2RAD * azimuthx;

    lat2 = asin(sin(lat1) * cos(beta) + cos(azimuth) * sin(beta) * cos(lat1));
    num = cos(beta) - (sin(lat1) * sin(lat2));
    den = cos(lat1) * cos(lat2);

    if (azimuth == 0.0 && (beta > HALFPI - lat1))
        lon2 = lon1 + PI;

    else if (azimuth == HALFPI && (beta > HALFPI + lat1))
        lon2 = lon1 + PI;

    else if (fabs(num / den) > 1.0)
        lon2 = lon1;

    else {
        if ((PI - azimuth) >= 0.0)
            lon2 = lon1 - arccos(num, den);
        else
            lon2 = lon1 + arccos(num, den);
    }

    while (lon2 < 0.0) lon2 += TWOPI;

    while (lon2 > TWOPI) lon2 -= TWOPI;

    lat2 = lat2 / DEG2RAD;
    lon2 = lon2 / DEG2RAD;

    destination.lat = lat2;
    destination.lon = lon2;

    /* If SDF data is missing for the endpoint of
       the radial, then the average terrain cannot
       be accurately calculated.  Return -9999.0 */

    if (GetElevation(destination) < -4999.0)
        return (-9999.0);
    else {
        ReadPath(source, destination);

        endpoint = path_.length;

        /* Shrink the length of the radial if the
           outermost portion is not over U.S. land. */

        for (c = endpoint - 1; c >= 0 && path_.elevation[c] == 0.0; c--);

        endpoint = c + 1;

        for (c = 0, samples = 0; c < endpoint; c++) {
            if (path_.distance[c] >= start_distance) {
                terrain +=
                    (path_.elevation[c] == 0.0 ? path_.elevation[c] : path_.elevation[c] + clutter_);
                samples++;
            }
        }

        if (samples == 0)
            terrain = -5000.0; /* No land */
        else
            terrain = (terrain / (double)samples);

        return terrain;
    }
}

double SplatProcessor::haat(struct site antenna) {
    /* This function returns the antenna's Height Above Average
       Terrain (HAAT) based on FCC Part 73.313(d).  If a critical
       error occurs, such as a lack of SDF data to complete the
       survey, -5000.0 is returned. */

    int azi, c;
    char error = 0;
    double terrain, avg_terrain, haat, sum = 0.0;

    /* Calculate the average terrain between 2 and 10 miles
       from the antenna site at azimuths of 0, 45, 90, 135,
       180, 225, 270, and 315 degrees. */

    for (c = 0, azi = 0; azi <= 315 && error == 0; azi += 45) {
        terrain = AverageTerrain(antenna, (double)azi, 2.0, 10.0);

        if (terrain < -9998.0) /* SDF data is missing */
            error = 1;

        if (terrain > -4999.0) /* It's land, not water */
        {
            sum += terrain; /* Sum of averages */
            c++;
        }
    }

    if (error)
        return -5000.0;
    else {
        avg_terrain = (sum / (double)c);
        haat = (antenna.alt + GetElevation(antenna)) - avg_terrain;
        return haat;
    }
}

void SplatProcessor::PlaceMarker(struct site location) {
    /* This function places text and marker data in the mask array
       for illustration on topographic maps generated by SPLAT!.
       By default, SPLAT! centers text information BELOW the marker,
       but may move it above, to the left, or to the right of the
       marker depending on how much room is available on the map,
       or depending on whether the area is already occupied by
       another marker or label.  If no room or clear space is
       available on the map to place the marker and its associated
       text, then the marker and text are not written to the map. */

    int a, b, c, byte;
    char ok2print, occupied;
    double x, y, lat, lon, textx = 0.0, texty = 0.0, xmin, xmax, ymin, ymax, p1, p3, p6, p8, p12,
                           p16, p24, label_length;

    xmin = (double)min_north_;
    xmax = (double)max_north_;
    ymin = (double)min_west_;
    ymax = (double)max_west_;
    lat = location.lat;
    lon = location.lon;

    if (lat < xmax && lat >= xmin && (LonDiff(lon, ymax) <= 0.0) && (LonDiff(lon, ymin) >= dpp_)) {
        p1 = 1.0 / ppd_;
        p3 = 3.0 / ppd_;
        p6 = 6.0 / ppd_;
        p8 = 8.0 / ppd_;
        p12 = 12.0 / ppd_;
        p16 = 16.0 / ppd_;
        p24 = 24.0 / ppd_;

        ok2print = 0;
        occupied = 0;

        /* Is Marker Position Clear Of Text Or Other Markers? */

        for (a = 0, x = lat - p3; (x <= xmax && x >= xmin && a < 7); x += p1, a++)
            for (b = 0, y = lon - p3;
                 (LonDiff(y, ymax) <= 0.0) && (LonDiff(y, ymin) >= dpp_) && b < 7; y += p1, b++)
                occupied |= (GetMask(x, y) & 2);

        if (occupied == 0) {
            /* Determine Where Text Can Be Positioned */

            /* label_length=length in pixels.
               Each character is 8 pixels wide. */

            label_length = p1 * (double)(strlen(location.name) << 3);

            if ((LonDiff(lon + label_length, ymax) <= 0.0) &&
                (LonDiff(lon - label_length, ymin) >= dpp_)) {
                /* Default: Centered Text */

                texty = lon + label_length / 2.0;

                if ((lat - p8) >= p16) {
                    /* Position Text Below The Marker */

                    textx = lat - p8;

                    x = textx;
                    y = texty;

                    /* Is This Position Clear Of
                       Text Or Other Markers? */

                    for (a = 0, occupied = 0; a < 16; a++) {
                        for (b = 0; b < (int)strlen(location.name); b++)
                            for (c = 0; c < 8; c++, y -= p1) occupied |= (GetMask(x, y) & 2);
                        x -= p1;
                        y = texty;
                    }

                    x = textx;
                    y = texty;

                    if (occupied == 0) ok2print = 1;
                }

                else {
                    /* Position Text Above The Marker */

                    textx = lat + p24;

                    x = textx;
                    y = texty;

                    /* Is This Position Clear Of
                       Text Or Other Markers? */

                    for (a = 0, occupied = 0; a < 16; a++) {
                        for (b = 0; b < (int)strlen(location.name); b++)
                            for (c = 0; c < 8; c++, y -= p1) occupied |= (GetMask(x, y) & 2);
                        x -= p1;
                        y = texty;
                    }

                    x = textx;
                    y = texty;

                    if (occupied == 0) ok2print = 1;
                }
            }

            if (ok2print == 0) {
                if (LonDiff(lon - label_length, ymin) >= dpp_) {
                    /* Position Text To The
                       Right Of The Marker */

                    textx = lat + p6;
                    texty = lon - p12;

                    x = textx;
                    y = texty;

                    /* Is This Position Clear Of
                       Text Or Other Markers? */

                    for (a = 0, occupied = 0; a < 16; a++) {
                        for (b = 0; b < (int)strlen(location.name); b++)
                            for (c = 0; c < 8; c++, y -= p1) occupied |= (GetMask(x, y) & 2);
                        x -= p1;
                        y = texty;
                    }

                    x = textx;
                    y = texty;

                    if (occupied == 0) ok2print = 1;
                }

                else {
                    /* Position Text To The
                       Left Of The Marker */

                    textx = lat + p6;
                    texty = lon + p8 + (label_length);

                    x = textx;
                    y = texty;

                    /* Is This Position Clear Of
                       Text Or Other Markers? */

                    for (a = 0, occupied = 0; a < 16; a++) {
                        for (b = 0; b < (int)strlen(location.name); b++)
                            for (c = 0; c < 8; c++, y -= p1) occupied |= (GetMask(x, y) & 2);
                        x -= p1;
                        y = texty;
                    }

                    x = textx;
                    y = texty;

                    if (occupied == 0) ok2print = 1;
                }
            }

            /* textx and texty contain the latitude and longitude
               coordinates that describe the placement of the text
               on the map. */

            if (ok2print) {
                /* Draw Text */

                x = textx;
                y = texty;

                for (a = 0; a < 16; a++) {
                    for (b = 0; b < (int)strlen(location.name); b++) {
                        byte = fontdata[16 * (location.name[b]) + a];

                        for (c = 128; c > 0; c = c >> 1, y -= p1)
                            if (byte & c) OrMask(x, y, 2);
                    }

                    x -= p1;
                    y = texty;
                }

                /* Draw Square Marker Centered
                   On Location Specified */

                for (a = 0, x = lat - p3; (x <= xmax && x >= xmin && a < 7); x += p1, a++)
                    for (b = 0, y = lon - p3;
                         (LonDiff(y, ymax) <= 0.0) && (LonDiff(y, ymin) >= dpp_) && b < 7;
                         y += p1, b++)
                        OrMask(x, y, 2);
            }
        }
    }
}

double SplatProcessor::ReadBearing(char *input) {
    /* This function takes numeric input in the form of a character
       string, and returns an equivalent bearing in degrees as a
       decimal number (double).  The input may either be expressed
       in decimal format (40.139722) or degree, minute, second
       format (40 08 23).  This function also safely handles
       extra spaces found either leading, trailing, or
       embedded within the numbers expressed in the
       input string.  Decimal seconds are permitted. */

    double seconds, bearing = 0.0;
    char string[20];
    int a, b, length, degrees, minutes;

    /* Copy "input" to "string", and ignore any extra
       spaces that might be present in the process. */

    string[0] = 0;
    length = strlen(input);

    for (a = 0, b = 0; a < length && a < 18; a++) {
        if ((input[a] != 32 && input[a] != '\n') ||
            (input[a] == 32 && input[a + 1] != 32 && input[a + 1] != '\n' && b != 0)) {
            string[b] = input[a];
            b++;
        }
    }

    string[b] = 0;

    /* Count number of spaces in the clean string. */

    length = strlen(string);

    for (a = 0, b = 0; a < length; a++)
        if (string[a] == 32) b++;

    if (b == 0) /* Decimal Format (40.139722) */
        sscanf(string, "%lf", &bearing);

    if (b == 2) /* Degree, Minute, Second Format (40 08 23.xx) */
    {
        sscanf(string, "%d %d %lf", &degrees, &minutes, &seconds);

        bearing = fabs((double)degrees);
        bearing += fabs(((double)minutes) / 60.0);
        bearing += fabs(seconds / 3600.0);

        if ((degrees < 0) || (minutes < 0) || (seconds < 0.0)) bearing = -bearing;
    }

    /* Anything else returns a 0.0 */

    if (bearing > 360.0 || bearing < -360.0) bearing = 0.0;

    return bearing;
}

SplatProcessor::site SplatProcessor::LoadQTH(char *filename) {
    /* This function reads SPLAT! .qth (site location) files.
       The latitude and longitude may be expressed either in
       decimal degrees, or in degree, minute, second format.
       Antenna height is assumed to be expressed in feet above
       ground level (AGL), unless followed by the letter 'M',
       or 'm', or by the word "meters" or "Meters", in which
       case meters is assumed, and is handled accordingly. */

    int x;
    char string[50], qthfile[255];
    struct site tempsite;
    FILE *fd = NULL;

    x = strlen(filename);
    strncpy(qthfile, filename, 254);

    if (qthfile[x - 3] != 'q' || qthfile[x - 2] != 't' || qthfile[x - 1] != 'h') {
        if (x > 249) qthfile[249] = 0;

        strncat(qthfile, ".qth\0", 5);
    }

    tempsite.lat = 91.0;
    tempsite.lon = 361.0;
    tempsite.alt = 0.0;
    tempsite.name[0] = 0;
    tempsite.filename[0] = 0;

    fd = fopen(qthfile, "r");

    if (fd != NULL) {
        /* Site Name */
        fgets(string, 49, fd);

        /* Strip <CR> and/or <LF> from end of site name */

        for (x = 0; string[x] != 13 && string[x] != 10 && string[x] != 0;
             tempsite.name[x] = string[x], x++);

        tempsite.name[x] = 0;

        /* Site Latitude */
        fgets(string, 49, fd);
        tempsite.lat = ReadBearing(string);

        /* Site Longitude */
        fgets(string, 49, fd);
        tempsite.lon = ReadBearing(string);

        if (tempsite.lon < 0.0) tempsite.lon += 360.0;

        /* Antenna Height */
        fgets(string, 49, fd);
        fclose(fd);

        /* Remove <CR> and/or <LF> from antenna height string */
        for (x = 0; string[x] != 13 && string[x] != 10 && string[x] != 0; x++);

        string[x] = 0;

        /* Antenna height may either be in feet or meters.
           If the letter 'M' or 'm' is discovered in
           the string, then this is an indication that
           the value given is expressed in meters, and
           must be converted to feet before exiting. */

        for (x = 0; string[x] != 'M' && string[x] != 'm' && string[x] != 0 && x < 48; x++);

        if (string[x] == 'M' || string[x] == 'm') {
            string[x] = 0;
            sscanf(string, "%f", &tempsite.alt);
            tempsite.alt *= 3.28084;
        }

        else {
            string[x] = 0;
            sscanf(string, "%f", &tempsite.alt);
        }

        for (x = 0; x < 254 && qthfile[x] != 0; x++) tempsite.filename[x] = qthfile[x];

        tempsite.filename[x] = 0;
    }

    return tempsite;
}

void SplatProcessor::LoadPAT(char *filename) {
    /* This function reads and processes antenna pattern (.az
       and .el) files that correspond in name to previously
       loaded SPLAT! .lrp files.  */

    int a, b, w, x, y, z, last_index, next_index, span;
    char string[255], azfile[255], elfile[255], *pointer = NULL;
    float az, xx, elevation, amplitude, rotation, valid1, valid2, delta, azimuth[361],
        azimuth_pattern[361], el_pattern[10001], elevation_pattern[361][1001], slant_angle[361],
        tilt, mechanical_tilt = 0.0, tilt_azimuth, tilt_increment, sum;
    FILE *fd = NULL;
    unsigned char read_count[10001];

    for (x = 0; filename[x] != '.' && filename[x] != 0 && x < 250; x++) {
        azfile[x] = filename[x];
        elfile[x] = filename[x];
    }

    azfile[x] = '.';
    azfile[x + 1] = 'a';
    azfile[x + 2] = 'z';
    azfile[x + 3] = 0;

    elfile[x] = '.';
    elfile[x + 1] = 'e';
    elfile[x + 2] = 'l';
    elfile[x + 3] = 0;

    rotation = 0.0;

    got_azimuth_pattern_ = 0;
    got_elevation_pattern_ = 0;

    /* Load .az antenna pattern file */

    fd = fopen(azfile, "r");

    if (fd != NULL) {
        /* Clear azimuth pattern array */

        for (x = 0; x <= 360; x++) {
            azimuth[x] = 0.0;
            read_count[x] = 0;
        }

        /* Read azimuth pattern rotation
           in degrees measured clockwise
           from true North. */

        fgets(string, 254, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%f", &rotation);

        /* Read azimuth (degrees) and corresponding
           normalized field radiation pattern amplitude
           (0.0 to 1.0) until EOF is reached. */

        fgets(string, 254, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%f %f", &az, &amplitude);

        do {
            x = (int)rintf(az);

            if (x >= 0 && x <= 360 && fd != NULL) {
                azimuth[x] += amplitude;
                read_count[x]++;
            }

            fgets(string, 254, fd);
            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            sscanf(string, "%f %f", &az, &amplitude);

        } while (feof(fd) == 0);

        fclose(fd);

        /* Handle 0=360 degree ambiguity */

        if ((read_count[0] == 0) && (read_count[360] != 0)) {
            read_count[0] = read_count[360];
            azimuth[0] = azimuth[360];
        }

        if ((read_count[0] != 0) && (read_count[360] == 0)) {
            read_count[360] = read_count[0];
            azimuth[360] = azimuth[0];
        }

        /* Average pattern values in case more than
            one was read for each degree of azimuth. */

        for (x = 0; x <= 360; x++) {
            if (read_count[x] > 1) azimuth[x] /= (float)read_count[x];
        }

        /* Interpolate missing azimuths
           to completely fill the array */

        last_index = -1;
        next_index = -1;

        for (x = 0; x <= 360; x++) {
            if (read_count[x] != 0) {
                if (last_index == -1)
                    last_index = x;
                else
                    next_index = x;
            }

            if (last_index != -1 && next_index != -1) {
                valid1 = azimuth[last_index];
                valid2 = azimuth[next_index];

                span = next_index - last_index;
                delta = (valid2 - valid1) / (float)span;

                for (y = last_index + 1; y < next_index; y++) azimuth[y] = azimuth[y - 1] + delta;

                last_index = y;
                next_index = -1;
            }
        }

        /* Perform azimuth pattern rotation
           and load azimuth_pattern[361] with
           azimuth pattern data in its final form. */

        for (x = 0; x < 360; x++) {
            y = x + (int)rintf(rotation);

            if (y >= 360) y -= 360;

            azimuth_pattern[y] = azimuth[x];
        }

        azimuth_pattern[360] = azimuth_pattern[0];

        got_azimuth_pattern_ = 255;
    }

    /* Read and process .el file */

    fd = fopen(elfile, "r");

    if (fd != NULL) {
        for (x = 0; x <= 10000; x++) {
            el_pattern[x] = 0.0;
            read_count[x] = 0;
        }

        /* Read mechanical tilt (degrees) and
           tilt azimuth in degrees measured
           clockwise from true North. */

        fgets(string, 254, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%f %f", &mechanical_tilt, &tilt_azimuth);

        /* Read elevation (degrees) and corresponding
           normalized field radiation pattern amplitude
           (0.0 to 1.0) until EOF is reached. */

        fgets(string, 254, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%f %f", &elevation, &amplitude);

        while (feof(fd) == 0) {
            /* Read in normalized radiated field values
               for every 0.01 degrees of elevation between
               -10.0 and +90.0 degrees */

            x = (int)rintf(100.0 * (elevation + 10.0));

            if (x >= 0 && x <= 10000) {
                el_pattern[x] += amplitude;
                read_count[x]++;
            }

            fgets(string, 254, fd);
            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            sscanf(string, "%f %f", &elevation, &amplitude);
        }

        fclose(fd);

        /* Average the field values in case more than
           one was read for each 0.01 degrees of elevation. */

        for (x = 0; x <= 10000; x++) {
            if (read_count[x] > 1) el_pattern[x] /= (float)read_count[x];
        }

        /* Interpolate between missing elevations (if
           any) to completely fill the array and provide
           radiated field values for every 0.01 degrees of
           elevation. */

        last_index = -1;
        next_index = -1;

        for (x = 0; x <= 10000; x++) {
            if (read_count[x] != 0) {
                if (last_index == -1)
                    last_index = x;
                else
                    next_index = x;
            }

            if (last_index != -1 && next_index != -1) {
                valid1 = el_pattern[last_index];
                valid2 = el_pattern[next_index];

                span = next_index - last_index;
                delta = (valid2 - valid1) / (float)span;

                for (y = last_index + 1; y < next_index; y++)
                    el_pattern[y] = el_pattern[y - 1] + delta;

                last_index = y;
                next_index = -1;
            }
        }

        /* Fill slant_angle[] array with offset angles based
           on the antenna's mechanical beam tilt (if any)
           and tilt direction (azimuth). */

        if (mechanical_tilt == 0.0) {
            for (x = 0; x <= 360; x++) slant_angle[x] = 0.0;
        }

        else {
            tilt_increment = mechanical_tilt / 90.0;

            for (x = 0; x <= 360; x++) {
                xx = (float)x;
                y = (int)rintf(tilt_azimuth + xx);

                while (y >= 360) y -= 360;

                while (y < 0) y += 360;

                if (x <= 180) slant_angle[y] = -(tilt_increment * (90.0 - xx));

                if (x > 180) slant_angle[y] = -(tilt_increment * (xx - 270.0));
            }
        }

        slant_angle[360] = slant_angle[0]; /* 360 degree wrap-around */

        for (w = 0; w <= 360; w++) {
            tilt = slant_angle[w];

            /** Convert tilt angle to
                an array index offset **/

            y = (int)rintf(100.0 * tilt);

            /* Copy shifted el_pattern[10001] field
               values into elevation_pattern[361][1001]
               at the corresponding azimuth, downsampling
               (averaging) along the way in chunks of 10. */

            for (x = y, z = 0; z <= 1000; x += 10, z++) {
                for (sum = 0.0, a = 0; a < 10; a++) {
                    b = a + x;

                    if (b >= 0 && b <= 10000) sum += el_pattern[b];
                    if (b < 0) sum += el_pattern[0];
                    if (b > 10000) sum += el_pattern[10000];
                }

                elevation_pattern[w][z] = sum / 10.0;
            }
        }

        got_elevation_pattern_ = 255;
    }

    for (x = 0; x <= 360; x++) {
        for (y = 0; y <= 1000; y++) {
            if (got_elevation_pattern_)
                elevation = elevation_pattern[x][y];
            else
                elevation = 1.0;

            if (got_azimuth_pattern_)
                az = azimuth_pattern[x];
            else
                az = 1.0;

            LR_.antenna_pattern[x][y] = az * elevation;
        }
    }
}

int SplatProcessor::LoadSDF_SDF(char *name) {
    /* This function reads uncompressed SPLAT Data Files (.sdf)
       containing digital elevation model data into memory.
       Elevation data, maximum and minimum elevations, and
       quadrangle limits are stored in the first available
       dem[] structure. */
    int x, y, data, indx, minlat, minlon, maxlat, maxlon;
    char found, free_page = 0, line[20], sdf_file[255], path_plus_name[512];
    FILE *fd;

    for (x = 0; name[x] != '.' && name[x] != 0 && x < 250; x++) sdf_file[x] = name[x];

    sdf_file[x] = 0;

    /* Parse filename for minimum latitude and longitude values */

    sscanf(sdf_file, "%d_%d_%d_%d", &minlat, &maxlat, &minlon, &maxlon);

    sdf_file[x] = '.';
    sdf_file[x + 1] = 's';
    sdf_file[x + 2] = 'd';
    sdf_file[x + 3] = 'f';
    sdf_file[x + 4] = 0;

    /* Is it already in memory? */

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0; indx++) {
        if (minlat == dem_[indx].min_north && minlon == dem_[indx].min_west &&
            maxlat == dem_[indx].max_north && maxlon == dem_[indx].max_west)
            found = 1;
    }

    /* Is room available to load it? */

    if (found == 0) {
        for (indx = 0, free_page = 0; indx < MAXPAGES && free_page == 0; indx++)
            if (dem_[indx].max_north == -90) free_page = 1;
    }

    indx--;

    if (free_page && found == 0 && indx >= 0 && indx < MAXPAGES) {
        /* Search for SDF file in current working directory first */

        strncpy(path_plus_name, sdf_file, 255);

        fd = fopen(path_plus_name, "rb");

        if (fd == NULL) {
            /* Next, try loading SDF file from path specified
               in $HOME/.splat_path file or by -d argument */

            strncpy(path_plus_name, sdf_path_, 255);
            strncat(path_plus_name, sdf_file, 254);

            fd = fopen(path_plus_name, "rb");
        }

        if (fd != NULL) {
            fprintf(stdout, "Loading \"%s\" into page %d...", path_plus_name, indx + 1);
            fflush(stdout);

            fgets(line, 19, fd);
            sscanf(line, "%d", &dem_[indx].max_west);

            fgets(line, 19, fd);
            sscanf(line, "%d", &dem_[indx].min_north);

            fgets(line, 19, fd);
            sscanf(line, "%d", &dem_[indx].min_west);

            fgets(line, 19, fd);
            sscanf(line, "%d", &dem_[indx].max_north);

            for (x = 0; x < ippd_; x++)
                for (y = 0; y < ippd_; y++) {
                    fgets(line, 19, fd);
                    data = atoi(line);

                    dem_[indx].data[x][y] = data;
                    dem_[indx].signal[x][y] = 0;
                    dem_[indx].mask[x][y] = 0;

                    if (data > dem_[indx].max_el) dem_[indx].max_el = data;

                    if (data < dem_[indx].min_el) dem_[indx].min_el = data;
                }

            fclose(fd);

            if (dem_[indx].min_el < min_elevation_) min_elevation_ = dem_[indx].min_el;

            if (dem_[indx].max_el > max_elevation_) max_elevation_ = dem_[indx].max_el;

            if (max_north_ == -90)
                max_north_ = dem_[indx].max_north;

            else if (dem_[indx].max_north > max_north_)
                max_north_ = dem_[indx].max_north;

            if (min_north_ == 90)
                min_north_ = dem_[indx].min_north;

            else if (dem_[indx].min_north < min_north_)
                min_north_ = dem_[indx].min_north;

            if (max_west_ == -1)
                max_west_ = dem_[indx].max_west;

            else {
                if (abs(dem_[indx].max_west - max_west_) < 180) {
                    if (dem_[indx].max_west > max_west_) max_west_ = dem_[indx].max_west;
                }

                else {
                    if (dem_[indx].max_west < max_west_) max_west_ = dem_[indx].max_west;
                }
            }

            if (min_west_ == 360)
                min_west_ = dem_[indx].min_west;

            else {
                if (fabs(dem_[indx].min_west - min_west_) < 180.0) {
                    if (dem_[indx].min_west < min_west_) min_west_ = dem_[indx].min_west;
                }

                else {
                    if (dem_[indx].min_west > min_west_) min_west_ = dem_[indx].min_west;
                }
            }

            fprintf(stdout, " Done!\n");
            fflush(stdout);

            return 1;
        }

        else
            return -1;
    }

    else
        return 0;
}

char *SplatProcessor::BZfgets(BZFILE *bzfd, unsigned length) {
    /* This function returns at most one less than 'length' number
       of characters from a bz2 compressed file whose file descriptor
       is pointed to by *bzfd.  In operation, a buffer is filled with
       uncompressed data (size = BZBUFFER), which is then parsed
       and doled out as NULL terminated character strings every time
       this function is invoked.  A NULL string indicates an EOF
       or error condition. */

    static int x, y, nBuf;
    static char buffer[BZBUFFER + 1], output[BZBUFFER + 1];
    char done = 0;

    if (opened_ != 1 && bzerror_ == BZ_OK) {
        /* First time through.  Initialize everything! */

        x = 0;
        y = 0;
        nBuf = 0;
        opened_ = 1;
        output[0] = 0;
    }

    do {
        if (x == nBuf && bzerror_ != BZ_STREAM_END && bzerror_ == BZ_OK && opened_) {
            /* Uncompress data into a static buffer */

            nBuf = BZ2_bzRead(&bzerror_, bzfd, buffer, BZBUFFER);
            buffer[nBuf] = 0;
            x = 0;
        }

        /* Build a string from buffer contents */

        output[y] = buffer[x];

        if (output[y] == '\n' || output[y] == 0 || y == (int)length - 1) {
            output[y + 1] = 0;
            done = 1;
            y = 0;
        }

        else
            y++;
        x++;

    } while (done == 0);

    if (output[0] == 0) opened_ = 0;

    return (output);
}

int SplatProcessor::LoadSDF_BZ(char *name) {
    /* This function reads .bz2 compressed SPLAT Data Files containing
       digital elevation model data into memory.  Elevation data,
       maximum and minimum elevations, and quadrangle limits are
       stored in the first available dem[] structure. */

    int x, y, data, indx, minlat, minlon, maxlat, maxlon;
    char found, free_page = 0, sdf_file[255], path_plus_name[512], *string;
    FILE *fd;
    BZFILE *bzfd;

    for (x = 0; name[x] != '.' && name[x] != 0 && x < 247; x++) sdf_file[x] = name[x];

    sdf_file[x] = 0;

    /* Parse sdf_file name for minimum latitude and longitude values */

    sscanf(sdf_file, "%d_%d_%d_%d", &minlat, &maxlat, &minlon, &maxlon);

    sdf_file[x] = '.';
    sdf_file[x + 1] = 's';
    sdf_file[x + 2] = 'd';
    sdf_file[x + 3] = 'f';
    sdf_file[x + 4] = '.';
    sdf_file[x + 5] = 'b';
    sdf_file[x + 6] = 'z';
    sdf_file[x + 7] = '2';
    sdf_file[x + 8] = 0;

    /* Is it already in memory? */

    for (indx = 0, found = 0; indx < MAXPAGES && found == 0; indx++) {
        if (minlat == dem_[indx].min_north && minlon == dem_[indx].min_west &&
            maxlat == dem_[indx].max_north && maxlon == dem_[indx].max_west)
            found = 1;
    }

    /* Is room available to load it? */

    if (found == 0) {
        for (indx = 0, free_page = 0; indx < MAXPAGES && free_page == 0; indx++)
            if (dem_[indx].max_north == -90) free_page = 1;
    }

    indx--;

    if (free_page && found == 0 && indx >= 0 && indx < MAXPAGES) {
        /* Search for SDF file in current working directory first */

        strncpy(path_plus_name, sdf_file, 255);

        fd = fopen(path_plus_name, "rb");
        bzfd = BZ2_bzReadOpen(&bzerror_, fd, 0, 0, NULL, 0);

        if (fd == NULL || bzerror_ != BZ_OK) {
            /* Next, try loading SDF file from path specified
               in $HOME/.splat_path file or by -d argument */

            strncpy(path_plus_name, sdf_path_, 255);
            strncat(path_plus_name, sdf_file, 254);

            fd = fopen(path_plus_name, "rb");
            bzfd = BZ2_bzReadOpen(&bzerror_, fd, 0, 0, NULL, 0);
        }

        if (fd != NULL && bzerror_ == BZ_OK) {
            fprintf(stdout, "Loading \"%s\" into page %d...", path_plus_name, indx + 1);
            fflush(stdout);

            sscanf(BZfgets(bzfd, 255), "%d", &dem_[indx].max_west);
            sscanf(BZfgets(bzfd, 255), "%d", &dem_[indx].min_north);
            sscanf(BZfgets(bzfd, 255), "%d", &dem_[indx].min_west);
            sscanf(BZfgets(bzfd, 255), "%d", &dem_[indx].max_north);

            for (x = 0; x < ippd_; x++)
                for (y = 0; y < ippd_; y++) {
                    string = BZfgets(bzfd, 20);
                    data = atoi(string);

                    dem_[indx].data[x][y] = data;
                    dem_[indx].signal[x][y] = 0;
                    dem_[indx].mask[x][y] = 0;

                    if (data > dem_[indx].max_el) dem_[indx].max_el = data;

                    if (data < dem_[indx].min_el) dem_[indx].min_el = data;
                }

            fclose(fd);

            BZ2_bzReadClose(&bzerror_, bzfd);

            if (dem_[indx].min_el < min_elevation_) min_elevation_ = dem_[indx].min_el;

            if (dem_[indx].max_el > max_elevation_) max_elevation_ = dem_[indx].max_el;

            if (max_north_ == -90)
                max_north_ = dem_[indx].max_north;

            else if (dem_[indx].max_north > max_north_)
                max_north_ = dem_[indx].max_north;

            if (min_north_ == 90)
                min_north_ = dem_[indx].min_north;

            else if (dem_[indx].min_north < min_north_)
                min_north_ = dem_[indx].min_north;

            if (max_west_ == -1)
                max_west_ = dem_[indx].max_west;

            else {
                if (abs(dem_[indx].max_west - max_west_) < 180) {
                    if (dem_[indx].max_west > max_west_) max_west_ = dem_[indx].max_west;
                }

                else {
                    if (dem_[indx].max_west < max_west_) max_west_ = dem_[indx].max_west;
                }
            }

            if (min_west_ == 360)
                min_west_ = dem_[indx].min_west;

            else {
                if (abs(dem_[indx].min_west - min_west_) < 180) {
                    if (dem_[indx].min_west < min_west_) min_west_ = dem_[indx].min_west;
                }

                else {
                    if (dem_[indx].min_west > min_west_) min_west_ = dem_[indx].min_west;
                }
            }

            fprintf(stdout, " Done!\n");
            fflush(stdout);

            return 1;
        }

        else
            return -1;
    }

    else
        return 0;
}

char SplatProcessor::LoadSDF(char *name) {
    /* This function loads the requested SDF file from the filesystem.
       It first tries to invoke the LoadSDF_SDF() function to load an
       uncompressed SDF file (since uncompressed files load slightly
       faster).  If that attempt fails, then it tries to load a
       compressed SDF file by invoking the LoadSDF_BZ() function.
       If that fails, then we can assume that no elevation data
       exists for the region requested, and that the region
       requested must be entirely over water. */
    int x, y, indx, minlat, minlon, maxlat, maxlon;
    char found, free_page = 0;
    int return_value = -1;

    /* Try to load an uncompressed SDF first. */

    return_value = LoadSDF_SDF(name);

    /* If that fails, try loading a compressed SDF. */

    if (return_value == 0 || return_value == -1) return_value = LoadSDF_BZ(name);

    /* If neither format can be found, then assume the area is water. */

    if (return_value == 0 || return_value == -1) {
        /* Parse SDF name for minimum latitude and longitude values */

        sscanf(name, "%d_%d_%d_%d", &minlat, &maxlat, &minlon, &maxlon);

        /* Is it already in memory? */

        for (indx = 0, found = 0; indx < MAXPAGES && found == 0; indx++) {
            if (minlat == dem_[indx].min_north && minlon == dem_[indx].min_west &&
                maxlat == dem_[indx].max_north && maxlon == dem_[indx].max_west)
                found = 1;
        }

        /* Is room available to load it? */

        if (found == 0) {
            for (indx = 0, free_page = 0; indx < MAXPAGES && free_page == 0; indx++)
                if (dem_[indx].max_north == -90) free_page = 1;
        }

        indx--;

        if (free_page && found == 0 && indx >= 0 && indx < MAXPAGES) {
            fprintf(stdout, "Region  \"%s\" assumed as sea-level into page %d...", name, indx + 1);
            fflush(stdout);

            dem_[indx].max_west = maxlon;
            dem_[indx].min_north = minlat;
            dem_[indx].min_west = minlon;
            dem_[indx].max_north = maxlat;

            /* Fill DEM with sea-level topography */

            for (x = 0; x < ippd_; x++)
                for (y = 0; y < ippd_; y++) {
                    dem_[indx].data[x][y] = 0;
                    dem_[indx].signal[x][y] = 0;
                    dem_[indx].mask[x][y] = 0;

                    if (dem_[indx].min_el > 0) dem_[indx].min_el = 0;
                }

            if (dem_[indx].min_el < min_elevation_) min_elevation_ = dem_[indx].min_el;

            if (dem_[indx].max_el > max_elevation_) max_elevation_ = dem_[indx].max_el;

            if (max_north_ == -90)
                max_north_ = dem_[indx].max_north;

            else if (dem_[indx].max_north > max_north_)
                max_north_ = dem_[indx].max_north;

            if (min_north_ == 90)
                min_north_ = dem_[indx].min_north;

            else if (dem_[indx].min_north < min_north_)
                min_north_ = dem_[indx].min_north;

            if (max_west_ == -1)
                max_west_ = dem_[indx].max_west;

            else {
                if (abs(dem_[indx].max_west - max_west_) < 180) {
                    if (dem_[indx].max_west > max_west_) max_west_ = dem_[indx].max_west;
                }

                else {
                    if (dem_[indx].max_west < max_west_) max_west_ = dem_[indx].max_west;
                }
            }

            if (min_west_ == 360)
                min_west_ = dem_[indx].min_west;

            else {
                if (abs(dem_[indx].min_west - min_west_) < 180) {
                    if (dem_[indx].min_west < min_west_) min_west_ = dem_[indx].min_west;
                }

                else {
                    if (dem_[indx].min_west > min_west_) min_west_ = dem_[indx].min_west;
                }
            }

            fprintf(stdout, " Done!\n");
            fflush(stdout);

            return_value = 1;
        }
    }

    return return_value;
}

void SplatProcessor::LoadCities(char *filename) {
    /* This function reads SPLAT! city/site files, and plots
       the locations and names of the cities and site locations
       read on topographic maps generated by SPLAT! */

    int x, y, z;
    char input[80], str[3][80];
    struct site city_site;
    FILE *fd = NULL;

    fd = fopen(filename, "r");

    if (fd != NULL) {
        fgets(input, 78, fd);

        fprintf(stdout, "\nReading \"%s\"... ", filename);
        fflush(stdout);

        while (fd != NULL && feof(fd) == 0) {
            /* Parse line for name, latitude, and longitude */

            for (x = 0, y = 0, z = 0; x < 78 && input[x] != 0 && z < 3; x++) {
                if (input[x] != ',' && y < 78) {
                    str[z][y] = input[x];
                    y++;
                }

                else {
                    str[z][y] = 0;
                    z++;
                    y = 0;
                }
            }

            strncpy(city_site.name, str[0], 49);
            city_site.lat = ReadBearing(str[1]);
            city_site.lon = ReadBearing(str[2]);
            city_site.alt = 0.0;

            if (city_site.lon < 0.0) city_site.lon += 360.0;

            PlaceMarker(city_site);

            fgets(input, 78, fd);
        }

        fclose(fd);
        fprintf(stdout, "Done!");
        fflush(stdout);
    }

    else
        fprintf(stderr, "\n*** ERROR: \"%s\": not found!", filename);
}

void SplatProcessor::LoadUDT(char *filename) {
    /* This function reads a file containing User-Defined Terrain
       features for their addition to the digital elevation model
       data used by SPLAT!.  Elevations in the UDT file are evaluated
       and then copied into a temporary file under /tmp.  Then the
       contents of the temp file are scanned, and if found to be unique,
       are added to the ground elevations described by the digital
       elevation data already loaded into memory. */

    int i, x, y, z, ypix, xpix, tempxpix, tempypix, fd = 0;
    char input[80], str[3][80], tempname[15], *pointer = NULL;
    double latitude, longitude, height, tempheight;
    FILE *fd1 = NULL, *fd2 = NULL;

    strcpy(tempname, "/tmp/XXXXXX\0");

    fd1 = fopen(filename, "r");

    if (fd1 != NULL) {
        fd = mkstemp(tempname);
        fd2 = fopen(tempname, "w");

        fgets(input, 78, fd1);

        pointer = strchr(input, ';');

        if (pointer != NULL) *pointer = 0;

        fprintf(stdout, "\nReading \"%s\"... ", filename);
        fflush(stdout);

        while (feof(fd1) == 0) {
            /* Parse line for latitude, longitude, height */

            for (x = 0, y = 0, z = 0; x < 78 && input[x] != 0 && z < 3; x++) {
                if (input[x] != ',' && y < 78) {
                    str[z][y] = input[x];
                    y++;
                }

                else {
                    str[z][y] = 0;
                    z++;
                    y = 0;
                }
            }

            latitude = ReadBearing(str[0]);
            longitude = ReadBearing(str[1]);

            if (longitude < 0.0) longitude += 360.0;

            /* Remove <CR> and/or <LF> from antenna height string */

            for (i = 0; str[2][i] != 13 && str[2][i] != 10 && str[2][i] != 0; i++);

            str[2][i] = 0;

            /* The terrain feature may be expressed in either
               feet or meters.  If the letter 'M' or 'm' is
               discovered in the string, then this is an
               indication that the value given is expressed
               in meters.  Otherwise the height is interpreted
               as being expressed in feet.  */

            for (i = 0; str[2][i] != 'M' && str[2][i] != 'm' && str[2][i] != 0 && i < 48; i++);

            if (str[2][i] == 'M' || str[2][i] == 'm') {
                str[2][i] = 0;
                height = rint(atof(str[2]));
            }

            else {
                str[2][i] = 0;
                height = rint(METERS_PER_FOOT * atof(str[2]));
            }

            if (height > 0.0)
                fprintf(fd2, "%d, %d, %f\n", (int)rint(latitude / dpp_), (int)rint(longitude / dpp_),
                        height);

            fgets(input, 78, fd1);

            pointer = strchr(input, ';');

            if (pointer != NULL) *pointer = 0;
        }

        fclose(fd1);
        fclose(fd2);
        close(fd);

        fprintf(stdout, "Done!");
        fflush(stdout);

        fd1 = fopen(tempname, "r");
        fd2 = fopen(tempname, "r");

        y = 0;

        fscanf(fd1, "%d, %d, %lf", &xpix, &ypix, &height);

        do {
            x = 0;
            z = 0;

            fscanf(fd2, "%d, %d, %lf", &tempxpix, &tempypix, &tempheight);

            do {
                if (x > y && xpix == tempxpix && ypix == tempypix) {
                    z = 1; /* Dupe! */

                    if (tempheight > height) height = tempheight;
                }

                else {
                    fscanf(fd2, "%d, %d, %lf", &tempxpix, &tempypix, &tempheight);
                    x++;
                }

            } while (feof(fd2) == 0 && z == 0);

            if (z == 0) /* No duplicate found */
                AddElevation(xpix * dpp_, ypix * dpp_, height);

            fscanf(fd1, "%d, %d, %lf", &xpix, &ypix, &height);
            y++;

            rewind(fd2);

        } while (feof(fd1) == 0);

        fclose(fd1);
        fclose(fd2);
        unlink(tempname);
    }

    else
        fprintf(stderr, "\n*** ERROR: \"%s\": not found!", filename);

    fprintf(stdout, "\n");
}

void SplatProcessor::LoadBoundaries(char *filename) {
    /* This function reads Cartographic Boundary Files available from
       the U.S. Census Bureau, and plots the data contained in those
       files on the PPM Map generated by SPLAT!.  Such files contain
       the coordinates that describe the boundaries of cities,
       counties, and states. */

    int x;
    double lat0, lon0, lat1, lon1;
    char string[80];
    struct site source, destination;
    FILE *fd = NULL;

    fd = fopen(filename, "r");

    if (fd != NULL) {
        fgets(string, 78, fd);

        fprintf(stdout, "\nReading \"%s\"... ", filename);
        fflush(stdout);

        do {
            fgets(string, 78, fd);
            sscanf(string, "%lf %lf", &lon0, &lat0);
            fgets(string, 78, fd);

            do {
                sscanf(string, "%lf %lf", &lon1, &lat1);

                source.lat = lat0;
                source.lon = (lon0 > 0.0 ? 360.0 - lon0 : -lon0);
                destination.lat = lat1;
                destination.lon = (lon1 > 0.0 ? 360.0 - lon1 : -lon1);

                ReadPath(source, destination);

                for (x = 0; x < path_.length; x++) OrMask(path_.lat[x], path_.lon[x], 4);

                lat0 = lat1;
                lon0 = lon1;

                fgets(string, 78, fd);

            } while (strncmp(string, "END", 3) != 0 && feof(fd) == 0);

            fgets(string, 78, fd);

        } while (strncmp(string, "END", 3) != 0 && feof(fd) == 0);

        fclose(fd);

        fprintf(stdout, "Done!");
        fflush(stdout);
    }

    else
        fprintf(stderr, "\n*** ERROR: \"%s\": not found!", filename);
}

void SplatProcessor::setLRPFilePath(const std::string& path) {
    lrpFilePathInput_ = path;
}

bool SplatProcessor::ReadCustomLRParm(struct site txsite, bool forced_read) {
    int x;
    char string[150];
    char *pointer = NULL;
    FILE *fd = NULL;
    std::string lrpPathToUse;
    
    // First check if custom path is provided and not empty
    if (!lrpFilePathInput_.empty()) {
        lrpPathToUse = lrpFilePathInput_;
    } else {
        // Use default path based on home directory
        lrpPathToUse = lrpFilePathCache_;
    }
    // Check if LRP file exists
    if ((fd = fopen(lrpPathToUse.c_str(), "r")) == NULL) {
        // If file doesn't exist, set default values
        if (forced_read) {
            std::cout << "\nUsing default LRP parameters since no valid LRP file found" << std::endl;
            LR_.eps_dielect = 15.0;   // Dielectric constant
            LR_.sgm_conductivity = 0.005; // Conductivity (S/m)
            LR_.eno_ns_surfref = 301.0;   // Earth's refractivity
            LR_.frq_mhz = 1400.0;      // Frequency (MHz)
            LR_.radio_climate = 5;    // Climate (5 = Continental temperate)
            LR_.pol = 1;              // Polarization (0 = Horizontal)
            LR_.conf = 0.50;          // Reliability for predictions
            LR_.rel = 0.50;           // Percentage of situations
            LR_.erp = 126;           // ERP in watts
            
            // Initialize antenna pattern to 1.0 (0 dB)
            for (int i = 0; i < 360; i++) {
                for (int j = 0; j < 1001; j++) {
                    LR_.antenna_pattern[i][j] = 1.0;
                }
            }

            if (forced_freq_ >= 20.0 && forced_freq_ <= 20000.0)
                LR_.frq_mhz = forced_freq_;
            // Print warning that defaults are being used
            fprintf(stderr, "\nWarning: LRP file not found at %s, using default values\n", 
                    lrpPathToUse.c_str());
            return false;
        } else {
            // If not forced and we have valid coordinates, throw an exception
            throw std::runtime_error("\nError: LRP file not found at " + lrpPathToUse);
        }
    } else {
        // Read from LRP file similar to ReadLRParm
        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.eps_dielect);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.sgm_conductivity);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.eno_ns_surfref);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.frq_mhz);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%d", &LR_.radio_climate);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%d", &LR_.pol);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.conf);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.rel);

        fgets(string, 149, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL)
            *pointer = 0;

        sscanf(string, "%lf", &LR_.erp);

        // Initialize antenna pattern with default values (1.0 = 0 dB)
        for (int i = 0; i < 360; i++) {
            for (int j = 0; j < 1001; j++) {
                LR_.antenna_pattern[i][j] = 1.0;
            }
        }
        
        // Try to read antenna pattern file if specified
        fgets(string, 149, fd);
        pointer = strchr(string, ';');
        
        if (pointer != NULL)
            *pointer = 0;
            
        // Remove any trailing whitespace
        for (x = 0; string[x] != '\0' && string[x] != '\n' && string[x] != '\r'; x++);
        string[x] = 0;
        
        // If a filename is specified for the antenna pattern, try to read it
        if (strlen(string) > 0 && strcmp(string, "none") != 0) {
            FILE* pattern_file = fopen(string, "r");
            if (pattern_file != NULL) {
                // Read antenna pattern file
                int az, el;
                float gain;
                while (fscanf(pattern_file, "%d,%d,%f", &az, &el, &gain) == 3) {
                    if (az >= 0 && az < 360 && el >= 0 && el <= 1000) {
                        LR_.antenna_pattern[az][el] = gain;
                    }
                }
                
                fclose(pattern_file);
            }
        }

        fclose(fd);

        // Make sure ERP is valid
        if (LR_.erp < 0.0)
            LR_.erp = 0.0;
        if (forced_freq_ >= 20.0 && forced_freq_ <= 20000.0) LR_.frq_mhz = forced_freq_;

        // Value to signify the LRP file was read
        return true;
    }
    
}

char SplatProcessor::ReadLRParm(struct site txsite, char forced_read) {
    /* This function reads ITM parameter data for the transmitter
       site.  The file name is the same as the txsite, except the
       filename extension is .lrp.  If the needed file is not found,
       then the file "splat.lrp" is read from the current working
       directory.  Failure to load this file under a forced_read
       condition will result in the default parameters hard coded
       into this function to be used and written to "splat.lrp". */

    double din;
    char filename[255], string[80], *pointer = NULL, return_value = 0;
    int iin, ok = 0, x;
    FILE *fd = NULL, *outfile = NULL;

    /* Default parameters */

    LR_.eps_dielect = 0.0;
    LR_.sgm_conductivity = 0.0;
    LR_.eno_ns_surfref = 0.0;
    LR_.frq_mhz = 0.0;
    LR_.radio_climate = 0;
    LR_.pol = 0;
    LR_.conf = 0.0;
    LR_.rel = 0.0;
    LR_.erp = 0.0;

    /* Generate .lrp filename from txsite filename. */

    for (x = 0; txsite.filename[x] != '.' && txsite.filename[x] != 0 && x < 250; x++)
        filename[x] = txsite.filename[x];

    filename[x] = '.';
    filename[x + 1] = 'l';
    filename[x + 2] = 'r';
    filename[x + 3] = 'p';
    filename[x + 4] = 0;

    fd = fopen(filename, "r");

    if (fd == NULL) {
        /* Load default "splat.lrp" file */

        strncpy(filename, "splat.lrp\0", 10);
        fd = fopen(filename, "r");
    }

    if (fd != NULL) {
        fgets(string, 80, fd);

        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        ok = sscanf(string, "%lf", &din);

        if (ok) {
            LR_.eps_dielect = din;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%lf", &din);
        }

        if (ok) {
            LR_.sgm_conductivity = din;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%lf", &din);
        }

        if (ok) {
            LR_.eno_ns_surfref = din;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%lf", &din);
        }

        if (ok) {
            LR_.frq_mhz = din;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%d", &iin);
        }

        if (ok) {
            LR_.radio_climate = iin;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%d", &iin);
        }

        if (ok) {
            LR_.pol = iin;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%lf", &din);
        }

        if (ok) {
            LR_.conf = din;

            fgets(string, 80, fd);

            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%lf", &din);
        }

        if (ok) {
            LR_.rel = din;
            din = 0.0;
            return_value = 1;

            if (fgets(string, 80, fd) != NULL) {
                pointer = strchr(string, ';');

                if (pointer != NULL) *pointer = 0;

                if (sscanf(string, "%lf", &din)) LR_.erp = din;

                /* ERP in SPLAT! is referenced to 1 Watt
                   into a dipole (0 dBd).  If ERP is
                   expressed in dBm (referenced to a
                   0 dBi radiator), convert dBm in EIRP
                   to ERP.  */

                if ((strstr(string, "dBm") != NULL) || (strstr(string, "dbm") != NULL))
                    LR_.erp = (pow(10.0, (LR_.erp - 32.14) / 10.0));
            }
        }

        fclose(fd);

        if (forced_erp_ != -1.0) LR_.erp = forced_erp_;

        if (forced_freq_ >= 20.0 && forced_freq_ <= 20000.0) LR_.frq_mhz = forced_freq_;

        if (ok) LoadPAT(filename);
    }

    if (fd == NULL && forced_read) {
        /* Assign some default parameters
           for use in this run. */

        /* Write them to a "splat.lrp" file. */

        outfile = fopen("splat.lrp", "w");

        fprintf(outfile, "%.3f\t; Earth Dielectric Constant (Relative permittivity)\n",
                LR_.eps_dielect);
        fprintf(outfile, "%.3f\t; Earth Conductivity (Siemens per meter)\n", LR_.sgm_conductivity);
        fprintf(outfile, "%.3f\t; Atmospheric Bending Constant (N-Units)\n", LR_.eno_ns_surfref);
        fprintf(outfile, "%.3f\t; Frequency in MHz (20 MHz to 20 GHz)\n", LR_.frq_mhz);
        fprintf(outfile, "%d\t; Radio Climate\n", LR_.radio_climate);
        fprintf(outfile, "%d\t; Polarization (0 = Horizontal, 1 = Vertical)\n", LR_.pol);
        fprintf(outfile, "%.2f\t; Fraction of Situations\n", LR_.conf);
        fprintf(outfile, "%.2f\t; Fraction of Time\n", LR_.rel);
        fprintf(outfile,
                "%.2f\t; Transmitter Effective Radiated Power in Watts or dBm (optional)\n",
                LR_.erp);
        fprintf(outfile,
                "\nPlease consult SPLAT! documentation for the meaning and use of this data.\n");

        fclose(outfile);

        return_value = 1;

        fprintf(stderr,
                "\n\n%c*** There were problems reading your \"%s\" file! ***\nA \"splat.lrp\" file "
                "was written to your directory with default data.\n",
                7, filename);
    }

    else if (forced_read == 0)
        return_value = 0;

    if (forced_read && (fd == NULL || ok == 0)) {
        fprintf(stderr, "Default parameters have been assumed for this analysis.\n");

        return_value = 1;
    }

    return (return_value);
}

void SplatProcessor::PlotPath(struct site source, struct site destination, char mask_value) {
    /* This function analyzes the path between the source and
       destination locations.  It determines which points along
       the path have line-of-sight visibility to the source.
       Points along with path having line-of-sight visibility
       to the source at an AGL altitude equal to that of the
       destination location are stored by setting bit 1 in the
       mask[][] array, which are displayed in green when PPM
       maps are later generated by SPLAT!. */

    char block;
    int x, y;
    register double cos_xmtr_angle, cos_test_angle, test_alt;
    double distance, rx_alt, tx_alt;

    ReadPath(source, destination);

    for (y = 0; y < path_.length; y++) {
        /* Test this point only if it hasn't been already
           tested and found to be free of obstructions. */

        if ((GetMask(path_.lat[y], path_.lon[y]) & mask_value) == 0) {
            distance = 5280.0 * path_.distance[y];
            tx_alt = earthradius_ + source.alt + path_.elevation[0];
            rx_alt = earthradius_ + destination.alt + path_.elevation[y];

            /* Calculate the cosine of the elevation of the
               transmitter as seen at the temp rx point. */

            cos_xmtr_angle = ((rx_alt * rx_alt) + (distance * distance) - (tx_alt * tx_alt)) /
                             (2.0 * rx_alt * distance);

            for (x = y, block = 0; x >= 0 && block == 0; x--) {
                distance = 5280.0 * (path_.distance[y] - path_.distance[x]);
                test_alt = earthradius_ + (path_.elevation[x] == 0.0 ? path_.elevation[x]
                                                                   : path_.elevation[x] + clutter_);

                cos_test_angle =
                    ((rx_alt * rx_alt) + (distance * distance) - (test_alt * test_alt)) /
                    (2.0 * rx_alt * distance);

                /* Compare these two angles to determine if
                   an obstruction exists.  Since we're comparing
                   the cosines of these angles rather than
                   the angles themselves, the following "if"
                   statement is reversed from what it would
                   be if the actual angles were compared. */

                if (cos_xmtr_angle >= cos_test_angle) block = 1;
            }

            if (block == 0) OrMask(path_.lat[y], path_.lon[y], mask_value);
        }
    }
}

void SplatProcessor::PlotLRPath(struct site source, struct site destination,
                                unsigned char mask_value, FILE *fd) {
    /* This function plots the RF path loss between source and
       destination points based on the ITWOM propagation model,
       taking into account antenna pattern data, if available. */

    int x, y, ifs, ofs, errnum;
    char block = 0, strmode[100];
    double loss, azimuth, pattern = 0.0, xmtr_alt, dest_alt, xmtr_alt2, dest_alt2, cos_rcvr_angle,
                          cos_test_angle = 0.0, test_alt, elevation = 0.0, distance = 0.0,
                          four_thirds_earth, rxp, dBm, field_strength = 0.0;
    struct site temp;

    ReadPath(source, destination);

    four_thirds_earth = FOUR_THIRDS * EARTHRADIUS;

    /* Copy elevations plus clutter along path into the elev[] array. */

    for (x = 1; x < path_.length - 1; x++)
        elev_[x + 2] = (path_.elevation[x] == 0.0 ? path_.elevation[x] * METERS_PER_FOOT
                                                : (clutter_ + path_.elevation[x]) * METERS_PER_FOOT);

    /* Copy ending points without clutter */

    elev_[2] = path_.elevation[0] * METERS_PER_FOOT;
    elev_[path_.length + 1] = path_.elevation[path_.length - 1] * METERS_PER_FOOT;

    /* Since the only energy the propagation model considers
       reaching the destination is based on what is scattered
       or deflected from the first obstruction along the path,
       we first need to find the location and elevation angle
       of that first obstruction (if it exists).  This is done
       using a 4/3rds Earth radius to match the radius used by
       the irregular terrain propagation model.  This information
       is required for properly integrating the antenna's elevation
       pattern into the calculation for overall path loss. */

    for (y = 2; (y < (path_.length - 1) && path_.distance[y] <= max_range_); y++) {
        /* Process this point only if it
           has not already been processed. */

        if ((GetMask(path_.lat[y], path_.lon[y]) & 248) != (mask_value << 3)) {
            distance = 5280.0 * path_.distance[y];
            xmtr_alt = four_thirds_earth + source.alt + path_.elevation[0];
            dest_alt = four_thirds_earth + destination.alt + path_.elevation[y];
            dest_alt2 = dest_alt * dest_alt;
            xmtr_alt2 = xmtr_alt * xmtr_alt;

            /* Calculate the cosine of the elevation of
               the receiver as seen by the transmitter. */

            cos_rcvr_angle =
                ((xmtr_alt2) + (distance * distance) - (dest_alt2)) / (2.0 * xmtr_alt * distance);

            if (cos_rcvr_angle > 1.0) cos_rcvr_angle = 1.0;

            if (cos_rcvr_angle < -1.0) cos_rcvr_angle = -1.0;

            if (got_elevation_pattern_ || fd != NULL) {
                /* Determine the elevation angle to the first obstruction
                   along the path IF elevation pattern data is available
                   or an output (.ano) file has been designated. */

                for (x = 2, block = 0; (x < y && block == 0); x++) {
                    distance = 5280.0 * path_.distance[x];

                    test_alt = four_thirds_earth + (path_.elevation[x] == 0.0
                                                        ? path_.elevation[x]
                                                        : path_.elevation[x] + clutter_);

                    /* Calculate the cosine of the elevation
                       angle of the terrain (test point)
                       as seen by the transmitter. */

                    cos_test_angle = ((xmtr_alt2) + (distance * distance) - (test_alt * test_alt)) /
                                     (2.0 * xmtr_alt * distance);

                    if (cos_test_angle > 1.0) cos_test_angle = 1.0;

                    if (cos_test_angle < -1.0) cos_test_angle = -1.0;

                    /* Compare these two angles to determine if
                       an obstruction exists.  Since we're comparing
                       the cosines of these angles rather than
                       the angles themselves, the sense of the
                       following "if" statement is reversed from
                       what it would be if the angles themselves
                       were compared. */

                    if (cos_rcvr_angle >= cos_test_angle) block = 1;
                }

                if (block)
                    elevation = ((acos(cos_test_angle)) / DEG2RAD) - 90.0;
                else
                    elevation = ((acos(cos_rcvr_angle)) / DEG2RAD) - 90.0;
            }

            /* Determine attenuation for each point along
               the path using ITWOM's point_to_point mode
               starting at y=2 (number_of_points = 1), the
               shortest distance terrain can play a role in
               path loss. */

            elev_[0] = y - 1; /* (number of points - 1) */

            /* Distance between elevation samples */

            elev_[1] = METERS_PER_MILE * (path_.distance[y] - path_.distance[y - 1]);

            if (olditm_)
                point_to_point_ITM(
                    elev_, source.alt * METERS_PER_FOOT, destination.alt * METERS_PER_FOOT,
                    LR_.eps_dielect, LR_.sgm_conductivity, LR_.eno_ns_surfref, LR_.frq_mhz,
                    LR_.radio_climate, LR_.pol, LR_.conf, LR_.rel, loss, strmode, errnum);
            else
                point_to_point(elev_, source.alt * METERS_PER_FOOT,
                               destination.alt * METERS_PER_FOOT, LR_.eps_dielect,
                               LR_.sgm_conductivity, LR_.eno_ns_surfref, LR_.frq_mhz, LR_.radio_climate,
                               LR_.pol, LR_.conf, LR_.rel, loss, strmode, errnum);

            temp.lat = path_.lat[y];
            temp.lon = path_.lon[y];

            azimuth = (Azimuth(source, temp));

            if (fd != NULL)
                fprintf(fd, "%.7f, %.7f, %.3f, %.3f, ", path_.lat[y], path_.lon[y], azimuth,
                        elevation);

            /* If ERP==0, write path loss to alphanumeric
               output file.  Otherwise, write field strength
               or received power level (below), as appropriate. */

            if (fd != NULL && LR_.erp == 0.0) fprintf(fd, "%.2f", loss);

            /* Integrate the antenna's radiation
               pattern into the overall path loss. */

            x = (int)rint(10.0 * (10.0 - elevation));

            if (x >= 0 && x <= 1000) {
                azimuth = rint(azimuth);

                pattern = (double)LR_.antenna_pattern[(int)azimuth][x];

                if (pattern != 0.0) {
                    pattern = 20.0 * log10(pattern);
                    loss -= pattern;
                }
            }

            if (LR_.erp != 0.0) {
                if (dbm_) {
                    /* dBm is based on EIRP (ERP + 2.14) */

                    rxp = LR_.erp / (pow(10.0, (loss - 2.14) / 10.0));

                    dBm = 10.0 * (log10(rxp * 1000.0));

                    if (fd != NULL) fprintf(fd, "%.3f", dBm);

                    /* Scale roughly between 0 and 255 */

                    ifs = 200 + (int)rint(dBm);

                    if (ifs < 0) ifs = 0;

                    if (ifs > 255) ifs = 255;

                    ofs = GetSignal(path_.lat[y], path_.lon[y]);

                    if (ofs > ifs) ifs = ofs;

                    PutSignal(path_.lat[y], path_.lon[y], (unsigned char)ifs);
                }

                else {
                    field_strength = (139.4 + (20.0 * log10(LR_.frq_mhz)) - loss) +
                                     (10.0 * log10(LR_.erp / 1000.0));

                    ifs = 100 + (int)rint(field_strength);

                    if (ifs < 0) ifs = 0;

                    if (ifs > 255) ifs = 255;

                    ofs = GetSignal(path_.lat[y], path_.lon[y]);

                    if (ofs > ifs) ifs = ofs;

                    PutSignal(path_.lat[y], path_.lon[y], (unsigned char)ifs);

                    if (fd != NULL) fprintf(fd, "%.3f", field_strength);
                }
            }

            else {
                if (loss > 255)
                    ifs = 255;
                else
                    ifs = (int)rint(loss);

                ofs = GetSignal(path_.lat[y], path_.lon[y]);

                if (ofs < ifs && ofs != 0) ifs = ofs;

                PutSignal(path_.lat[y], path_.lon[y], (unsigned char)ifs);
            }

            if (fd != NULL) {
                if (block) fprintf(fd, " *");

                fprintf(fd, "\n");
            }

            /* Mark this point as having been analyzed */

            PutMask(path_.lat[y], path_.lon[y],
                    (GetMask(path_.lat[y], path_.lon[y]) & 7) + (mask_value << 3));
        }
    }
}

void SplatProcessor::PlotLOSMap(struct site source, double altitude) {
    /* This function performs a 360 degree sweep around the
       transmitter site (source location), and plots the
       line-of-sight coverage of the transmitter on the SPLAT!
       generated topographic map based on a receiver located
       at the specified altitude (in feet AGL).  Results
       are stored in memory, and written out in the form
       of a topographic map when the WritePPM() function
       is later invoked. */

    int y, z, count;
    struct site edge;
    unsigned char symbol[4], x;
    double lat, lon, minwest, maxnorth, th;
    static unsigned char mask_value = 1;

    symbol[0] = '.';
    symbol[1] = 'o';
    symbol[2] = 'O';
    symbol[3] = 'o';

    count = 0;

    fprintf(
        stdout, "\nComputing line-of-sight coverage of \"%s\" with an RX antenna\nat %.2f %s AGL",
        source.name, metric_ ? altitude * METERS_PER_FOOT : altitude, metric_ ? "meters" : "feet");

    if (clutter_ > 0.0)
        fprintf(stdout, " and %.2f %s of ground clutter",
                metric_ ? clutter_ * METERS_PER_FOOT : clutter_, metric_ ? "meters" : "feet");

    fprintf(stdout, "...\n\n 0%c to  25%c ", 37, 37);
    fflush(stdout);

    /* th=pixels/degree divided by 64 loops per
       progress indicator symbol (.oOo) printed. */

    th = ppd_ / 64.0;

    z = (int)(th * ReduceAngle(max_west_ - min_west_));

    minwest = dpp_ + (double)min_west_;
    maxnorth = (double)max_north_ - dpp_;

    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = max_north_;
        edge.lon = lon;
        edge.alt = altitude;

        PlotPath(source, edge, mask_value);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n25%c to  50%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));

    for (lat = maxnorth, x = 0, y = 0; lat >= (double)min_north_;
         y++, lat = maxnorth - (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = min_west_;
        edge.alt = altitude;

        PlotPath(source, edge, mask_value);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n50%c to  75%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * ReduceAngle(max_west_ - min_west_));

    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = min_north_;
        edge.lon = lon;
        edge.alt = altitude;

        PlotPath(source, edge, mask_value);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n75%c to 100%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));

    for (lat = (double)min_north_, x = 0, y = 0; lat < (double)max_north_;
         y++, lat = (double)min_north_ + (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = max_west_;
        edge.alt = altitude;

        PlotPath(source, edge, mask_value);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    fprintf(stdout, "\nDone!\n");
    fflush(stdout);

    /* Assign next mask value */

    switch (mask_value) {
        case 1:
            mask_value = 8;
            break;

        case 8:
            mask_value = 16;
            break;

        case 16:
            mask_value = 32;
    }
}

double SplatProcessor::AdjustAngleForNegativeXAxis(double angle) {
    // Add up 90 degrees to shift reference to the positive y-axis instead of negative x-axis.

    double adjustedAngle = fmod(angle + 90.0, 360.0);

    // Ensure angle is within 0–360 degrees
    if (adjustedAngle > 360.0) {
        adjustedAngle -= 360.0;
    }

    return adjustedAngle;
}

void SplatProcessor::PlotLRMapSpecifiedAngles(struct site source, double altitude,
                                              char *plo_filename, double start_angle_,
                                              double end_angle_) {
    /* This function performs a 360 degree sweep around the
       transmitter site (source location), and plots the
       Irregular Terrain Model attenuation on the SPLAT!
       generated topographic map based on a receiver located
       at the specified altitude (in feet AGL).  Results
       are stored in memory, and written out in the form
       of a topographic map when the WritePPMLR() or
       WritePPMSS() functions are later invoked. */
    // Adjust angles for the new reference (negative x-axis)
    // start_angle = AdjustAngleForNegativeXAxis(start_angle);
    // end_angle = AdjustAngleForNegativeXAxis(end_angle);
    
    /* Continue with the existing logic using the adjusted angles */
    printf("Adjusted Start Angle: %.2f, Adjusted End Angle: %.2f\n", start_angle_, end_angle_);

    int y, z, count;
    struct site edge;
    double lat, lon, minwest, maxnorth, th;
    double azimuth;
    unsigned char x, symbol[4];
    static unsigned char mask_value = 1;
    FILE *fd = NULL;

    minwest = dpp_ + (double)min_west_;
    maxnorth = (double)max_north_ - dpp_;

    symbol[0] = '.';
    symbol[1] = 'o';
    symbol[2] = 'O';
    symbol[3] = 'o';

    count = 0;

    if (olditm_)
        fprintf(stdout, "\nComputing ITM ");
    else
        fprintf(stdout, "\nComputing ITWOM ");

    if (LR_.erp == 0.0)
        fprintf(stdout, "path loss");
    else {
        if (dbm_)
            fprintf(stdout, "signal power level");
        else
            fprintf(stdout, "field strength");
    }

    fprintf(
        stdout, " contours of \"%s\"\nout to a radius of %.2f %s with an RX antenna at %.2f %s AGL",
        source.name, metric_ ? max_range_ * KM_PER_MILE : max_range_, metric_ ? "kilometers" : "miles",
        metric_ ? altitude * METERS_PER_FOOT : altitude, metric_ ? "meters" : "feet");

    if (clutter_ > 0.0)
        fprintf(stdout, "\nand %.2f %s of ground clutter",
                metric_ ? clutter_ * METERS_PER_FOOT : clutter_, metric_ ? "meters" : "feet");

    fprintf(stdout, "...\n\n 0%c to  25%c ", 37, 37);
    fflush(stdout);

    if (plo_filename[0] != 0) fd = fopen(plo_filename, "wb");

    if (fd != NULL) {
        /* Write header information to output file */

        fprintf(fd, "%d, %d\t; max_west, min_west\n%d, %d\t; max_north, min_north\n", max_west_,
                min_west_, max_north_, min_north_);
    }

    /* th=pixels/degree divided by 64 loops per
       progress indicator symbol (.oOo) printed. */

    th = ppd_ / 64.0;

    z = (int)(th * ReduceAngle(max_west_ - min_west_));
    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = max_north_;
        edge.lon = lon;
        edge.alt = altitude;

        // Inside the loop
        azimuth = (Azimuth(source, edge));

        // Skip points outside the specified angle range
        if (azimuth < start_angle_ || azimuth > end_angle_) {
            continue;
        }
        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n25%c to  50%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));
    for (lat = maxnorth, x = 0, y = 0; lat >= (double)min_north_;
         y++, lat = maxnorth - (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = min_west_;
        edge.alt = altitude;

        // Inside the loop
        azimuth = (Azimuth(source, edge));

        // Skip points outside the specified angle range
        if (azimuth < start_angle_ || azimuth > end_angle_) {
            continue;
        }

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n50%c to  75%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * ReduceAngle(max_west_ - min_west_));
    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = min_north_;
        edge.lon = lon;
        edge.alt = altitude;

        // Inside the loop
        azimuth = (Azimuth(source, edge));

        // Skip points outside the specified angle range
        if (azimuth < start_angle_ || azimuth > end_angle_) {
            continue;
        }

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n75%c to 100%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));
    for (lat = (double)min_north_, x = 0, y = 0; lat < (double)max_north_;
         y++, lat = (double)min_north_ + (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = max_west_;
        edge.alt = altitude;

        // Inside the loop
        azimuth = (Azimuth(source, edge));

        // Skip points outside the specified angle range
        if (azimuth < start_angle_ || azimuth > end_angle_) {
            continue;
        }

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    if (fd != NULL) fclose(fd);

    fprintf(stdout, "\nDone!\n");
    fflush(stdout);

    if (mask_value < 30) mask_value++;
}

void SplatProcessor::PlotLRMap(struct site source, double altitude, char *plo_filename) {
    /* This function performs a 360 degree sweep around the
       transmitter site (source location), and plots the
       Irregular Terrain Model attenuation on the SPLAT!
       generated topographic map based on a receiver located
       at the specified altitude (in feet AGL).  Results
       are stored in memory, and written out in the form
       of a topographic map when the WritePPMLR() or
       WritePPMSS() functions are later invoked. */
    int y, z, count;
    struct site edge;
    double lat, lon, minwest, maxnorth, th;
    unsigned char x, symbol[4];
    static unsigned char mask_value = 1;
    FILE *fd = NULL;

    minwest = dpp_ + (double)min_west_;
    maxnorth = (double)max_north_ - dpp_;

    symbol[0] = '.';
    symbol[1] = 'o';
    symbol[2] = 'O';
    symbol[3] = 'o';

    count = 0;

    if (olditm_)
        fprintf(stdout, "\nComputing ITM ");
    else
        fprintf(stdout, "\nComputing ITWOM ");

    if (LR_.erp == 0.0)
        fprintf(stdout, "path loss");
    else {
        if (dbm_)
            fprintf(stdout, "signal power level");
        else
            fprintf(stdout, "field strength");
    }

    fprintf(
        stdout, " contours of \"%s\"\nout to a radius of %.2f %s with an RX antenna at %.2f %s AGL",
        source.name, metric_ ? max_range_ * KM_PER_MILE : max_range_, metric_ ? "kilometers" : "miles",
        metric_ ? altitude * METERS_PER_FOOT : altitude, metric_ ? "meters" : "feet");

    if (clutter_ > 0.0)
        fprintf(stdout, "\nand %.2f %s of ground clutter",
                metric_ ? clutter_ * METERS_PER_FOOT : clutter_, metric_ ? "meters" : "feet");

    fprintf(stdout, "...\n\n 0%c to  25%c ", 37, 37);
    fflush(stdout);

    if (plo_filename[0] != 0) fd = fopen(plo_filename, "wb");

    if (fd != NULL) {
        /* Write header information to output file */

        fprintf(fd, "%d, %d\t; max_west, min_west\n%d, %d\t; max_north, min_north\n", max_west_,
                min_west_, max_north_, min_north_);
    }

    /* th=pixels/degree divided by 64 loops per
       progress indicator symbol (.oOo) printed. */

    th = ppd_ / 64.0;

    z = (int)(th * ReduceAngle(max_west_ - min_west_));

    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = max_north_;
        edge.lon = lon;
        edge.alt = altitude;

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n25%c to  50%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));

    for (lat = maxnorth, x = 0, y = 0; lat >= (double)min_north_;
         y++, lat = maxnorth - (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = min_west_;
        edge.alt = altitude;

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n50%c to  75%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * ReduceAngle(max_west_ - min_west_));

    for (lon = minwest, x = 0, y = 0; (LonDiff(lon, (double)max_west_) <= 0.0);
         y++, lon = minwest + (dpp_ * (double)y)) {
        if (lon >= 360.0) lon -= 360.0;

        edge.lat = min_north_;
        edge.lon = lon;
        edge.alt = altitude;

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    count = 0;
    fprintf(stdout, "\n75%c to 100%c ", 37, 37);
    fflush(stdout);

    z = (int)(th * (double)(max_north_ - min_north_));

    for (lat = (double)min_north_, x = 0, y = 0; lat < (double)max_north_;
         y++, lat = (double)min_north_ + (dpp_ * (double)y)) {
        edge.lat = lat;
        edge.lon = max_west_;
        edge.alt = altitude;

        PlotLRPath(source, edge, mask_value, fd);
        count++;

        if (count == z) {
            fprintf(stdout, "%c", symbol[x]);
            fflush(stdout);
            count = 0;

            if (x == 3)
                x = 0;
            else
                x++;
        }
    }

    if (fd != NULL) fclose(fd);

    fprintf(stdout, "\nDone!\n");
    fflush(stdout);

    if (mask_value < 30) mask_value++;

}

void SplatProcessor::LoadSignalColors(struct site xmtr) {
    int x, y, ok, val[4];
    char filename[255], string[80], *pointer = NULL;
    FILE *fd = NULL;

    for (x = 0; xmtr.filename[x] != '.' && xmtr.filename[x] != 0 && x < 250; x++)
        filename[x] = xmtr.filename[x];

    filename[x] = '.';
    filename[x + 1] = 's';
    filename[x + 2] = 'c';
    filename[x + 3] = 'f';
    filename[x + 4] = 0;

    /* Default values */

    region_.level[0] = 128;
    region_.color[0][0] = 255;
    region_.color[0][1] = 0;
    region_.color[0][2] = 0;

    region_.level[1] = 118;
    region_.color[1][0] = 255;
    region_.color[1][1] = 165;
    region_.color[1][2] = 0;

    region_.level[2] = 108;
    region_.color[2][0] = 255;
    region_.color[2][1] = 206;
    region_.color[2][2] = 0;

    region_.level[3] = 98;
    region_.color[3][0] = 255;
    region_.color[3][1] = 255;
    region_.color[3][2] = 0;

    region_.level[4] = 88;
    region_.color[4][0] = 184;
    region_.color[4][1] = 255;
    region_.color[4][2] = 0;

    region_.level[5] = 78;
    region_.color[5][0] = 0;
    region_.color[5][1] = 255;
    region_.color[5][2] = 0;

    region_.level[6] = 68;
    region_.color[6][0] = 0;
    region_.color[6][1] = 208;
    region_.color[6][2] = 0;

    region_.level[7] = 58;
    region_.color[7][0] = 0;
    region_.color[7][1] = 196;
    region_.color[7][2] = 196;

    region_.level[8] = 48;
    region_.color[8][0] = 0;
    region_.color[8][1] = 148;
    region_.color[8][2] = 255;

    region_.level[9] = 38;
    region_.color[9][0] = 80;
    region_.color[9][1] = 80;
    region_.color[9][2] = 255;

    region_.level[10] = 28;
    region_.color[10][0] = 0;
    region_.color[10][1] = 38;
    region_.color[10][2] = 255;

    region_.level[11] = 18;
    region_.color[11][0] = 142;
    region_.color[11][1] = 63;
    region_.color[11][2] = 255;

    region_.level[12] = 8;
    region_.color[12][0] = 140;
    region_.color[12][1] = 0;
    region_.color[12][2] = 128;

    region_.levels = 13;

    fd = fopen("splat.scf", "r");

    if (fd == NULL) fd = fopen(filename, "r");

    if (fd == NULL) {
        fd = fopen(filename, "w");

        fprintf(fd, "; SPLAT! Auto-generated Signal Color Definition (\"%s\") File\n", filename);
        fprintf(fd, ";\n; Format for the parameters held in this file is as follows:\n;\n");
        fprintf(fd, ";    dBuV/m: red, green, blue\n;\n");
        fprintf(fd, "; ...where \"dBuV/m\" is the signal strength (in dBuV/m) and\n");
        fprintf(fd, "; \"red\", \"green\", and \"blue\" are the corresponding RGB color\n");
        fprintf(fd, "; definitions ranging from 0 to 255 for the region specified.\n");
        fprintf(fd, ";\n; The following parameters may be edited and/or expanded\n");
        fprintf(fd, "; for future runs of SPLAT!  A total of 32 contour regions\n");
        fprintf(fd, "; may be defined in this file.\n;\n;\n");

        for (x = 0; x < region_.levels; x++)
            fprintf(fd, "%3d: %3d, %3d, %3d\n", region_.level[x], region_.color[x][0],
                    region_.color[x][1], region_.color[x][2]);

        fclose(fd);
    }

    else {
        x = 0;
        fgets(string, 80, fd);

        while (x < 32 && feof(fd) == 0) {
            pointer = strchr(string, ';');

            if (pointer != NULL) *pointer = 0;

            ok = sscanf(string, "%d: %d, %d, %d", &val[0], &val[1], &val[2], &val[3]);

            if (ok == 4) {
                for (y = 0; y < 4; y++) {
                    if (val[y] > 255) val[y] = 255;

                    if (val[y] < 0) val[y] = 0;
                }

                region_.level[x] = val[0];
                region_.color[x][0] = val[1];
                region_.color[x][1] = val[2];
                region_.color[x][2] = val[3];
                x++;
            }

            fgets(string, 80, fd);
        }

        fclose(fd);
        region_.levels = x;
    }
}

void SplatProcessor::LoadLossColors() {

    /* Default values */

    region_.level[0] = 80;
    region_.color[0][0] = 255;
    region_.color[0][1] = 0;
    region_.color[0][2] = 0;

    region_.level[1] = 90;
    region_.color[1][0] = 255;
    region_.color[1][1] = 128;
    region_.color[1][2] = 0;

    region_.level[2] = 100;
    region_.color[2][0] = 255;
    region_.color[2][1] = 165;
    region_.color[2][2] = 0;

    region_.level[3] = 110;
    region_.color[3][0] = 255;
    region_.color[3][1] = 206;
    region_.color[3][2] = 0;

    region_.level[4] = 120;
    region_.color[4][0] = 255;
    region_.color[4][1] = 255;
    region_.color[4][2] = 0;

    region_.level[5] = 130;
    region_.color[5][0] = 184;
    region_.color[5][1] = 255;
    region_.color[5][2] = 0;

    region_.level[6] = 140;
    region_.color[6][0] = 0;
    region_.color[6][1] = 255;
    region_.color[6][2] = 0;

    region_.level[7] = 150;
    region_.color[7][0] = 0;
    region_.color[7][1] = 208;
    region_.color[7][2] = 0;

    region_.level[8] = 160;
    region_.color[8][0] = 0;
    region_.color[8][1] = 196;
    region_.color[8][2] = 196;

    region_.level[9] = 170;
    region_.color[9][0] = 0;
    region_.color[9][1] = 148;
    region_.color[9][2] = 255;

    region_.level[10] = 180;
    region_.color[10][0] = 80;
    region_.color[10][1] = 80;
    region_.color[10][2] = 255;

    region_.level[11] = 190;
    region_.color[11][0] = 0;
    region_.color[11][1] = 38;
    region_.color[11][2] = 255;

    region_.level[12] = 200;
    region_.color[12][0] = 142;
    region_.color[12][1] = 63;
    region_.color[12][2] = 255;

    region_.level[13] = 210;
    region_.color[13][0] = 196;
    region_.color[13][1] = 54;
    region_.color[13][2] = 255;

    region_.level[14] = 220;
    region_.color[14][0] = 255;
    region_.color[14][1] = 0;
    region_.color[14][2] = 255;

    region_.level[15] = 230;
    region_.color[15][0] = 255;
    region_.color[15][1] = 194;
    region_.color[15][2] = 204;

    region_.levels = 16;

}

void SplatProcessor::LoadDBMColors() {

    /* Default values */

    region_.level[0] = 0;
    region_.color[0][0] = 255;
    region_.color[0][1] = 0;
    region_.color[0][2] = 0;

    region_.level[1] = -10;
    region_.color[1][0] = 255;
    region_.color[1][1] = 128;
    region_.color[1][2] = 0;

    region_.level[2] = -20;
    region_.color[2][0] = 255;
    region_.color[2][1] = 165;
    region_.color[2][2] = 0;

    region_.level[3] = -30;
    region_.color[3][0] = 255;
    region_.color[3][1] = 206;
    region_.color[3][2] = 0;

    region_.level[4] = -40;
    region_.color[4][0] = 255;
    region_.color[4][1] = 255;
    region_.color[4][2] = 0;

    region_.level[5] = -50;
    region_.color[5][0] = 184;
    region_.color[5][1] = 255;
    region_.color[5][2] = 0;

    region_.level[6] = -60;
    region_.color[6][0] = 0;
    region_.color[6][1] = 255;
    region_.color[6][2] = 0;

    region_.level[7] = -70;
    region_.color[7][0] = 0;
    region_.color[7][1] = 208;
    region_.color[7][2] = 0;

    region_.level[8] = -80;
    region_.color[8][0] = 0;
    region_.color[8][1] = 196;
    region_.color[8][2] = 196;

    region_.level[9] = -90;
    region_.color[9][0] = 0;
    region_.color[9][1] = 148;
    region_.color[9][2] = 255;

    region_.level[10] = -100;
    region_.color[10][0] = 80;
    region_.color[10][1] = 80;
    region_.color[10][2] = 255;

    region_.level[11] = -110;
    region_.color[11][0] = 0;
    region_.color[11][1] = 38;
    region_.color[11][2] = 255;

    region_.level[12] = -120;
    region_.color[12][0] = 142;
    region_.color[12][1] = 63;
    region_.color[12][2] = 255;

    region_.level[13] = -130;
    region_.color[13][0] = 196;
    region_.color[13][1] = 54;
    region_.color[13][2] = 255;

    region_.level[14] = -140;
    region_.color[14][0] = 255;
    region_.color[14][1] = 0;
    region_.color[14][2] = 255;

    region_.level[15] = -150;
    region_.color[15][0] = 255;
    region_.color[15][1] = 194;
    region_.color[15][2] = 204;

    region_.levels = 16;

}

void SplatProcessor::WritePPM(char *filename, unsigned char geo, unsigned char kml,
                              unsigned char ngs, struct site *xmtr, unsigned char txsites) {
    /* This function generates a topographic map in Portable Pix Map
       (PPM) format based on logarithmically scaled topology data,
       as well as the content of flags held in the mask[][] array.
       The image created is rotated counter-clockwise 90 degrees
       from its representation in dem[][] so that north points
       up and east points right in the image generated. */

    char mapfile[255], geofile[255], kmlfile[255];
    unsigned char found, mask;
    unsigned width, height, terrain;
    int indx, x, y, x0 = 0, y0 = 0;
    double lat, lon, conversion, one_over_gamma, north, south, east, west, minwest;
    FILE *fd;

    one_over_gamma = 1.0 / GAMMA;
    conversion = 255.0 / pow((double)(max_elevation_ - min_elevation_), one_over_gamma);

    width = (unsigned)(ippd_ * ReduceAngle(max_west_ - min_west_));
    height = (unsigned)(ippd_ * ReduceAngle(max_north_ - min_north_));

    if (filename[0] == 0) {
        strncpy(filename, xmtr[0].filename, 254);
        filename[strlen(filename) - 4] = 0; /* Remove .qth */
    }

    y = strlen(filename);

    if (y > 4) {
        if (filename[y - 1] == 'm' && filename[y - 2] == 'p' && filename[y - 3] == 'p' &&
            filename[y - 4] == '.')
            y -= 4;
    }

    for (x = 0; x < y; x++) {
        mapfile[x] = filename[x];
        geofile[x] = filename[x];
        kmlfile[x] = filename[x];
    }

    mapfile[x] = '.';
    geofile[x] = '.';
    kmlfile[x] = '.';
    mapfile[x + 1] = 'p';
    geofile[x + 1] = 'g';
    kmlfile[x + 1] = 'k';
    mapfile[x + 2] = 'p';
    geofile[x + 2] = 'e';
    kmlfile[x + 2] = 'm';
    mapfile[x + 3] = 'm';
    geofile[x + 3] = 'o';
    kmlfile[x + 3] = 'l';
    mapfile[x + 4] = 0;
    geofile[x + 4] = 0;
    kmlfile[x + 4] = 0;

    minwest = ((double)min_west_) + dpp_;

    if (minwest > 360.0) minwest -= 360.0;

    north = (double)max_north_ - dpp_;
    south = (double)min_north_;
    east = (minwest < 180.0 ? -minwest : 360.0 - min_west_);
    west = (double)(max_west_ < 180 ? -max_west_ : 360 - max_west_);

    if (kml == 0 && geo) {
        fd = fopen(geofile, "wb");

        fprintf(fd, "FILENAME\t%s\n", mapfile);
        fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
        fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);
        fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
        fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);
        fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", splat_name_, splat_version_);

        fclose(fd);
    }

    if (kml && geo == 0) {
        fd = fopen(kmlfile, "wb");

        fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
        fprintf(fd, "  <Folder>\n");
        fprintf(fd, "   <name>%s</name>\n", splat_name_);
        fprintf(fd, "     <description>Line-of-Sight Contour</description>\n");
        fprintf(fd, "       <GroundOverlay>\n");
        fprintf(fd, "         <name>%s Line-of-Sight Contour</name>\n", splat_name_);
        fprintf(fd, "           <description>SPLAT! Coverage</description>\n");
        fprintf(fd, "		<Icon>\n");
        fprintf(fd, "              <href>%s</href>\n", mapfile);
        fprintf(fd, "		</Icon>\n");
        /* fprintf(fd,"            <opacity>128</opacity>\n"); */
        fprintf(fd, "            <LatLonBox>\n");
        fprintf(fd, "               <north>%.5f</north>\n", north);
        fprintf(fd, "               <south>%.5f</south>\n", south);
        fprintf(fd, "               <east>%.5f</east>\n", east);
        fprintf(fd, "               <west>%.5f</west>\n", west);
        fprintf(fd, "               <rotation>0.0</rotation>\n");
        fprintf(fd, "            </LatLonBox>\n");
        fprintf(fd, "       </GroundOverlay>\n");

        for (x = 0; x < txsites; x++) {
            fprintf(fd, "     <Placemark>\n");
            fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
            fprintf(fd, "       <visibility>1</visibility>\n");
            fprintf(fd, "       <Style>\n");
            fprintf(fd, "       <IconStyle>\n");
            fprintf(fd, "        <Icon>\n");
            fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
            fprintf(fd, "          <x>224</x>\n");
            fprintf(fd, "          <y>224</y>\n");
            fprintf(fd, "          <w>32</w>\n");
            fprintf(fd, "          <h>32</h>\n");
            fprintf(fd, "        </Icon>\n");
            fprintf(fd, "       </IconStyle>\n");
            fprintf(fd, "       </Style>\n");
            fprintf(fd, "      <Point>\n");
            fprintf(fd, "        <extrude>1</extrude>\n");
            fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
            fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n",
                    (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat,
                    xmtr[x].alt);
            fprintf(fd, "      </Point>\n");
            fprintf(fd, "     </Placemark>\n");
        }

        fprintf(fd, "  </Folder>\n");
        fprintf(fd, "</kml>\n");

        fclose(fd);
    }

    fd = fopen(mapfile, "wb");

    fprintf(fd, "P6\n%u %u\n255\n", width, height);
    fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
    fflush(stdout);

    for (y = 0, lat = north; y < (int)height; y++, lat = north - (dpp_ * (double)y)) {
        for (x = 0, lon = max_west_; x < (int)width;
             x++, lon = (double)max_west_ - (dpp_ * (double)x)) {
            if (lon < 0.0) lon += 360.0;

            for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
                x0 = (int)rint(ppd_ * (lat - (double)dem_[indx].min_north));
                y0 = mpi_ - (int)rint(ppd_ * (LonDiff((double)dem_[indx].max_west, lon)));

                if (x0 >= 0 && x0 <= mpi_ && y0 >= 0 && y0 <= mpi_)
                    found = 1;
                else
                    indx++;
            }

            if (found) {
                mask = dem_[indx].mask[x0][y0];

                if (mask & 2) /* Text Labels: Red */
                    fprintf(fd, "%c%c%c", 255, 0, 0);

                else if (mask & 4)
                    /* County Boundaries: Light Cyan */
                    fprintf(fd, "%c%c%c", 128, 128, 255);

                else
                    switch (mask & 57) {
                        case 1:
                            /* TX1: Green */
                            fprintf(fd, "%c%c%c", 0, 255, 0);
                            break;

                        case 8:
                            /* TX2: Cyan */
                            fprintf(fd, "%c%c%c", 0, 255, 255);
                            break;

                        case 9:
                            /* TX1 + TX2: Yellow */
                            fprintf(fd, "%c%c%c", 255, 255, 0);
                            break;

                        case 16:
                            /* TX3: Medium Violet */
                            fprintf(fd, "%c%c%c", 147, 112, 219);
                            break;

                        case 17:
                            /* TX1 + TX3: Pink */
                            fprintf(fd, "%c%c%c", 255, 192, 203);
                            break;

                        case 24:
                            /* TX2 + TX3: Orange */
                            fprintf(fd, "%c%c%c", 255, 165, 0);
                            break;

                        case 25:
                            /* TX1 + TX2 + TX3: Dark Green */
                            fprintf(fd, "%c%c%c", 0, 100, 0);
                            break;

                        case 32:
                            /* TX4: Sienna 1 */
                            fprintf(fd, "%c%c%c", 255, 130, 71);
                            break;

                        case 33:
                            /* TX1 + TX4: Green Yellow */
                            fprintf(fd, "%c%c%c", 173, 255, 47);
                            break;

                        case 40:
                            /* TX2 + TX4: Dark Sea Green 1 */
                            fprintf(fd, "%c%c%c", 193, 255, 193);
                            break;

                        case 41:
                            /* TX1 + TX2 + TX4: Blanched Almond */
                            fprintf(fd, "%c%c%c", 255, 235, 205);
                            break;

                        case 48:
                            /* TX3 + TX4: Dark Turquoise */
                            fprintf(fd, "%c%c%c", 0, 206, 209);
                            break;

                        case 49:
                            /* TX1 + TX3 + TX4: Medium Spring Green */
                            fprintf(fd, "%c%c%c", 0, 250, 154);
                            break;

                        case 56:
                            /* TX2 + TX3 + TX4: Tan */
                            fprintf(fd, "%c%c%c", 210, 180, 140);
                            break;

                        case 57:
                            /* TX1 + TX2 + TX3 + TX4: Gold2 */
                            fprintf(fd, "%c%c%c", 238, 201, 0);
                            break;

                        default:
                            if (ngs) /* No terrain */
                                fprintf(fd, "%c%c%c", 255, 255, 255);
                            else {
                                /* Sea-level: Medium Blue */
                                if (dem_[indx].data[x0][y0] == 0)
                                    fprintf(fd, "%c%c%c", 0, 0, 170);
                                else {
                                    /* Elevation: Greyscale */
                                    terrain = (unsigned)(0.5 + pow((double)(dem_[indx].data[x0][y0] -
                                                                            min_elevation_),
                                                                   one_over_gamma) *
                                                                   conversion);
                                    fprintf(fd, "%c%c%c", terrain, terrain, terrain);
                                }
                            }
                    }
            }

            else {
                /* We should never get here, but if */
                /* we do, display the region as black */

                fprintf(fd, "%c%c%c", 0, 0, 0);
            }
        }
    }

    fclose(fd);
    fprintf(stdout, "Done!\n");
    fflush(stdout);
}

void SplatProcessor::WritePPMLR(char *filename, unsigned char geo, unsigned char kml,
                                unsigned char ngs, struct site *xmtr, unsigned char txsites) {
    /* This function generates a topographic map in Portable Pix Map
       (PPM) format based on the content of flags held in the mask[][]
       array (only).  The image created is rotated counter-clockwise
       90 degrees from its representation in dem[][] so that north
       points up and east points right in the image generated. */

    char mapfile[255], geofile[255], kmlfile[255], ckfile[255];
    unsigned width, height, red, green, blue, terrain = 0;
    unsigned char found, mask, cityorcounty;
    int indx, x, y, z, colorwidth, x0, y0, loss, level, hundreds, tens, units, match;
    double lat, lon, conversion, one_over_gamma, north, south, east, west, minwest;
    FILE *fd;

    one_over_gamma = 1.0 / GAMMA;
    conversion = 255.0 / pow((double)(max_elevation_ - min_elevation_), one_over_gamma);

    width = (unsigned)(ippd_ * ReduceAngle(max_west_ - min_west_));
    height = (unsigned)(ippd_ * ReduceAngle(max_north_ - min_north_));

    LoadLossColors();

    if (filename[0] == 0) {
        strncpy(filename, xmtr[0].filename, 254);
        filename[strlen(filename) - 4] = 0; /* Remove .qth */
    }

    y = strlen(filename);

    if (y > 240) y = 240;

    if (y > 4) {
        if (filename[y - 1] == 'm' && filename[y - 2] == 'p' && filename[y - 3] == 'p' &&
            filename[y - 4] == '.')
            y -= 4;
    }

    for (x = 0; x < y; x++) {
        mapfile[x] = filename[x];
        geofile[x] = filename[x];
        kmlfile[x] = filename[x];
        ckfile[x] = filename[x];
    }

    mapfile[x] = '.';
    geofile[x] = '.';
    kmlfile[x] = '.';
    mapfile[x + 1] = 'p';
    geofile[x + 1] = 'g';
    kmlfile[x + 1] = 'k';
    mapfile[x + 2] = 'p';
    geofile[x + 2] = 'e';
    kmlfile[x + 2] = 'm';
    mapfile[x + 3] = 'm';
    geofile[x + 3] = 'o';
    kmlfile[x + 3] = 'l';
    mapfile[x + 4] = 0;
    geofile[x + 4] = 0;
    kmlfile[x + 4] = 0;

    ckfile[x] = '-';
    ckfile[x + 1] = 'c';
    ckfile[x + 2] = 'k';
    ckfile[x + 3] = '.';
    ckfile[x + 4] = 'p';
    ckfile[x + 5] = 'p';
    ckfile[x + 6] = 'm';
    ckfile[x + 7] = 0;

    minwest = ((double)min_west_) + dpp_;

    if (minwest > 360.0) minwest -= 360.0;

    north = (double)max_north_ - dpp_;

    if (kml || geo)
        south = (double)min_north_; /* No bottom legend */
    else
        south = (double)min_north_ - (30.0 / ppd_); /* 30 pixels for bottom legend */

    east = (minwest < 180.0 ? -minwest : 360.0 - min_west_);
    west = (double)(max_west_ < 180 ? -max_west_ : 360 - max_west_);

    if (kml == 0 && geo) {
        fd = fopen(geofile, "wb");

        fprintf(fd, "FILENAME\t%s\n", mapfile);
        fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
        fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);

        fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
        fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);

        fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", splat_name_, splat_version_);

        fclose(fd);
    }

    if (kml && geo == 0) {
        fd = fopen(kmlfile, "wb");

        fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
        fprintf(fd, "<!-- Generated by %s Version %s -->\n", splat_name_, splat_version_);
        fprintf(fd, "  <Folder>\n");
        fprintf(fd, "   <name>%s</name>\n", splat_name_);
        fprintf(fd, "     <description>%s Transmitter Path Loss Overlay</description>\n",
                xmtr[0].name);
        fprintf(fd, "       <GroundOverlay>\n");
        fprintf(fd, "         <name>SPLAT! Path Loss Overlay</name>\n");
        fprintf(fd, "           <description>SPLAT! Coverage</description>\n");
        fprintf(fd, "		<Icon>\n");
        fprintf(fd, "              <href>%s</href>\n", mapfile);
        fprintf(fd, "		</Icon>\n");
        /* fprintf(fd,"            <opacity>128</opacity>\n"); */
        fprintf(fd, "            <LatLonBox>\n");
        fprintf(fd, "               <north>%.5f</north>\n", north);
        fprintf(fd, "               <south>%.5f</south>\n", south);
        fprintf(fd, "               <east>%.5f</east>\n", east);
        fprintf(fd, "               <west>%.5f</west>\n", west);
        fprintf(fd, "               <rotation>0.0</rotation>\n");
        fprintf(fd, "            </LatLonBox>\n");
        fprintf(fd, "       </GroundOverlay>\n");
        fprintf(fd, "       <ScreenOverlay>\n");
        fprintf(fd, "          <name>Color Key</name>\n");
        fprintf(fd, "		<description>Contour Color Key</description>\n");
        fprintf(fd, "          <Icon>\n");
        fprintf(fd, "            <href>%s</href>\n", ckfile);
        fprintf(fd, "          </Icon>\n");
        fprintf(fd,
                "          <overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd,
                "          <screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(
            fd,
            "          <rotationXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd, "          <size x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd, "       </ScreenOverlay>\n");

        for (x = 0; x < txsites; x++) {
            fprintf(fd, "     <Placemark>\n");
            fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
            fprintf(fd, "       <visibility>1</visibility>\n");
            fprintf(fd, "       <Style>\n");
            fprintf(fd, "       <IconStyle>\n");
            fprintf(fd, "        <Icon>\n");
            fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
            fprintf(fd, "          <x>224</x>\n");
            fprintf(fd, "          <y>224</y>\n");
            fprintf(fd, "          <w>32</w>\n");
            fprintf(fd, "          <h>32</h>\n");
            fprintf(fd, "        </Icon>\n");
            fprintf(fd, "       </IconStyle>\n");
            fprintf(fd, "       </Style>\n");
            fprintf(fd, "      <Point>\n");
            fprintf(fd, "        <extrude>1</extrude>\n");
            fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
            fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n",
                    (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat,
                    xmtr[x].alt);
            fprintf(fd, "      </Point>\n");
            fprintf(fd, "     </Placemark>\n");
        }

        fprintf(fd, "  </Folder>\n");
        fprintf(fd, "</kml>\n");

        fclose(fd);
    }

    fd = fopen(mapfile, "wb");

    if (kml || geo) {
        /* No bottom legend */

        fprintf(fd, "P6\n%u %u\n255\n", width, height);
        fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
    }

    else {
        /* Allow space for bottom legend */

        fprintf(fd, "P6\n%u %u\n255\n", width, height + 30);
        fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height + 30);
    }

    fflush(stdout);

    for (y = 0, lat = north; y < (int)height; y++, lat = north - (dpp_ * (double)y)) {
        for (x = 0, lon = max_west_; x < (int)width; x++, lon = max_west_ - (dpp_ * (double)x)) {
            if (lon < 0.0) lon += 360.0;

            for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
                x0 = (int)rint(ppd_ * (lat - (double)dem_[indx].min_north));
                y0 = mpi_ - (int)rint(ppd_ * (LonDiff((double)dem_[indx].max_west, lon)));

                if (x0 >= 0 && x0 <= mpi_ && y0 >= 0 && y0 <= mpi_)
                    found = 1;
                else
                    indx++;
            }

            if (found) {
                mask = dem_[indx].mask[x0][y0];
                loss = (dem_[indx].signal[x0][y0]);
                cityorcounty = 0;

                match = 255;

                red = 0;
                green = 0;
                blue = 0;

                if (loss <= region_.level[0])
                    match = 0;
                else {
                    for (z = 1; (z < region_.levels && match == 255); z++) {
                        if (loss >= region_.level[z - 1] && loss < region_.level[z]) match = z;
                    }
                }

                if (match < region_.levels) {
                    if (smooth_contours_ && match > 0) {
                        red = (unsigned)interpolate(region_.color[match - 1][0],
                                                    region_.color[match][0], region_.level[match - 1],
                                                    region_.level[match], loss);
                        green = (unsigned)interpolate(
                            region_.color[match - 1][1], region_.color[match][1],
                            region_.level[match - 1], region_.level[match], loss);
                        blue = (unsigned)interpolate(
                            region_.color[match - 1][2], region_.color[match][2],
                            region_.level[match - 1], region_.level[match], loss);
                    }

                    else {
                        red = region_.color[match][0];
                        green = region_.color[match][1];
                        blue = region_.color[match][2];
                    }
                }

                if (mask & 2) {
                    /* Text Labels: Red or otherwise */

                    if (red >= 180 && green <= 75 && blue <= 75 && loss != 0)
                        fprintf(fd, "%c%c%c", 255 ^ red, 255 ^ green, 255 ^ blue);
                    else
                        fprintf(fd, "%c%c%c", 255, 0, 0);

                    cityorcounty = 1;
                }

                else if (mask & 4) {
                    /* County Boundaries: Black */

                    fprintf(fd, "%c%c%c", 0, 0, 0);

                    cityorcounty = 1;
                }

                if (cityorcounty == 0) {
                    if (loss == 0 || (contour_threshold_ != 0 && loss > abs(contour_threshold_))) {
                        if (ngs) /* No terrain */
                            fprintf(fd, "%c%c%c", 255, 255, 255);
                        else {
                            /* Display land or sea elevation */

                            if (dem_[indx].data[x0][y0] == 0)
                                fprintf(fd, "%c%c%c", 0, 0, 170);
                            else {
                                terrain =
                                    (unsigned)(0.5 +
                                               pow((double)(dem_[indx].data[x0][y0] - min_elevation_),
                                                   one_over_gamma) *
                                                   conversion);
                                fprintf(fd, "%c%c%c", terrain, terrain, terrain);
                            }
                        }
                    }

                    else {
                        /* Plot path loss in color */

                        if (red != 0 || green != 0 || blue != 0)
                            fprintf(fd, "%c%c%c", red, green, blue);

                        else /* terrain / sea-level */
                        {
                            if (dem_[indx].data[x0][y0] == 0)
                                fprintf(fd, "%c%c%c", 0, 0, 170);
                            else {
                                /* Elevation: Greyscale */
                                terrain =
                                    (unsigned)(0.5 +
                                               pow((double)(dem_[indx].data[x0][y0] - min_elevation_),
                                                   one_over_gamma) *
                                                   conversion);
                                fprintf(fd, "%c%c%c", terrain, terrain, terrain);
                            }
                        }
                    }
                }
            }

            else {
                /* We should never get here, but if */
                /* we do, display the region as black */

                fprintf(fd, "%c%c%c", 0, 0, 0);
            }
        }
    }

    if (kml == 0 && geo == 0) {
        /* Display legend along bottom of image
         * if not generating .kml or .geo output.
         */

        colorwidth = (int)rint((float)width / (float)region_.levels);

        for (y0 = 0; y0 < 30; y0++) {
            for (x0 = 0; x0 < (int)width; x0++) {
                indx = x0 / colorwidth;
                x = x0 % colorwidth;
                level = region_.level[indx];

                hundreds = level / 100;

                if (hundreds > 0) level -= (hundreds * 100);

                tens = level / 10;

                if (tens > 0) level -= (tens * 10);

                units = level;

                if (y0 >= 8 && y0 <= 23) {
                    if (hundreds > 0) {
                        if (x >= 11 && x <= 18)
                            if (fontdata[16 * (hundreds + '0') + (y0 - 8)] & (128 >> (x - 11)))
                                indx = 255;
                    }

                    if (tens > 0 || hundreds > 0) {
                        if (x >= 19 && x <= 26)
                            if (fontdata[16 * (tens + '0') + (y0 - 8)] & (128 >> (x - 19)))
                                indx = 255;
                    }

                    if (x >= 27 && x <= 34)
                        if (fontdata[16 * (units + '0') + (y0 - 8)] & (128 >> (x - 27))) indx = 255;

                    if (x >= 42 && x <= 49)
                        if (fontdata[16 * ('d') + (y0 - 8)] & (128 >> (x - 42))) indx = 255;

                    if (x >= 50 && x <= 57)
                        if (fontdata[16 * ('B') + (y0 - 8)] & (128 >> (x - 50))) indx = 255;
                }

                if (indx > region_.levels)
                    fprintf(fd, "%c%c%c", 0, 0, 0);
                else {
                    red = region_.color[indx][0];
                    green = region_.color[indx][1];
                    blue = region_.color[indx][2];

                    fprintf(fd, "%c%c%c", red, green, blue);
                }
            }
        }
    }

    fclose(fd);

    if (kml) {
        /* Write colorkey image file */

        fd = fopen(ckfile, "wb");

        height = 30 * region_.levels;
        width = 100;

        fprintf(fd, "P6\n%u %u\n255\n", width, height);

        for (y0 = 0; y0 < (int)height; y0++) {
            for (x0 = 0; x0 < (int)width; x0++) {
                indx = y0 / 30;
                x = x0;
                level = region_.level[indx];

                hundreds = level / 100;

                if (hundreds > 0) level -= (hundreds * 100);

                tens = level / 10;

                if (tens > 0) level -= (tens * 10);

                units = level;

                if ((y0 % 30) >= 8 && (y0 % 30) <= 23) {
                    if (hundreds > 0) {
                        if (x >= 11 && x <= 18)
                            if (fontdata[16 * (hundreds + '0') + ((y0 % 30) - 8)] &
                                (128 >> (x - 11)))
                                indx = 255;
                    }

                    if (tens > 0 || hundreds > 0) {
                        if (x >= 19 && x <= 26)
                            if (fontdata[16 * (tens + '0') + ((y0 % 30) - 8)] & (128 >> (x - 19)))
                                indx = 255;
                    }

                    if (x >= 27 && x <= 34)
                        if (fontdata[16 * (units + '0') + ((y0 % 30) - 8)] & (128 >> (x - 27)))
                            indx = 255;

                    if (x >= 42 && x <= 49)
                        if (fontdata[16 * ('d') + ((y0 % 30) - 8)] & (128 >> (x - 42))) indx = 255;

                    if (x >= 50 && x <= 57)
                        if (fontdata[16 * ('B') + ((y0 % 30) - 8)] & (128 >> (x - 50))) indx = 255;
                }

                if (indx > region_.levels)
                    fprintf(fd, "%c%c%c", 0, 0, 0);
                else {
                    red = region_.color[indx][0];
                    green = region_.color[indx][1];
                    blue = region_.color[indx][2];

                    fprintf(fd, "%c%c%c", red, green, blue);
                }
            }
        }

        fclose(fd);
    }

    fprintf(stdout, "Done!\n");
    fflush(stdout);
}
void SplatProcessor::updateCoverageInfo(const char *coverage_name, double latitude, double longitude, double radius) {
    generatedImageInfo_.coverage_name = coverage_name;
    generatedImageInfo_.transmitter_lat = latitude;
    generatedImageInfo_.transmitter_lon = longitude;
    generatedImageInfo_.coverage_radius = radius;
}

void SplatProcessor::updateImageBounds(const char *pngfile, double min_x, double min_y,
                                     double max_x, double max_y, int image_width,
                                     int image_height) {
    //generatedImageInfo.image_file = pngfile;
    generatedImageInfo_.image_size = {.width = image_width, .height = image_height};
    generatedImageInfo_.coordinates = {.north = min_y, .south = max_y, .east = max_x, .west = min_x};
}

// Function to write image info to a text file
void SplatProcessor::write_image_info_to_txt(const std::string &filename, double min_x,
                                             double min_y, double max_x, double max_y,
                                             int image_width, int image_height) {
    // Open the text file for writing
    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write the image information to the text file
    outfile << "Image Coordinates and Size Information\n";
    outfile << "---------------------------------------\n";
    outfile << std::fixed << std::setprecision(6);  // Set precision for floating point numbers
    outfile << "Min Longitude: " << min_x << "\n";
    outfile << "Min Latitude: " << min_y << "\n";
    outfile << "Max Longitude: " << max_x << "\n";
    outfile << "Max Latitude: " << max_y << "\n";
    outfile << "Image Width: " << image_width << " pixels\n";
    outfile << "Image Height: " << image_height << " pixels\n";

    // Close the file
    outfile.close();

    std::cout << "Image information written to " << filename << std::endl;
}

void SplatProcessor::WritePPMSS(char *filename, unsigned char geo, unsigned char kml,
                                unsigned char ngs, struct site *xmtr, unsigned char txsites) {
    /* This function generates a topographic map in Portable Pix Map
       (PPM) format based on the signal strength values held in the
       signal[][] array.  The image created is rotated counter-clockwise
       90 degrees from its representation in dem[][] so that north
       points up and east points right in the image generated. */

    char mapfile[255], geofile[255], kmlfile[255], ckfile[255];
    unsigned width, height, terrain, red, green, blue;
    unsigned char found, mask, cityorcounty;
    int indx, x, y, z = 1, x0, y0, signal, level, hundreds, tens, units, match, colorwidth;
    double conversion, one_over_gamma, lat, lon, north, south, east, west, minwest;
    FILE *fd;

    one_over_gamma = 1.0 / GAMMA;
    conversion = 255.0 / pow((double)(max_elevation_ - min_elevation_), one_over_gamma);

    width = (unsigned)(ippd_ * ReduceAngle(max_west_ - min_west_));
    height = (unsigned)(ippd_ * ReduceAngle(max_north_ - min_north_));

    LoadSignalColors(xmtr[0]);

    if (filename[0] == 0) {
        strncpy(filename, xmtr[0].filename, 254);
        filename[strlen(filename) - 4] = 0; /* Remove .qth */
    }

    y = strlen(filename);

    if (y > 240) y = 240;

    if (y > 4) {
        if (filename[y - 1] == 'm' && filename[y - 2] == 'p' && filename[y - 3] == 'p' &&
            filename[y - 4] == '.')
            y -= 4;
    }

    for (x = 0; x < y; x++) {
        mapfile[x] = filename[x];
        geofile[x] = filename[x];
        kmlfile[x] = filename[x];
        ckfile[x] = filename[x];
    }

    mapfile[x] = '.';
    geofile[x] = '.';
    kmlfile[x] = '.';
    mapfile[x + 1] = 'p';
    geofile[x + 1] = 'g';
    kmlfile[x + 1] = 'k';
    mapfile[x + 2] = 'p';
    geofile[x + 2] = 'e';
    kmlfile[x + 2] = 'm';
    mapfile[x + 3] = 'm';
    geofile[x + 3] = 'o';
    kmlfile[x + 3] = 'l';
    mapfile[x + 4] = 0;
    geofile[x + 4] = 0;
    kmlfile[x + 4] = 0;

    ckfile[x] = '-';
    ckfile[x + 1] = 'c';
    ckfile[x + 2] = 'k';
    ckfile[x + 3] = '.';
    ckfile[x + 4] = 'p';
    ckfile[x + 5] = 'p';
    ckfile[x + 6] = 'm';
    ckfile[x + 7] = 0;

    char pngfile[255];
    char txtfile[255];
    // Generate the .png filename
    strcpy(pngfile, mapfile);
    // change extension of pngfile from .ppm to .png
    strcpy(pngfile + strlen(pngfile) - 4, ".png");

    // change extension of mapfile from .ppm to .png
    strcpy(mapfile + strlen(mapfile) - 4, ".png");

    // Generate the .txt filename
    strcpy(txtfile, pngfile);
    strcpy(txtfile + strlen(txtfile) - 4, ".txt");

    minwest = ((double)min_west_) + dpp_;

    if (minwest > 360.0) minwest -= 360.0;

    north = (double)max_north_ - dpp_;

    if (kml || geo)
        south = (double)min_north_; /* No bottom legend */
    else
        south = (double)min_north_ - (30.0 / ppd_); /* 30 pixels for bottom legend */

    east = (minwest < 180.0 ? -minwest : 360.0 - min_west_);
    west = (double)(max_west_ < 180 ? -max_west_ : 360 - max_west_);

    if (geo && kml == 0) {
        fd = fopen(geofile, "wb");

        fprintf(fd, "FILENAME\t%s\n", mapfile);
        fprintf(fd, "#\t\tX\tY\tLong\t\tLat\n");
        fprintf(fd, "TIEPOINT\t0\t0\t%.3f\t\t%.3f\n", west, north);

        fprintf(fd, "TIEPOINT\t%u\t%u\t%.3f\t\t%.3f\n", width - 1, height - 1, east, south);
        fprintf(fd, "IMAGESIZE\t%u\t%u\n", width, height);

        fprintf(fd, "#\n# Auto Generated by %s v%s\n#\n", splat_name_, splat_version_);

        fclose(fd);
    }

    if (kml && geo == 0) {
        fd = fopen(kmlfile, "wb");

        fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
        fprintf(fd, "<!-- Generated by %s Version %s -->\n", splat_name_, splat_version_);
        fprintf(fd, "  <Folder>\n");
        fprintf(fd, "   <name>%s</name>\n", splat_name_);
        fprintf(fd, "     <description>%s Transmitter Contours</description>\n", xmtr[0].name);
        fprintf(fd, "       <GroundOverlay>\n");
        fprintf(fd, "         <name>SPLAT! Signal Strength Contours</name>\n");
        fprintf(fd, "           <description>SPLAT! Coverage</description>\n");
        fprintf(fd, "		<Icon>\n");
        // fprintf(fd,"              <href>%s</href>\n",mapfile);
        fprintf(fd, "              <href>%s</href>\n", pngfile);
        fprintf(fd, "		</Icon>\n");
        /* fprintf(fd,"            <opacity>128</opacity>\n"); */
        fprintf(fd, "            <LatLonBox>\n");
        fprintf(fd, "               <north>%.5f</north>\n", north);
        fprintf(fd, "               <south>%.5f</south>\n", south);
        fprintf(fd, "               <east>%.5f</east>\n", east);
        fprintf(fd, "               <west>%.5f</west>\n", west);
        fprintf(fd, "               <rotation>0.0</rotation>\n");
        fprintf(fd, "            </LatLonBox>\n");
        fprintf(fd, "       </GroundOverlay>\n");
        fprintf(fd, "       <ScreenOverlay>\n");
        fprintf(fd, "          <name>Color Key</name>\n");
        fprintf(fd, "            <description>Contour Color Key</description>\n");
        fprintf(fd, "          <Icon>\n");
        fprintf(fd, "            <href>%s</href>\n", ckfile);
        fprintf(fd, "          </Icon>\n");
        fprintf(fd,
                "          <overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd,
                "          <screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(
            fd,
            "          <rotationXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd, "          <size x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
        fprintf(fd, "       </ScreenOverlay>\n");

        for (x = 0; x < txsites; x++) {
            fprintf(fd, "     <Placemark>\n");
            fprintf(fd, "       <name>%s</name>\n", xmtr[x].name);
            fprintf(fd, "       <visibility>1</visibility>\n");
            fprintf(fd, "       <Style>\n");
            fprintf(fd, "       <IconStyle>\n");
            fprintf(fd, "        <Icon>\n");
            fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
            fprintf(fd, "          <x>224</x>\n");
            fprintf(fd, "          <y>224</y>\n");
            fprintf(fd, "          <w>32</w>\n");
            fprintf(fd, "          <h>32</h>\n");
            fprintf(fd, "        </Icon>\n");
            fprintf(fd, "       </IconStyle>\n");
            fprintf(fd, "       </Style>\n");
            fprintf(fd, "      <Point>\n");
            fprintf(fd, "        <extrude>1</extrude>\n");
            fprintf(fd, "        <altitudeMode>relativeToGround</altitudeMode>\n");
            fprintf(fd, "        <coordinates>%f,%f,%f</coordinates>\n",
                    (xmtr[x].lon < 180.0 ? -xmtr[x].lon : 360.0 - xmtr[x].lon), xmtr[x].lat,
                    xmtr[x].alt);
            fprintf(fd, "      </Point>\n");
            fprintf(fd, "     </Placemark>\n");
        }

        fprintf(fd, "  </Folder>\n");
        fprintf(fd, "</kml>\n");

        fclose(fd);
    }

    fd = fopen(mapfile, "wb");

    if (kml || geo) {
        /* No bottom legend */

        fprintf(fd, "P6\n%u %u\n255\n", width, height);
        fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height);
    }

    else {
        /* Allow space for bottom legend */

        fprintf(fd, "P6\n%u %u\n255\n", width, height + 30);
        fprintf(stdout, "\nWriting \"%s\" (%ux%u pixmap image)... ", mapfile, width, height + 30);
    }

    fflush(stdout);

    for (y = 0, lat = north; y < (int)height; y++, lat = north - (dpp_ * (double)y)) {
        for (x = 0, lon = max_west_; x < (int)width; x++, lon = max_west_ - (dpp_ * (double)x)) {
            if (lon < 0.0) lon += 360.0;

            for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
                x0 = (int)rint(ppd_ * (lat - (double)dem_[indx].min_north));
                y0 = mpi_ - (int)rint(ppd_ * (LonDiff((double)dem_[indx].max_west, lon)));

                if (x0 >= 0 && x0 <= mpi_ && y0 >= 0 && y0 <= mpi_)
                    found = 1;
                else
                    indx++;
            }

            if (found) {
                mask = dem_[indx].mask[x0][y0];
                signal = (dem_[indx].signal[x0][y0]) - 100;
                cityorcounty = 0;

                match = 255;

                red = 0;
                green = 0;
                blue = 0;

                if (signal >= region_.level[0])
                    match = 0;
                else {
                    for (z = 1; (z < region_.levels && match == 255); z++) {
                        if (signal < region_.level[z - 1] && signal >= region_.level[z]) match = z;
                    }
                }

                if (match < region_.levels) {
                    if (smooth_contours_ && match > 0) {
                        red = (unsigned)interpolate(region_.color[match][0],
                                                    region_.color[match - 1][0], region_.level[match],
                                                    region_.level[match - 1], signal);
                        green = (unsigned)interpolate(
                            region_.color[match][1], region_.color[match - 1][1], region_.level[match],
                            region_.level[match - 1], signal);
                        blue = (unsigned)interpolate(
                            region_.color[match][2], region_.color[match - 1][2], region_.level[match],
                            region_.level[match - 1], signal);
                    }

                    else {
                        red = region_.color[match][0];
                        green = region_.color[match][1];
                        blue = region_.color[match][2];
                    }
                }

                if (mask & 2) {
                    /* Text Labels: Red or otherwise */

                    if (red >= 180 && green <= 75 && blue <= 75)
                        fprintf(fd, "%c%c%c", 255 ^ red, 255 ^ green, 255 ^ blue);
                    else
                        fprintf(fd, "%c%c%c", 255, 0, 0);

                    cityorcounty = 1;
                }

                else if (mask & 4) {
                    /* County Boundaries: Black */

                    fprintf(fd, "%c%c%c", 0, 0, 0);

                    cityorcounty = 1;
                }

                if (cityorcounty == 0) {
                    if (contour_threshold_ != 0 && signal < contour_threshold_) {
                        if (ngs)
                            fprintf(fd, "%c%c%c", 255, 255, 255);
                        else {
                            /* Display land or sea elevation */

                            if (dem_[indx].data[x0][y0] == 0)
                                fprintf(fd, "%c%c%c", 0, 0, 170);
                            else {
                                terrain =
                                    (unsigned)(0.5 +
                                               pow((double)(dem_[indx].data[x0][y0] - min_elevation_),
                                                   one_over_gamma) *
                                                   conversion);
                                fprintf(fd, "%c%c%c", terrain, terrain, terrain);
                            }
                        }
                    }

                    else {
                        /* Plot field strength regions in color */

                        if (red != 0 || green != 0 || blue != 0)
                            fprintf(fd, "%c%c%c", red, green, blue);

                        else /* terrain / sea-level */
                        {
                            if (ngs)
                                fprintf(fd, "%c%c%c", 255, 255, 255);
                            else {
                                if (dem_[indx].data[x0][y0] == 0)
                                    fprintf(fd, "%c%c%c", 0, 0, 170);
                                else {
                                    /* Elevation: Greyscale */
                                    terrain = (unsigned)(0.5 + pow((double)(dem_[indx].data[x0][y0] -
                                                                            min_elevation_),
                                                                   one_over_gamma) *
                                                                   conversion);
                                    fprintf(fd, "%c%c%c", terrain, terrain, terrain);
                                }
                            }
                        }
                    }
                }
            }

            else {
                /* We should never get here, but if */
                /* we do, display the region as black */

                fprintf(fd, "%c%c%c", 0, 0, 0);
            }
        }
    }

    if (kml == 0 && geo == 0) {
        /* Display legend along bottom of image
         * if not generating .kml or .geo output.
         */

        colorwidth = (int)rint((float)width / (float)region_.levels);

        for (y0 = 0; y0 < 30; y0++) {
            for (x0 = 0; x0 < (int)width; x0++) {
                indx = x0 / colorwidth;
                x = x0 % colorwidth;
                level = region_.level[indx];

                hundreds = level / 100;

                if (hundreds > 0) level -= (hundreds * 100);

                tens = level / 10;

                if (tens > 0) level -= (tens * 10);

                units = level;

                if (y0 >= 8 && y0 <= 23) {
                    if (hundreds > 0) {
                        if (x >= 5 && x <= 12)
                            if (fontdata[16 * (hundreds + '0') + (y0 - 8)] & (128 >> (x - 5)))
                                indx = 255;
                    }

                    if (tens > 0 || hundreds > 0) {
                        if (x >= 13 && x <= 20)
                            if (fontdata[16 * (tens + '0') + (y0 - 8)] & (128 >> (x - 13)))
                                indx = 255;
                    }

                    if (x >= 21 && x <= 28)
                        if (fontdata[16 * (units + '0') + (y0 - 8)] & (128 >> (x - 21))) indx = 255;

                    if (x >= 36 && x <= 43)
                        if (fontdata[16 * ('d') + (y0 - 8)] & (128 >> (x - 36))) indx = 255;

                    if (x >= 44 && x <= 51)
                        if (fontdata[16 * ('B') + (y0 - 8)] & (128 >> (x - 44))) indx = 255;

                    if (x >= 52 && x <= 59)
                        if (fontdata[16 * (230) + (y0 - 8)] & (128 >> (x - 52))) indx = 255;

                    if (x >= 60 && x <= 67)
                        if (fontdata[16 * ('V') + (y0 - 8)] & (128 >> (x - 60))) indx = 255;

                    if (x >= 68 && x <= 75)
                        if (fontdata[16 * ('/') + (y0 - 8)] & (128 >> (x - 68))) indx = 255;

                    if (x >= 76 && x <= 83)
                        if (fontdata[16 * ('m') + (y0 - 8)] & (128 >> (x - 76))) indx = 255;
                }

                if (indx > region_.levels)
                    fprintf(fd, "%c%c%c", 0, 0, 0);
                else {
                    red = region_.color[indx][0];
                    green = region_.color[indx][1];
                    blue = region_.color[indx][2];

                    fprintf(fd, "%c%c%c", red, green, blue);
                }
            }
        }
    }

    fclose(fd);

    if (kml) {
        /* Write colorkey image file */

        fd = fopen(ckfile, "wb");

        height = 30 * region_.levels;
        width = 100;

        fprintf(fd, "P6\n%u %u\n255\n", width, height);

        for (y0 = 0; y0 < (int)height; y0++) {
            for (x0 = 0; x0 < (int)width; x0++) {
                indx = y0 / 30;
                x = x0;
                level = region_.level[indx];

                hundreds = level / 100;

                if (hundreds > 0) level -= (hundreds * 100);

                tens = level / 10;

                if (tens > 0) level -= (tens * 10);

                units = level;

                if ((y0 % 30) >= 8 && (y0 % 30) <= 23) {
                    if (hundreds > 0) {
                        if (x >= 5 && x <= 12)
                            if (fontdata[16 * (hundreds + '0') + ((y0 % 30) - 8)] &
                                (128 >> (x - 5)))
                                indx = 255;
                    }

                    if (tens > 0 || hundreds > 0) {
                        if (x >= 13 && x <= 20)
                            if (fontdata[16 * (tens + '0') + ((y0 % 30) - 8)] & (128 >> (x - 13)))
                                indx = 255;
                    }

                    if (x >= 21 && x <= 28)
                        if (fontdata[16 * (units + '0') + ((y0 % 30) - 8)] & (128 >> (x - 21)))
                            indx = 255;

                    if (x >= 36 && x <= 43)
                        if (fontdata[16 * ('d') + ((y0 % 30) - 8)] & (128 >> (x - 36))) indx = 255;

                    if (x >= 44 && x <= 51)
                        if (fontdata[16 * ('B') + ((y0 % 30) - 8)] & (128 >> (x - 44))) indx = 255;

                    if (x >= 52 && x <= 59)
                        if (fontdata[16 * (230) + ((y0 % 30) - 8)] & (128 >> (x - 52))) indx = 255;

                    if (x >= 60 && x <= 67)
                        if (fontdata[16 * ('V') + ((y0 % 30) - 8)] & (128 >> (x - 60))) indx = 255;

                    if (x >= 68 && x <= 75)
                        if (fontdata[16 * ('/') + ((y0 % 30) - 8)] & (128 >> (x - 68))) indx = 255;

                    if (x >= 76 && x <= 83)
                        if (fontdata[16 * ('m') + ((y0 % 30) - 8)] & (128 >> (x - 76))) indx = 255;
                }

                if (indx > region_.levels)
                    fprintf(fd, "%c%c%c", 0, 0, 0);
                else {
                    red = region_.color[indx][0];
                    green = region_.color[indx][1];
                    blue = region_.color[indx][2];

                    fprintf(fd, "%c%c%c", red, green, blue);
                }
            }
        }

        fclose(fd);
    }


    fprintf(stdout, "Done!\n");
    fflush(stdout);
}

void SplatProcessor::WritePPMDBM(char *filename,
                                 unsigned char ngs,
                                 struct site *xmtr, unsigned char txsites)
{
    /* This function generates a topographic map in Portable Pix Map
       (PPM) format based on the signal power level values held in the
       signal[][] array.  The image created is rotated counter-clockwise
       90 degrees from its representation in dem[][] so that north
       points up and east points right in the image generated. */
    char mapfile[255]; 
    unsigned width, height, terrain, red, green, blue;
    unsigned char found, mask, cityorcounty;
    int indx, x, y, z = 1, x0, y0, dBm, level, hundreds, tens, units, match, colorwidth;
    double conversion, one_over_gamma, lat, lon, north, south, east, west, minwest;

    one_over_gamma = 1.0 / GAMMA;
    conversion = 255.0 / pow((double)(max_elevation_ - min_elevation_), one_over_gamma);


    width = (unsigned)(ippd_ * ReduceAngle(max_west_ - min_west_));
    height = (unsigned)(ippd_ * ReduceAngle(max_north_ - min_north_));

    LoadDBMColors();

    if (filename[0] == 0) {
        strncpy(filename, xmtr[0].filename, 254);
        filename[strlen(filename) - 4] = 0; /* Remove .qth */
    }

    y = strlen(filename);

    if (y > 240) y = 240;

    if (y > 4) {
        if (filename[y - 1] == 'm' && filename[y - 2] == 'p' && filename[y - 3] == 'p' &&
            filename[y - 4] == '.')
            y -= 4;
    }

    for (x = 0; x < y; x++) {
        mapfile[x] = filename[x];
    }

    mapfile[x] = '.';
    mapfile[x + 1] = 'p';
    mapfile[x + 2] = 'p';
    mapfile[x + 3] = 'm';
    mapfile[x + 4] = 0;
    
    char pngfile[255];
    // Generate the .png filename
    strcpy(pngfile, mapfile);
    // change extension of pngfile from .ppm to .png
    strcpy(pngfile + strlen(pngfile) - 4, ".png");

    minwest = ((double)min_west_) + dpp_;
    // change extension of mapfile from .ppm to .png
    strcpy(mapfile + strlen(mapfile) - 4, ".png");


    if (minwest > 360.0) minwest -= 360.0;

    north = (double)max_north_ - dpp_;

    south = (double)min_north_;

    east = (minwest < 180.0 ? -minwest : 360.0 - min_west_);
    west = (double)(max_west_ < 180 ? -max_west_ : 360 - max_west_);

    // std::ostringstream oss;
    cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    for (y = 0, lat = north; y < (int)height; y++, lat = north - (dpp_ * (double)y)) {
        for (x = 0, lon = max_west_; x < (int)width; x++, lon = max_west_ - (dpp_ * (double)x)) {
            if (lon < 0.0) lon += 360.0;

            for (indx = 0, found = 0; indx < MAXPAGES && found == 0;) {
                x0 = (int)rint(ppd_ * (lat - (double)dem_[indx].min_north));
                y0 = mpi_ - (int)rint(ppd_ * (LonDiff((double)dem_[indx].max_west, lon)));

                if (x0 >= 0 && x0 <= mpi_ && y0 >= 0 && y0 <= mpi_)
                    found = 1;
                else
                    indx++;
            }

            if (found) {
                mask = dem_[indx].mask[x0][y0];
                dBm = (dem_[indx].signal[x0][y0]) - 200;
                cityorcounty = 0;

                match = 255;

                red = 0;
                green = 0;
                blue = 0;

                if (dBm >= region_.level[0])
                    match = 0;
                else {
                    for (z = 1; (z < region_.levels && match == 255); z++) {
                        if (dBm < region_.level[z - 1] && dBm >= region_.level[z]) match = z;
                    }
                }

                if (match < region_.levels) {
                    if (smooth_contours_ && match > 0) {
                        red = (unsigned)interpolate(region_.color[match][0],
                                                    region_.color[match - 1][0], region_.level[match],
                                                    region_.level[match - 1], dBm);
                        green = (unsigned)interpolate(
                            region_.color[match][1], region_.color[match - 1][1], region_.level[match],
                            region_.level[match - 1], dBm);
                        blue = (unsigned)interpolate(
                            region_.color[match][2], region_.color[match - 1][2], region_.level[match],
                            region_.level[match - 1], dBm);
                    }

                    else {
                        red = region_.color[match][0];
                        green = region_.color[match][1];
                        blue = region_.color[match][2];
                    }
                }

                cv::Vec3b color;

                if (mask & 2) {
                    /* Text Labels: Red or otherwise */
                    if (red >= 180 && green <= 75 && blue <= 75 && dBm != 0) {
                        color[0] = 255 ^ blue;  // OpenCV uses BGR order
                        color[1] = 255 ^ green;
                        color[2] = 255 ^ red;
                    } else {
                        color[0] = 0;    // Blue component
                        color[1] = 0;    // Green component
                        color[2] = 255;  // Red component
                    }
                    cityorcounty = 1;
                } else if (mask & 4) {
                    /* County Boundaries: Black */
                    color[0] = 0;
                    color[1] = 0;
                    color[2] = 0;
                    cityorcounty = 1;
                }

                if (cityorcounty == 0) {
                    if (contour_threshold_ != 0 && dBm < contour_threshold_) {
                        if (transparent_mode_) {
                            /* Transparent background */
                            color[0] = 0;
                            color[1] = 0;
                            color[2] = 0;
                        } else if (ngs) {
                            /* No terrain */
                            color[0] = 255;
                            color[1] = 255;
                            color[2] = 255;
                        } else {
                            if (dem_[indx].data[x0][y0] == 0) {
                                /* Sea level */
                                color[0] = 170;  // Blue
                                color[1] = 0;    // Green
                                color[2] = 0;    // Red
                            } else {
                                /* Terrain: Greyscale */
                                terrain =
                                    (unsigned)(0.5 +
                                               pow((double)(dem_[indx].data[x0][y0] - min_elevation_),
                                                   one_over_gamma) *
                                                   conversion);
                                color[0] = terrain;
                                color[1] = terrain;
                                color[2] = terrain;
                            }
                        }
                    } else {
                        /* Plot signal power level regions in color */
                        if (red != 0 || green != 0 || blue != 0) {
                            color[0] = blue;  // BGR order in OpenCV
                            color[1] = green;
                            color[2] = red;
                        } else {
                            /* terrain / sea-level */
                            if (transparent_mode_) {
                                /* Transparent background */
                                color[0] = 0;
                                color[1] = 0;
                                color[2] = 0;
                            } else if (ngs) {
                                color[0] = 255;
                                color[1] = 255;
                                color[2] = 255;
                            } else {
                                if (dem_[indx].data[x0][y0] == 0) {
                                    /* Sea level */
                                    color[0] = 170;  // Blue
                                    color[1] = 0;    // Green
                                    color[2] = 0;    // Red
                                } else {
                                    /* Elevation: Greyscale */
                                    terrain = (unsigned)(0.5 + pow((double)(dem_[indx].data[x0][y0] -
                                                                            min_elevation_),
                                                                   one_over_gamma) *
                                                                   conversion);
                                    color[0] = terrain;
                                    color[1] = terrain;
                                    color[2] = terrain;
                                }
                            }
                        }
                    }
                }
                image.at<cv::Vec3b>(y, x) = color;
            } else {
                /* We should never get here, but if */
                /* we do, display the region as black */
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
        }
    }

     image_ = image.clone();
     cv::imwrite("ddd.png", image_);

     // Write the data to a struct
     updateImageBounds(pngfile, west, north, east, south, width, height);

     fprintf(stdout, "✅ Splat Processing Complete! ===\n");
     fflush(stdout);
}

void SplatProcessor::GraphTerrain(struct site source, struct site destination, char *name) {
    /* This function invokes gnuplot to generate an appropriate
       output file indicating the terrain profile between the source
       and destination locations when the -p command line option
       is used.  "basename" is the name assigned to the output
       file generated by gnuplot.  The filename extension is used
       to set gnuplot's terminal setting and output file type.
       If no extension is found, .png is assumed.  */

    int x, y, z;
    char basename[255], term[30], ext[15];
    double minheight = 100000.0, maxheight = -100000.0;
    FILE *fd = NULL, *fd1 = NULL;

    ReadPath(destination, source);

    fd = fopen("profile.gp", "wb");

    if (clutter_ > 0.0) fd1 = fopen("clutter.gp", "wb");

    for (x = 0; x < path_.length; x++) {
        if ((path_.elevation[x] + clutter_) > maxheight) maxheight = path_.elevation[x] + clutter_;

        if (path_.elevation[x] < minheight) minheight = path_.elevation[x];

        if (metric_) {
            fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[x],
                    METERS_PER_FOOT * path_.elevation[x]);

            if (fd1 != NULL && x > 0 && x < path_.length - 2)
                fprintf(
                    fd1, "%f\t%f\n", KM_PER_MILE * path_.distance[x],
                    METERS_PER_FOOT * (path_.elevation[x] == 0.0 ? path_.elevation[x]
                                                                : (path_.elevation[x] + clutter_)));
        }

        else {
            fprintf(fd, "%f\t%f\n", path_.distance[x], path_.elevation[x]);

            if (fd1 != NULL && x > 0 && x < path_.length - 2)
                fprintf(
                    fd1, "%f\t%f\n", path_.distance[x],
                    (path_.elevation[x] == 0.0 ? path_.elevation[x] : (path_.elevation[x] + clutter_)));
        }
    }

    fclose(fd);

    if (fd1 != NULL) fclose(fd1);

    if (name[0] == '.') {
        /* Default filename and output file type */

        strncpy(basename, "profile\0", 8);
        strncpy(term, "png\0", 4);
        strncpy(ext, "png\0", 4);
    }

    else {
        /* Extract extension and terminal type from "name" */

        ext[0] = 0;
        y = strlen(name);
        strncpy(basename, name, 254);

        for (x = y - 1; x > 0 && name[x] != '.'; x--);

        if (x > 0) /* Extension found */
        {
            for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++) {
                ext[z - (x + 1)] = tolower(name[z]);
                term[z - (x + 1)] = name[z];
            }

            ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
            term[z - (x + 1)] = 0;
            basename[x] = 0;
        }

        if (ext[0] == 0) /* No extension -- Default is png */
        {
            strncpy(term, "png\0", 4);
            strncpy(ext, "png\0", 4);
        }
    }

    /* Either .ps or .postscript may be used
       as an extension for postscript output. */

    if (strncmp(term, "postscript", 10) == 0)
        strncpy(ext, "ps\0", 3);

    else if (strncmp(ext, "ps", 2) == 0)
        strncpy(term, "postscript enhanced color\0", 26);

    if (maxheight < 1.0) {
        maxheight = 1.0;  /* Avoid a gnuplot y-range error */
        minheight = -1.0; /* over a completely sea-level path */
    }

    else
        minheight -= (0.01 * maxheight);

    fd = fopen("splat.gp", "w");
    fprintf(fd, "set grid\n");
    fprintf(fd, "set yrange [%2.3f to %2.3f]\n", metric_ ? minheight * METERS_PER_FOOT : minheight,
            metric_ ? maxheight * METERS_PER_FOOT : maxheight);
    fprintf(fd, "set encoding iso_8859_1\n");
    fprintf(fd, "set term %s\n", term);
    fprintf(fd, "set title \"%s Terrain Profile Between %s and %s (%.2f%c Azimuth)\"\n", splat_name_,
            destination.name, source.name, Azimuth(destination, source), 176);

    if (metric_) {
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n",
                destination.name, source.name, KM_PER_MILE * Distance(source, destination));
        fprintf(fd, "set ylabel \"Ground Elevation Above Sea Level (meters)\"\n");
    }

    else {
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name,
                source.name, Distance(source, destination));
        fprintf(fd, "set ylabel \"Ground Elevation Above Sea Level (feet)\"\n");
    }

    fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

    if (clutter_ > 0.0) {
        if (metric_)
            fprintf(fd,
                    "plot \"profile.gp\" title \"Terrain Profile\" with lines, \"clutter.gp\" "
                    "title \"Clutter Profile (%.2f meters)\" with lines\n",
                    clutter_ * METERS_PER_FOOT);
        else
            fprintf(fd,
                    "plot \"profile.gp\" title \"Terrain Profile\" with lines, \"clutter.gp\" "
                    "title \"Clutter Profile (%.2f feet)\" with lines\n",
                    clutter_);
    }

    else
        fprintf(fd, "plot \"profile.gp\" title \"\" with lines\n");

    fclose(fd);

    x = system("gnuplot splat.gp");

    if (x != -1) {
        if (gpsav_ == 0) {
            unlink("splat.gp");
            unlink("profile.gp");
        }

        fprintf(stdout, "Terrain plot written to: \"%s.%s\"\n", basename, ext);
        fflush(stdout);
    }

    else
        fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot!\n");
}

void SplatProcessor::GraphElevation(struct site source, struct site destination, char *name) {
    /* This function invokes gnuplot to generate an appropriate
       output file indicating the terrain elevation profile between
       the source and destination locations when the -e command line
       option is used.  "basename" is the name assigned to the output
       file generated by gnuplot.  The filename extension is used
       to set gnuplot's terminal setting and output file type.
       If no extension is found, .png is assumed.  */

    int x, y, z;
    char basename[255], term[30], ext[15];
    double angle, clutter_angle = 0.0, refangle, maxangle = -90.0, minangle = 90.0, distance;
    struct site remote, remote2;
    FILE *fd = NULL, *fd1 = NULL, *fd2 = NULL;

    ReadPath(destination, source); /* destination=RX, source=TX */
    refangle = ElevationAngle(destination, source);
    distance = Distance(source, destination);

    fd = fopen("profile.gp", "wb");

    if (clutter_ > 0.0) fd1 = fopen("clutter.gp", "wb");

    fd2 = fopen("reference.gp", "wb");

    for (x = 1; x < path_.length - 1; x++) {
        remote.lat = path_.lat[x];
        remote.lon = path_.lon[x];
        remote.alt = 0.0;
        angle = ElevationAngle(destination, remote);

        if (clutter_ > 0.0) {
            remote2.lat = path_.lat[x];
            remote2.lon = path_.lon[x];

            if (path_.elevation[x] != 0.0)
                remote2.alt = clutter_;
            else
                remote2.alt = 0.0;

            clutter_angle = ElevationAngle(destination, remote2);
        }

        if (metric_) {
            fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[x], angle);

            if (fd1 != NULL)
                fprintf(fd1, "%f\t%f\n", KM_PER_MILE * path_.distance[x], clutter_angle);

            fprintf(fd2, "%f\t%f\n", KM_PER_MILE * path_.distance[x], refangle);
        }

        else {
            fprintf(fd, "%f\t%f\n", path_.distance[x], angle);

            if (fd1 != NULL) fprintf(fd1, "%f\t%f\n", path_.distance[x], clutter_angle);

            fprintf(fd2, "%f\t%f\n", path_.distance[x], refangle);
        }

        if (angle > maxangle) maxangle = angle;

        if (clutter_angle > maxangle) maxangle = clutter_angle;

        if (angle < minangle) minangle = angle;
    }

    if (metric_) {
        fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1], refangle);
        fprintf(fd2, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1], refangle);
    }

    else {
        fprintf(fd, "%f\t%f\n", path_.distance[path_.length - 1], refangle);
        fprintf(fd2, "%f\t%f\n", path_.distance[path_.length - 1], refangle);
    }

    fclose(fd);

    if (fd1 != NULL) fclose(fd1);

    fclose(fd2);

    if (name[0] == '.') {
        /* Default filename and output file type */

        strncpy(basename, "profile\0", 8);
        strncpy(term, "png\0", 4);
        strncpy(ext, "png\0", 4);
    }

    else {
        /* Extract extension and terminal type from "name" */

        ext[0] = 0;
        y = strlen(name);
        strncpy(basename, name, 254);

        for (x = y - 1; x > 0 && name[x] != '.'; x--);

        if (x > 0) /* Extension found */
        {
            for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++) {
                ext[z - (x + 1)] = tolower(name[z]);
                term[z - (x + 1)] = name[z];
            }

            ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
            term[z - (x + 1)] = 0;
            basename[x] = 0;
        }

        if (ext[0] == 0) /* No extension -- Default is png */
        {
            strncpy(term, "png\0", 4);
            strncpy(ext, "png\0", 4);
        }
    }

    /* Either .ps or .postscript may be used
       as an extension for postscript output. */

    if (strncmp(term, "postscript", 10) == 0)
        strncpy(ext, "ps\0", 3);

    else if (strncmp(ext, "ps", 2) == 0)
        strncpy(term, "postscript enhanced color\0", 26);

    fd = fopen("splat.gp", "w");

    fprintf(fd, "set grid\n");

    if (distance > 2.0)
        fprintf(fd, "set yrange [%2.3f to %2.3f]\n", (-fabs(refangle) - 0.25), maxangle + 0.25);
    else
        fprintf(fd, "set yrange [%2.3f to %2.3f]\n", minangle, refangle + (-minangle / 8.0));

    fprintf(fd, "set encoding iso_8859_1\n");
    fprintf(fd, "set term %s\n", term);
    fprintf(fd, "set title \"%s Elevation Profile Between %s and %s (%.2f%c azimuth)\"\n",
            splat_name_, destination.name, source.name, Azimuth(destination, source), 176);

    if (metric_)
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n",
                destination.name, source.name, KM_PER_MILE * distance);
    else
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name,
                source.name, distance);

    fprintf(fd, "set ylabel \"Elevation Angle Along LOS Path Between\\n%s and %s (degrees)\"\n",
            destination.name, source.name);
    fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

    if (clutter_ > 0.0) {
        if (metric_)
            fprintf(fd,
                    "plot \"profile.gp\" title \"Real Earth Profile\" with lines, \"clutter.gp\" "
                    "title \"Clutter Profile (%.2f meters)\" with lines, \"reference.gp\" title "
                    "\"Line of Sight Path (%.2f%c elevation)\" with lines\n",
                    clutter_ * METERS_PER_FOOT, refangle, 176);
        else
            fprintf(fd,
                    "plot \"profile.gp\" title \"Real Earth Profile\" with lines, \"clutter.gp\" "
                    "title \"Clutter Profile (%.2f feet)\" with lines, \"reference.gp\" title "
                    "\"Line of Sight Path (%.2f%c elevation)\" with lines\n",
                    clutter_, refangle, 176);
    }

    else
        fprintf(fd,
                "plot \"profile.gp\" title \"Real Earth Profile\" with lines, \"reference.gp\" "
                "title \"Line of Sight Path (%.2f%c elevation)\" with lines\n",
                refangle, 176);

    fclose(fd);

    x = system("gnuplot splat.gp");

    if (x != -1) {
        if (gpsav_ == 0) {
            unlink("splat.gp");
            unlink("profile.gp");
            unlink("reference.gp");

            if (clutter_ > 0.0) unlink("clutter.gp");
        }

        fprintf(stdout, "Elevation plot written to: \"%s.%s\"\n", basename, ext);
        fflush(stdout);
    }

    else
        fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot!\n");
}

void SplatProcessor::GraphHeight(struct site source, struct site destination, char *name,
                                 unsigned char fresnel_plot, unsigned char normalized) {
    /* This function invokes gnuplot to generate an appropriate
       output file indicating the terrain height profile between
       the source and destination locations referenced to the
       line-of-sight path between the receive and transmit sites
       when the -h or -H command line option is used.  "basename"
       is the name assigned to the output file generated by gnuplot.
       The filename extension is used to set gnuplot's terminal
       setting and output file type.  If no extension is found,
       .png is assumed.  */

    int x, y, z;
    char basename[255], term[30], ext[15];
    double a, b, c, height = 0.0, refangle, cangle, maxheight = -100000.0, minheight = 100000.0,
                    lambda = 0.0, f_zone = 0.0, fpt6_zone = 0.0, nm = 0.0, nb = 0.0, ed = 0.0,
                    es = 0.0, r = 0.0, d = 0.0, d1 = 0.0, terrain, azimuth, distance, dheight = 0.0,
                    minterrain = 100000.0, minearth = 100000.0, miny, maxy, min2y, max2y;
    struct site remote;
    FILE *fd = NULL, *fd1 = NULL, *fd2 = NULL, *fd3 = NULL, *fd4 = NULL, *fd5 = NULL;

    ReadPath(destination, source); /* destination=RX, source=TX */
    azimuth = Azimuth(destination, source);
    distance = Distance(destination, source);
    refangle = ElevationAngle(destination, source);
    b = GetElevation(destination) + destination.alt + earthradius_;

    /* Wavelength and path distance (great circle) in feet. */

    if (fresnel_plot) {
        lambda = 9.8425e8 / (LR_.frq_mhz * 1e6);
        d = 5280.0 * path_.distance[path_.length - 1];
    }

    if (normalized) {
        ed = GetElevation(destination);
        es = GetElevation(source);
        nb = -destination.alt - ed;
        nm = (-source.alt - es - nb) / (path_.distance[path_.length - 1]);
    }

    fd = fopen("profile.gp", "wb");

    if (clutter_ > 0.0) fd1 = fopen("clutter.gp", "wb");

    fd2 = fopen("reference.gp", "wb");
    fd5 = fopen("curvature.gp", "wb");

    if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
        fd3 = fopen("fresnel.gp", "wb");
        fd4 = fopen("fresnel_pt_6.gp", "wb");
    }

    for (x = 0; x < path_.length - 1; x++) {
        remote.lat = path_.lat[x];
        remote.lon = path_.lon[x];
        remote.alt = 0.0;

        terrain = GetElevation(remote);

        if (x == 0) terrain += destination.alt; /* RX antenna spike */

        a = terrain + earthradius_;
        cangle = 5280.0 * Distance(destination, remote) / earthradius_;
        c = b * sin(refangle * DEG2RAD + HALFPI) / sin(HALFPI - refangle * DEG2RAD - cangle);

        height = a - c;

        /* Per Fink and Christiansen, Electronics
         * Engineers' Handbook, 1989:
         *
         *   H = sqrt(lamba * d1 * (d - d1)/d)
         *
         * where H is the distance from the LOS
         * path to the first Fresnel zone boundary.
         */

        if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
            d1 = 5280.0 * path_.distance[x];
            f_zone = -1.0 * sqrt(lambda * d1 * (d - d1) / d);
            fpt6_zone = f_zone * fzone_clearance_;
        }

        if (normalized) {
            r = -(nm * path_.distance[x]) - nb;
            height += r;

            if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
                f_zone += r;
                fpt6_zone += r;
            }
        }

        else
            r = 0.0;

        if (metric_) {
            fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[x], METERS_PER_FOOT * height);

            if (fd1 != NULL && x > 0 && x < path_.length - 2)
                fprintf(fd1, "%f\t%f\n", KM_PER_MILE * path_.distance[x],
                        METERS_PER_FOOT * (terrain == 0.0 ? height : (height + clutter_)));

            fprintf(fd2, "%f\t%f\n", KM_PER_MILE * path_.distance[x], METERS_PER_FOOT * r);
            fprintf(fd5, "%f\t%f\n", KM_PER_MILE * path_.distance[x],
                    METERS_PER_FOOT * (height - terrain));
        }

        else {
            fprintf(fd, "%f\t%f\n", path_.distance[x], height);

            if (fd1 != NULL && x > 0 && x < path_.length - 2)
                fprintf(fd1, "%f\t%f\n", path_.distance[x],
                        (terrain == 0.0 ? height : (height + clutter_)));

            fprintf(fd2, "%f\t%f\n", path_.distance[x], r);
            fprintf(fd5, "%f\t%f\n", path_.distance[x], height - terrain);
        }

        if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
            if (metric_) {
                fprintf(fd3, "%f\t%f\n", KM_PER_MILE * path_.distance[x], METERS_PER_FOOT * f_zone);
                fprintf(fd4, "%f\t%f\n", KM_PER_MILE * path_.distance[x],
                        METERS_PER_FOOT * fpt6_zone);
            }

            else {
                fprintf(fd3, "%f\t%f\n", path_.distance[x], f_zone);
                fprintf(fd4, "%f\t%f\n", path_.distance[x], fpt6_zone);
            }

            if (f_zone < minheight) minheight = f_zone;
        }

        if ((height + clutter_) > maxheight) maxheight = height + clutter_;

        if (height < minheight) minheight = height;

        if (r > maxheight) maxheight = r;

        if (terrain < minterrain) minterrain = terrain;

        if ((height - terrain) < minearth) minearth = height - terrain;
    }

    if (normalized)
        r = -(nm * path_.distance[path_.length - 1]) - nb;
    else
        r = 0.0;

    if (metric_) {
        fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1], METERS_PER_FOOT * r);
        fprintf(fd2, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1], METERS_PER_FOOT * r);
    }

    else {
        fprintf(fd, "%f\t%f\n", path_.distance[path_.length - 1], r);
        fprintf(fd2, "%f\t%f\n", path_.distance[path_.length - 1], r);
    }

    if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
        if (metric_) {
            fprintf(fd3, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1],
                    METERS_PER_FOOT * r);
            fprintf(fd4, "%f\t%f\n", KM_PER_MILE * path_.distance[path_.length - 1],
                    METERS_PER_FOOT * r);
        }

        else {
            fprintf(fd3, "%f\t%f\n", path_.distance[path_.length - 1], r);
            fprintf(fd4, "%f\t%f\n", path_.distance[path_.length - 1], r);
        }
    }

    if (r > maxheight) maxheight = r;

    if (r < minheight) minheight = r;

    fclose(fd);

    if (fd1 != NULL) fclose(fd1);

    fclose(fd2);
    fclose(fd5);

    if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
        fclose(fd3);
        fclose(fd4);
    }

    if (name[0] == '.') {
        /* Default filename and output file type */

        strncpy(basename, "profile\0", 8);
        strncpy(term, "png\0", 4);
        strncpy(ext, "png\0", 4);
    }

    else {
        /* Extract extension and terminal type from "name" */

        ext[0] = 0;
        y = strlen(name);
        strncpy(basename, name, 254);

        for (x = y - 1; x > 0 && name[x] != '.'; x--);

        if (x > 0) /* Extension found */
        {
            for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++) {
                ext[z - (x + 1)] = tolower(name[z]);
                term[z - (x + 1)] = name[z];
            }

            ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
            term[z - (x + 1)] = 0;
            basename[x] = 0;
        }

        if (ext[0] == 0) /* No extension -- Default is png */
        {
            strncpy(term, "png\0", 4);
            strncpy(ext, "png\0", 4);
        }
    }

    /* Either .ps or .postscript may be used
       as an extension for postscript output. */

    if (strncmp(term, "postscript", 10) == 0)
        strncpy(ext, "ps\0", 3);

    else if (strncmp(ext, "ps", 2) == 0)
        strncpy(term, "postscript enhanced color\0", 26);

    fd = fopen("splat.gp", "w");

    dheight = maxheight - minheight;
    miny = minheight - 0.15 * dheight;
    maxy = maxheight + 0.05 * dheight;

    if (maxy < 20.0) maxy = 20.0;

    dheight = maxheight - minheight;
    min2y = miny - minterrain + 0.05 * dheight;

    if (minearth < min2y) {
        miny -= min2y - minearth + 0.05 * dheight;
        min2y = minearth - 0.05 * dheight;
    }

    max2y = min2y + maxy - miny;

    fprintf(fd, "set grid\n");
    fprintf(fd, "set yrange [%2.3f to %2.3f]\n", metric_ ? miny * METERS_PER_FOOT : miny,
            metric_ ? maxy * METERS_PER_FOOT : maxy);
    fprintf(fd, "set y2range [%2.3f to %2.3f]\n", metric_ ? min2y * METERS_PER_FOOT : min2y,
            metric_ ? max2y * METERS_PER_FOOT : max2y);
    fprintf(fd, "set xrange [-0.5 to %2.3f]\n",
            metric_ ? KM_PER_MILE * rint(distance + 0.5) : rint(distance + 0.5));
    fprintf(fd, "set encoding iso_8859_1\n");
    fprintf(fd, "set term %s\n", term);

    if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot)
        fprintf(fd,
                "set title \"%s Path Profile Between %s and %s (%.2f%c azimuth)\\nWith First "
                "Fresnel Zone\"\n",
                splat_name_, destination.name, source.name, azimuth, 176);

    else
        fprintf(fd, "set title \"%s Height Profile Between %s and %s (%.2f%c azimuth)\"\n",
                splat_name_, destination.name, source.name, azimuth, 176);

    if (metric_)
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n",
                destination.name, source.name, KM_PER_MILE * Distance(source, destination));
    else
        fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n", destination.name,
                source.name, Distance(source, destination));

    if (normalized) {
        if (metric_)
            fprintf(fd,
                    "set ylabel \"Normalized Height Referenced To LOS Path Between\\n%s and %s "
                    "(meters)\"\n",
                    destination.name, source.name);

        else
            fprintf(fd,
                    "set ylabel \"Normalized Height Referenced To LOS Path Between\\n%s and %s "
                    "(feet)\"\n",
                    destination.name, source.name);

    }

    else {
        if (metric_)
            fprintf(fd,
                    "set ylabel \"Height Referenced To LOS Path Between\\n%s and %s (meters)\"\n",
                    destination.name, source.name);

        else
            fprintf(fd, "set ylabel \"Height Referenced To LOS Path Between\\n%s and %s (feet)\"\n",
                    destination.name, source.name);
    }

    fprintf(fd, "set output \"%s.%s\"\n", basename, ext);

    if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
        if (clutter_ > 0.0) {
            if (metric_)
                fprintf(
                    fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"clutter.gp\" title \"Ground Clutter (%.2f meters)\" with lines, "
                    "\"reference.gp\" title \"Line of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines, \"fresnel.gp\" axes "
                    "x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"fresnel_pt_6.gp\" "
                    "title \"%.0f%% of First Fresnel Zone\" with lines\n",
                    clutter_ * METERS_PER_FOOT, LR_.frq_mhz, fzone_clearance_ * 100.0);
            else
                fprintf(
                    fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"clutter.gp\" title \"Ground Clutter (%.2f feet)\" with lines, "
                    "\"reference.gp\" title \"Line of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines, \"fresnel.gp\" axes "
                    "x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"fresnel_pt_6.gp\" "
                    "title \"%.0f%% of First Fresnel Zone\" with lines\n",
                    clutter_, LR_.frq_mhz, fzone_clearance_ * 100.0);
        }

        else
            fprintf(fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"reference.gp\" title \"Line of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines, \"fresnel.gp\" axes "
                    "x1y1 title \"First Fresnel Zone (%.3f MHz)\" with lines, \"fresnel_pt_6.gp\" "
                    "title \"%.0f%% of First Fresnel Zone\" with lines\n",
                    LR_.frq_mhz, fzone_clearance_ * 100.0);
    }

    else {
        if (clutter_ > 0.0) {
            if (metric_)
                fprintf(
                    fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"clutter.gp\" title \"Ground Clutter (%.2f meters)\" with lines, "
                    "\"reference.gp\" title \"Line Of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines\n",
                    clutter_ * METERS_PER_FOOT);
            else
                fprintf(
                    fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"clutter.gp\" title \"Ground Clutter (%.2f feet)\" with lines, "
                    "\"reference.gp\" title \"Line Of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines\n",
                    clutter_);
        }

        else
            fprintf(fd,
                    "plot \"profile.gp\" title \"Point-to-Point Profile\" with lines, "
                    "\"reference.gp\" title \"Line Of Sight Path\" with lines, \"curvature.gp\" "
                    "axes x1y2 title \"Earth's Curvature Contour\" with lines\n");
    }

    fclose(fd);

    x = system("gnuplot splat.gp");

    if (x != -1) {
        if (gpsav_ == 0) {
            unlink("splat.gp");
            unlink("profile.gp");
            unlink("reference.gp");
            unlink("curvature.gp");

            if (fd1 != NULL) unlink("clutter.gp");

            if ((LR_.frq_mhz >= 20.0) && (LR_.frq_mhz <= 20000.0) && fresnel_plot) {
                unlink("fresnel.gp");
                unlink("fresnel_pt_6.gp");
            }
        }

        fprintf(stdout, "\nHeight plot written to: \"%s.%s\"", basename, ext);
        fflush(stdout);
    }

    else
        fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot!\n");
}

void SplatProcessor::ObstructionAnalysis(struct site xmtr, struct site rcvr, double f,
                                         FILE *outfile) {
    /* Perform an obstruction analysis along the
       path between receiver and transmitter. */

    int x;
    struct site site_x;
    double h_r, h_t, h_x, h_r_orig, cos_tx_angle, cos_test_angle, cos_tx_angle_f1,
        cos_tx_angle_fpt6, d_tx, d_x, h_r_f1, h_r_fpt6, h_f, h_los, lambda = 0.0;
    char string[255], string_fpt6[255], string_f1[255];

    ReadPath(xmtr, rcvr);
    h_r = GetElevation(rcvr) + rcvr.alt + earthradius_;
    h_r_f1 = h_r;
    h_r_fpt6 = h_r;
    h_r_orig = h_r;
    h_t = GetElevation(xmtr) + xmtr.alt + earthradius_;
    d_tx = 5280.0 * Distance(rcvr, xmtr);
    cos_tx_angle = ((h_r * h_r) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r * d_tx);
    cos_tx_angle_f1 = cos_tx_angle;
    cos_tx_angle_fpt6 = cos_tx_angle;

    if (f) lambda = 9.8425e8 / (f * 1e6);

    if (clutter_ > 0.0) {
        fprintf(outfile, "Terrain has been raised by");

        if (metric_)
            fprintf(outfile, " %.2f meters", METERS_PER_FOOT * clutter_);
        else
            fprintf(outfile, " %.2f feet", clutter_);

        fprintf(outfile, " to account for ground clutter.\n\n");
    }

    /* At each point along the path calculate the cosine
       of a sort of "inverse elevation angle" at the receiver.
       From the antenna, 0 deg. looks at the ground, and 90 deg.
       is parallel to the ground.

       Start at the receiver.  If this is the lowest antenna,
       then terrain obstructions will be nearest to it.  (Plus,
       that's the way SPLAT!'s original los() did it.)

       Calculate cosines only.  That's sufficient to compare
       angles and it saves the extra computational burden of
       acos().  However, note the inverted comparison: if
       acos(A) > acos(B), then B > A. */

    for (x = path_.length - 1; x > 0; x--) {
        site_x.lat = path_.lat[x];
        site_x.lon = path_.lon[x];
        site_x.alt = 0.0;

        h_x = GetElevation(site_x) + earthradius_ + clutter_;
        d_x = 5280.0 * Distance(rcvr, site_x);

        /* Deal with the LOS path first. */

        cos_test_angle = ((h_r * h_r) + (d_x * d_x) - (h_x * h_x)) / (2.0 * h_r * d_x);

        if (cos_tx_angle > cos_test_angle) {
            if (h_r == h_r_orig)
                fprintf(outfile, "Between %s and %s, %s detected obstructions at:\n\n", rcvr.name,
                        xmtr.name, splat_name_);

            if (site_x.lat >= 0.0) {
                if (metric_)
                    fprintf(outfile, "   %8.4f N,%9.4f W, %5.2f kilometers, %6.2f meters AMSL\n",
                            site_x.lat, site_x.lon, KM_PER_MILE * (d_x / 5280.0),
                            METERS_PER_FOOT * (h_x - earthradius_));
                else
                    fprintf(outfile, "   %8.4f N,%9.4f W, %5.2f miles, %6.2f feet AMSL\n",
                            site_x.lat, site_x.lon, d_x / 5280.0, h_x - earthradius_);
            }

            else {
                if (metric_)
                    fprintf(outfile, "   %8.4f S,%9.4f W, %5.2f kilometers, %6.2f meters AMSL\n",
                            -site_x.lat, site_x.lon, KM_PER_MILE * (d_x / 5280.0),
                            METERS_PER_FOOT * (h_x - earthradius_));
                else

                    fprintf(outfile, "   %8.4f S,%9.4f W, %5.2f miles, %6.2f feet AMSL\n",
                            -site_x.lat, site_x.lon, d_x / 5280.0, h_x - earthradius_);
            }
        }

        while (cos_tx_angle > cos_test_angle) {
            h_r += 1;
            cos_test_angle = ((h_r * h_r) + (d_x * d_x) - (h_x * h_x)) / (2.0 * h_r * d_x);
            cos_tx_angle = ((h_r * h_r) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r * d_tx);
        }

        if (f) {
            /* Now clear the first Fresnel zone... */

            cos_tx_angle_f1 =
                ((h_r_f1 * h_r_f1) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r_f1 * d_tx);
            h_los = sqrt(h_r_f1 * h_r_f1 + d_x * d_x - 2 * h_r_f1 * d_x * cos_tx_angle_f1);
            h_f = h_los - sqrt(lambda * d_x * (d_tx - d_x) / d_tx);

            while (h_f < h_x) {
                h_r_f1 += 1;
                cos_tx_angle_f1 =
                    ((h_r_f1 * h_r_f1) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r_f1 * d_tx);
                h_los = sqrt(h_r_f1 * h_r_f1 + d_x * d_x - 2 * h_r_f1 * d_x * cos_tx_angle_f1);
                h_f = h_los - sqrt(lambda * d_x * (d_tx - d_x) / d_tx);
            }

            /* and clear the 60% F1 zone. */

            cos_tx_angle_fpt6 =
                ((h_r_fpt6 * h_r_fpt6) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r_fpt6 * d_tx);
            h_los = sqrt(h_r_fpt6 * h_r_fpt6 + d_x * d_x - 2 * h_r_fpt6 * d_x * cos_tx_angle_fpt6);
            h_f = h_los - fzone_clearance_ * sqrt(lambda * d_x * (d_tx - d_x) / d_tx);

            while (h_f < h_x) {
                h_r_fpt6 += 1;
                cos_tx_angle_fpt6 =
                    ((h_r_fpt6 * h_r_fpt6) + (d_tx * d_tx) - (h_t * h_t)) / (2.0 * h_r_fpt6 * d_tx);
                h_los =
                    sqrt(h_r_fpt6 * h_r_fpt6 + d_x * d_x - 2 * h_r_fpt6 * d_x * cos_tx_angle_fpt6);
                h_f = h_los - fzone_clearance_ * sqrt(lambda * d_x * (d_tx - d_x) / d_tx);
            }
        }
    }

    if (h_r > h_r_orig) {
        if (metric_)
            snprintf(string, 150,
                     "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear all "
                     "obstructions detected by %s.\n",
                     rcvr.name, METERS_PER_FOOT * (h_r - GetElevation(rcvr) - earthradius_),
                     splat_name_);
        else
            snprintf(string, 150,
                     "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear all "
                     "obstructions detected by %s.\n",
                     rcvr.name, h_r - GetElevation(rcvr) - earthradius_, splat_name_);
    }

    else
        snprintf(string, 150, "\nNo obstructions to LOS path due to terrain were detected by %s\n",
                 splat_name_);

    if (f) {
        if (h_r_fpt6 > h_r_orig) {
            if (metric_)
                snprintf(string_fpt6, 150,
                         "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear "
                         "%.0f%c of the first Fresnel zone.\n",
                         rcvr.name, METERS_PER_FOOT * (h_r_fpt6 - GetElevation(rcvr) - earthradius_),
                         fzone_clearance_ * 100.0, 37);

            else
                snprintf(string_fpt6, 150,
                         "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear "
                         "%.0f%c of the first Fresnel zone.\n",
                         rcvr.name, h_r_fpt6 - GetElevation(rcvr) - earthradius_,
                         fzone_clearance_ * 100.0, 37);
        }

        else
            snprintf(string_fpt6, 150, "\n%.0f%c of the first Fresnel zone is clear.\n",
                     fzone_clearance_ * 100.0, 37);

        if (h_r_f1 > h_r_orig) {
            if (metric_)
                snprintf(string_f1, 150,
                         "\nAntenna at %s must be raised to at least %.2f meters AGL\nto clear the "
                         "first Fresnel zone.\n",
                         rcvr.name, METERS_PER_FOOT * (h_r_f1 - GetElevation(rcvr) - earthradius_));

            else
                snprintf(string_f1, 150,
                         "\nAntenna at %s must be raised to at least %.2f feet AGL\nto clear the "
                         "first Fresnel zone.\n",
                         rcvr.name, h_r_f1 - GetElevation(rcvr) - earthradius_);

        }

        else
            snprintf(string_f1, 150, "\nThe first Fresnel zone is clear.\n");
    }

    fprintf(outfile, "%s", string);

    if (f) {
        fprintf(outfile, "%s", string_f1);
        fprintf(outfile, "%s", string_fpt6);
    }
}

void SplatProcessor::PathReport(struct site source, struct site destination, char *name,
                                char graph_it) {
    /* This function writes a SPLAT! Path Report (name.txt) to
       the filesystem.  If (graph_it == 1), then gnuplot is invoked
       to generate an appropriate output file indicating the ITM
       model loss between the source and destination locations.
       "filename" is the name assigned to the output file generated
       by gnuplot.  The filename extension is used to set gnuplot's
       terminal setting and output file type.  If no extension is
       found, .png is assumed. */

    int x, y, z, errnum;
    char basename[255], term[30], ext[15], strmode[100], report_name[80], block = 0, propstring[20];
    double maxloss = -100000.0, minloss = 100000.0, loss, haavt, angle1, angle2, azimuth,
           pattern = 1.0, patterndB = 0.0, total_loss = 0.0, cos_xmtr_angle, cos_test_angle = 0.0,
           source_alt, test_alt, dest_alt, source_alt2, dest_alt2, distance, elevation,
           four_thirds_earth, field_strength, free_space_loss = 0.0, eirp = 0.0, voltage, rxp, dBm,
           power_density;
    FILE *fd = NULL, *fd2 = NULL;

    sprintf(report_name, "%s-to-%s.txt", source.name, destination.name);

    four_thirds_earth = FOUR_THIRDS * EARTHRADIUS;

    for (x = 0; report_name[x] != 0; x++)
        if (report_name[x] == 32 || report_name[x] == 17 || report_name[x] == 92 ||
            report_name[x] == 42 || report_name[x] == 47)
            report_name[x] = '_';

    fd2 = fopen(report_name, "w");

    fprintf(fd2, "\n\t\t--==[ %s v%s Path Analysis ]==--\n\n", splat_name_, splat_version_);
    fprintf(fd2, "%s\n\n", dashes_);
    fprintf(fd2, "Transmitter site: %s\n", source.name);

    if (source.lat >= 0.0) {
        fprintf(fd2, "Site location: %.4f North / %.4f West", source.lat, source.lon);
        fprintf(fd2, " (%s N / ", dec2dms(source.lat));
    }

    else {
        fprintf(fd2, "Site location: %.4f South / %.4f West", -source.lat, source.lon);
        fprintf(fd2, " (%s S / ", dec2dms(source.lat));
    }

    fprintf(fd2, "%s W)\n", dec2dms(source.lon));

    if (metric_) {
        fprintf(fd2, "Ground elevation: %.2f meters AMSL\n",
                METERS_PER_FOOT * GetElevation(source));
        fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n",
                METERS_PER_FOOT * source.alt,
                METERS_PER_FOOT * (source.alt + GetElevation(source)));
    }

    else {
        fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetElevation(source));
        fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", source.alt,
                source.alt + GetElevation(source));
    }

    haavt = haat(source);

    if (haavt > -4999.0) {
        if (metric_)
            fprintf(fd2, "Antenna height above average terrain: %.2f meters\n",
                    METERS_PER_FOOT * haavt);
        else
            fprintf(fd2, "Antenna height above average terrain: %.2f feet\n", haavt);
    }

    azimuth = Azimuth(source, destination);
    angle1 = ElevationAngle(source, destination);
    angle2 = ElevationAngle2(source, destination, earthradius_);

    if (got_azimuth_pattern_ || got_elevation_pattern_) {
        x = (int)rint(10.0 * (10.0 - angle2));

        if (x >= 0 && x <= 1000) pattern = (double)LR_.antenna_pattern[(int)rint(azimuth)][x];

        patterndB = 20.0 * log10(pattern);
    }

    if (metric_)
        fprintf(fd2, "Distance to %s: %.2f kilometers\n", destination.name,
                KM_PER_MILE * Distance(source, destination));

    else
        fprintf(fd2, "Distance to %s: %.2f miles\n", destination.name,
                Distance(source, destination));

    fprintf(fd2, "Azimuth to %s: %.2f degrees\n", destination.name, azimuth);

    if (angle1 >= 0.0)
        fprintf(fd2, "Elevation angle to %s: %+.4f degrees\n", destination.name, angle1);

    else
        fprintf(fd2, "Depression angle to %s: %+.4f degrees\n", destination.name, angle1);

    if ((angle2 - angle1) > 0.0001) {
        if (angle2 < 0.0)
            fprintf(fd2, "Depression");
        else
            fprintf(fd2, "Elevation");

        fprintf(fd2, " angle to the first obstruction: %+.4f degrees\n", angle2);
    }

    fprintf(fd2, "\n%s\n\n", dashes_);

    /* Receiver */

    fprintf(fd2, "Receiver site: %s\n", destination.name);

    if (destination.lat >= 0.0) {
        fprintf(fd2, "Site location: %.4f North / %.4f West", destination.lat, destination.lon);
        fprintf(fd2, " (%s N / ", dec2dms(destination.lat));
    }

    else {
        fprintf(fd2, "Site location: %.4f South / %.4f West", -destination.lat, destination.lon);
        fprintf(fd2, " (%s S / ", dec2dms(destination.lat));
    }

    fprintf(fd2, "%s W)\n", dec2dms(destination.lon));

    if (metric_) {
        fprintf(fd2, "Ground elevation: %.2f meters AMSL\n",
                METERS_PER_FOOT * GetElevation(destination));
        fprintf(fd2, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n",
                METERS_PER_FOOT * destination.alt,
                METERS_PER_FOOT * (destination.alt + GetElevation(destination)));
    }

    else {
        fprintf(fd2, "Ground elevation: %.2f feet AMSL\n", GetElevation(destination));
        fprintf(fd2, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", destination.alt,
                destination.alt + GetElevation(destination));
    }

    haavt = haat(destination);

    if (haavt > -4999.0) {
        if (metric_)
            fprintf(fd2, "Antenna height above average terrain: %.2f meters\n",
                    METERS_PER_FOOT * haavt);
        else
            fprintf(fd2, "Antenna height above average terrain: %.2f feet\n", haavt);
    }

    if (metric_)
        fprintf(fd2, "Distance to %s: %.2f kilometers\n", source.name,
                KM_PER_MILE * Distance(source, destination));

    else
        fprintf(fd2, "Distance to %s: %.2f miles\n", source.name, Distance(source, destination));

    azimuth = Azimuth(destination, source);

    angle1 = ElevationAngle(destination, source);
    angle2 = ElevationAngle2(destination, source, earthradius_);

    fprintf(fd2, "Azimuth to %s: %.2f degrees\n", source.name, azimuth);

    if (angle1 >= 0.0)
        fprintf(fd2, "Elevation angle to %s: %+.4f degrees\n", source.name, angle1);

    else
        fprintf(fd2, "Depression angle to %s: %+.4f degrees\n", source.name, angle1);

    if ((angle2 - angle1) > 0.0001) {
        if (angle2 < 0.0)
            fprintf(fd2, "Depression");
        else
            fprintf(fd2, "Elevation");

        fprintf(fd2, " angle to the first obstruction: %+.4f degrees\n", angle2);
    }

    fprintf(fd2, "\n%s\n\n", dashes_);

    if (LR_.frq_mhz > 0.0) {
        if (olditm_)
            fprintf(fd2, "Longley-Rice Parameters Used In This Analysis:\n\n");
        else
            fprintf(fd2, "ITWOM Version %.1f Parameters Used In This Analysis:\n\n",
                    ITWOMVersion());

        fprintf(fd2, "Earth's Dielectric Constant: %.3lf\n", LR_.eps_dielect);
        fprintf(fd2, "Earth's Conductivity: %.3lf Siemens/meter\n", LR_.sgm_conductivity);
        fprintf(fd2, "Atmospheric Bending Constant (N-units): %.3lf ppm\n", LR_.eno_ns_surfref);
        fprintf(fd2, "Frequency: %.3lf MHz\n", LR_.frq_mhz);
        fprintf(fd2, "Radio Climate: %d (", LR_.radio_climate);

        switch (LR_.radio_climate) {
            case 1:
                fprintf(fd2, "Equatorial");
                break;

            case 2:
                fprintf(fd2, "Continental Subtropical");
                break;

            case 3:
                fprintf(fd2, "Maritime Subtropical");
                break;

            case 4:
                fprintf(fd2, "Desert");
                break;

            case 5:
                fprintf(fd2, "Continental Temperate");
                break;

            case 6:
                fprintf(fd2, "Martitime Temperate, Over Land");
                break;

            case 7:
                fprintf(fd2, "Maritime Temperate, Over Sea");
                break;

            default:
                fprintf(fd2, "Unknown");
        }

        fprintf(fd2, ")\nPolarization: %d (", LR_.pol);

        if (LR_.pol == 0) fprintf(fd2, "Horizontal");

        if (LR_.pol == 1) fprintf(fd2, "Vertical");

        fprintf(fd2, ")\nFraction of Situations: %.1lf%c\n", LR_.conf * 100.0, 37);
        fprintf(fd2, "Fraction of Time: %.1lf%c\n", LR_.rel * 100.0, 37);

        if (LR_.erp != 0.0) {
            fprintf(fd2, "Transmitter ERP: ");

            if (LR_.erp < 1.0) fprintf(fd2, "%.1lf milliwatts", 1000.0 * LR_.erp);

            if (LR_.erp >= 1.0 && LR_.erp < 10.0) fprintf(fd2, "%.1lf Watts", LR_.erp);

            if (LR_.erp >= 10.0 && LR_.erp < 10.0e3) fprintf(fd2, "%.0lf Watts", LR_.erp);

            if (LR_.erp >= 10.0e3) fprintf(fd2, "%.3lf kilowatts", LR_.erp / 1.0e3);

            dBm = 10.0 * (log10(LR_.erp * 1000.0));
            fprintf(fd2, " (%+.2f dBm)\n", dBm);

            /* EIRP = ERP + 2.14 dB */

            fprintf(fd2, "Transmitter EIRP: ");

            eirp = LR_.erp * 1.636816521;

            if (eirp < 1.0) fprintf(fd2, "%.1lf milliwatts", 1000.0 * eirp);

            if (eirp >= 1.0 && eirp < 10.0) fprintf(fd2, "%.1lf Watts", eirp);

            if (eirp >= 10.0 && eirp < 10.0e3) fprintf(fd2, "%.0lf Watts", eirp);

            if (eirp >= 10.0e3) fprintf(fd2, "%.3lf kilowatts", eirp / 1.0e3);

            dBm = 10.0 * (log10(eirp * 1000.0));
            fprintf(fd2, " (%+.2f dBm)\n", dBm);
        }

        fprintf(fd2, "\n%s\n\n", dashes_);

        fprintf(fd2, "Summary For The Link Between %s and %s:\n\n", source.name, destination.name);

        if (patterndB != 0.0)
            fprintf(fd2, "%s antenna pattern towards %s: %.3f (%.2f dB)\n", source.name,
                    destination.name, pattern, patterndB);

        ReadPath(source, destination); /* source=TX, destination=RX */

        /* Copy elevations plus clutter along
           path into the elev[] array. */

        for (x = 1; x < path_.length - 1; x++)
            elev_[x + 2] =
                METERS_PER_FOOT *
                (path_.elevation[x] == 0.0 ? path_.elevation[x] : (clutter_ + path_.elevation[x]));

        /* Copy ending points without clutter */

        elev_[2] = path_.elevation[0] * METERS_PER_FOOT;
        elev_[path_.length + 1] = path_.elevation[path_.length - 1] * METERS_PER_FOOT;

        fd = fopen("profile.gp", "w");

        azimuth = rint(Azimuth(source, destination));

        for (y = 2; y < (path_.length - 1); y++) /* path_.length-1 avoids LR error */
        {
            distance = 5280.0 * path_.distance[y];
            source_alt = four_thirds_earth + source.alt + path_.elevation[0];
            dest_alt = four_thirds_earth + destination.alt + path_.elevation[y];
            dest_alt2 = dest_alt * dest_alt;
            source_alt2 = source_alt * source_alt;

            /* Calculate the cosine of the elevation of
               the receiver as seen by the transmitter. */

            cos_xmtr_angle = ((source_alt2) + (distance * distance) - (dest_alt2)) /
                             (2.0 * source_alt * distance);

            if (got_elevation_pattern_) {
                /* If an antenna elevation pattern is available, the
                   following code determines the elevation angle to
                   the first obstruction along the path. */

                for (x = 2, block = 0; x < y && block == 0; x++) {
                    distance = 5280.0 * (path_.distance[y] - path_.distance[x]);
                    test_alt = four_thirds_earth + path_.elevation[x];

                    /* Calculate the cosine of the elevation
                       angle of the terrain (test point)
                       as seen by the transmitter. */

                    cos_test_angle =
                        ((source_alt2) + (distance * distance) - (test_alt * test_alt)) /
                        (2.0 * source_alt * distance);

                    /* Compare these two angles to determine if
                       an obstruction exists.  Since we're comparing
                       the cosines of these angles rather than
                       the angles themselves, the sense of the
                       following "if" statement is reversed from
                       what it would be if the angles themselves
                       were compared. */

                    if (cos_xmtr_angle >= cos_test_angle) block = 1;
                }

                /* At this point, we have the elevation angle
                   to the first obstruction (if it exists). */
            }

            /* Determine path loss for each point along
               the path using ITWOM's point_to_point mode
               starting at x=2 (number_of_points = 1), the
               shortest distance terrain can play a role in
               path loss. */

            elev_[0] = y - 1; /* (number of points - 1) */

            /* Distance between elevation samples */

            elev_[1] = METERS_PER_MILE * (path_.distance[y] - path_.distance[y - 1]);

            if (olditm_)
                point_to_point_ITM(
                    elev_, source.alt * METERS_PER_FOOT, destination.alt * METERS_PER_FOOT,
                    LR_.eps_dielect, LR_.sgm_conductivity, LR_.eno_ns_surfref, LR_.frq_mhz,
                    LR_.radio_climate, LR_.pol, LR_.conf, LR_.rel, loss, strmode, errnum);
            else
                point_to_point(elev_, source.alt * METERS_PER_FOOT,
                               destination.alt * METERS_PER_FOOT, LR_.eps_dielect,
                               LR_.sgm_conductivity, LR_.eno_ns_surfref, LR_.frq_mhz, LR_.radio_climate,
                               LR_.pol, LR_.conf, LR_.rel, loss, strmode, errnum);

            if (block)
                elevation = ((acos(cos_test_angle)) / DEG2RAD) - 90.0;
            else
                elevation = ((acos(cos_xmtr_angle)) / DEG2RAD) - 90.0;

            /* Integrate the antenna's radiation
               pattern into the overall path loss. */

            x = (int)rint(10.0 * (10.0 - elevation));

            if (x >= 0 && x <= 1000) {
                pattern = (double)LR_.antenna_pattern[(int)azimuth][x];

                if (pattern != 0.0) patterndB = 20.0 * log10(pattern);
            }

            else
                patterndB = 0.0;

            total_loss = loss - patterndB;

            if (metric_)
                fprintf(fd, "%f\t%f\n", KM_PER_MILE * path_.distance[y], total_loss);

            else
                fprintf(fd, "%f\t%f\n", path_.distance[y], total_loss);

            if (total_loss > maxloss) maxloss = total_loss;

            if (total_loss < minloss) minloss = total_loss;
        }

        fclose(fd);

        distance = Distance(source, destination);

        if (distance != 0.0) {
            free_space_loss = 36.6 + (20.0 * log10(LR_.frq_mhz)) + (20.0 * log10(distance));

            fprintf(fd2, "Free space path loss: %.2f dB\n", free_space_loss);
        }

        if (olditm_)
            fprintf(fd2, "Longley-Rice path loss: %.2f dB\n", loss);
        else
            fprintf(fd2, "ITWOM Version %.1f path loss: %.2f dB\n", ITWOMVersion(), loss);

        if (free_space_loss != 0.0)
            fprintf(fd2, "Attenuation due to terrain shielding: %.2f dB\n", loss - free_space_loss);

        if (patterndB != 0.0)
            fprintf(fd2, "Total path loss including %s antenna pattern: %.2f dB\n", source.name,
                    total_loss);

        if (LR_.erp != 0.0) {
            field_strength =
                (139.4 + (20.0 * log10(LR_.frq_mhz)) - total_loss) + (10.0 * log10(LR_.erp / 1000.0));

            /* dBm is referenced to EIRP */

            rxp = eirp / (pow(10.0, (total_loss / 10.0)));
            dBm = 10.0 * (log10(rxp * 1000.0));
            power_density = (eirp / (pow(10.0, (total_loss - free_space_loss) / 10.0)));
            /* divide by 4*PI*distance_in_meters squared */
            power_density /= (4.0 * PI * distance * distance * 2589988.11);

            fprintf(fd2, "Field strength at %s: %.2f dBuV/meter\n", destination.name,
                    field_strength);
            fprintf(fd2, "Signal power level at %s: %+.2f dBm\n", destination.name, dBm);
            fprintf(fd2, "Signal power density at %s: %+.2f dBW per square meter\n",
                    destination.name, 10.0 * log10(power_density));
            voltage = 1.0e6 * sqrt(50.0 * (eirp / (pow(10.0, (total_loss - 2.14) / 10.0))));
            fprintf(fd2, "Voltage across a 50 ohm dipole at %s: %.2f uV (%.2f dBuV)\n",
                    destination.name, voltage, 20.0 * log10(voltage));

            voltage = 1.0e6 * sqrt(75.0 * (eirp / (pow(10.0, (total_loss - 2.14) / 10.0))));
            fprintf(fd2, "Voltage across a 75 ohm dipole at %s: %.2f uV (%.2f dBuV)\n",
                    destination.name, voltage, 20.0 * log10(voltage));
        }

        fprintf(fd2, "Mode of propagation: ");

        if (olditm_) {
            fprintf(fd2, "%s\n", strmode);
            fprintf(fd2, "Longley-Rice model error number: %d", errnum);
        }

        else {
            if (strcmp(strmode, "L-o-S") == 0) fprintf(fd2, "Line of Sight\n");

            if (strncmp(strmode, "1_Hrzn", 6) == 0) fprintf(fd2, "Single Horizon ");

            if (strncmp(strmode, "2_Hrzn", 6) == 0) fprintf(fd2, "Double Horizon ");

            y = strlen(strmode);

            if (y > 19) y = 19;

            for (x = 6; x < y; x++) propstring[x - 6] = strmode[x];

            propstring[x] = 0;

            if (strncmp(propstring, "_Diff", 5) == 0) fprintf(fd2, "Diffraction Dominant\n");

            if (strncmp(propstring, "_Tropo", 6) == 0) fprintf(fd2, "Troposcatter Dominant\n");

            if (strncmp(propstring, "_Peak", 5) == 0)
                fprintf(fd2, "RX at Peak Terrain Along Path\n");

            fprintf(fd2, "ITWOM error number: %d", errnum);
        }

        switch (errnum) {
            case 0:
                fprintf(fd2, " (No error)\n");
                break;

            case 1:
                fprintf(fd2, "\n  Warning: Some parameters are nearly out of range.\n");
                fprintf(fd2, "  Results should be used with caution.\n");
                break;

            case 2:
                fprintf(
                    fd2,
                    "\n  Note: Default parameters have been substituted for impossible ones.\n");
                break;

            case 3:
                fprintf(fd2, "\n  Warning: A combination of parameters is out of range.\n");
                fprintf(fd2, "  Results are probably invalid.\n");
                break;

            default:
                fprintf(fd2, "\n  Warning: Some parameters are out of range.\n");
                fprintf(fd2, "  Results are probably invalid.\n");
        }

        fprintf(fd2, "\n%s\n\n", dashes_);
    }

    fprintf(stdout, "\nPath Loss Report written to: \"%s\"\n", report_name);
    fflush(stdout);

    ObstructionAnalysis(source, destination, LR_.frq_mhz, fd2);

    fclose(fd2);

    /* Skip plotting the graph if ONLY a path-loss report is needed. */

    if (graph_it) {
        if (name[0] == '.') {
            /* Default filename and output file type */

            strncpy(basename, "profile\0", 8);
            strncpy(term, "png\0", 4);
            strncpy(ext, "png\0", 4);
        }

        else {
            /* Extract extension and terminal type from "name" */

            ext[0] = 0;
            y = strlen(name);
            strncpy(basename, name, 254);

            for (x = y - 1; x > 0 && name[x] != '.'; x--);

            if (x > 0) /* Extension found */
            {
                for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++) {
                    ext[z - (x + 1)] = tolower(name[z]);
                    term[z - (x + 1)] = name[z];
                }

                ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
                term[z - (x + 1)] = 0;
                basename[x] = 0;
            }
        }

        if (ext[0] == 0) /* No extension -- Default is png */
        {
            strncpy(term, "png\0", 4);
            strncpy(ext, "png\0", 4);
        }

        /* Either .ps or .postscript may be used
           as an extension for postscript output. */

        if (strncmp(term, "postscript", 10) == 0)
            strncpy(ext, "ps\0", 3);

        else if (strncmp(ext, "ps", 2) == 0)
            strncpy(term, "postscript enhanced color\0", 26);

        fd = fopen("splat.gp", "w");

        fprintf(fd, "set grid\n");
        fprintf(fd, "set yrange [%2.3f to %2.3f]\n", minloss, maxloss);
        fprintf(fd, "set encoding iso_8859_1\n");
        fprintf(fd, "set term %s\n", term);
        fprintf(fd, "set title \"%s Loss Profile Along Path Between %s and %s (%.2f%c azimuth)\"\n",
                splat_name_, destination.name, source.name, Azimuth(destination, source), 176);

        if (metric_)
            fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f kilometers)\"\n",
                    destination.name, source.name, KM_PER_MILE * Distance(destination, source));
        else
            fprintf(fd, "set xlabel \"Distance Between %s and %s (%.2f miles)\"\n",
                    destination.name, source.name, Distance(destination, source));

        if (got_azimuth_pattern_ || got_elevation_pattern_)
            fprintf(fd, "set ylabel \"Total Path Loss (including TX antenna pattern) (dB)");
        else {
            if (olditm_)
                fprintf(fd, "set ylabel \"Longley-Rice Path Loss (dB)");
            else
                fprintf(fd, "set ylabel \"ITWOM Version %.1f Path Loss (dB)", ITWOMVersion());
        }

        fprintf(fd, "\"\nset output \"%s.%s\"\n", basename, ext);
        fprintf(fd, "plot \"profile.gp\" title \"Path Loss\" with lines\n");

        fclose(fd);

        x = system("gnuplot splat.gp");

        if (x != -1) {
            if (gpsav_ == 0) {
                unlink("splat.gp");
                unlink("profile.gp");
                unlink("reference.gp");
            }

            fprintf(stdout, "Path loss plot written to: \"%s.%s\"\n", basename, ext);
            fflush(stdout);
        }

        else
            fprintf(stderr, "\n*** ERROR: Error occurred invoking gnuplot!\n");
    }

    if (x != -1 && gpsav_ == 0) unlink("profile.gp");
}

// Function to extract the directory path up to the PPM file
void SplatProcessor::extractSplatPath(const char *ppmFilePath, char *outputPath,
                                      size_t outputPathSize) {
    const char *lastSlash = strrchr(ppmFilePath, '/');  // Find the last occurrence of '/'
    if (lastSlash != NULL) {
        size_t length = lastSlash - ppmFilePath + 1;  // Include the '/' at the end
        if (length < outputPathSize) {
            strncpy(outputPath, ppmFilePath, length);
            outputPath[length] = '\0';  // Null-terminate the string
        } else {
            fprintf(stderr, "Error: Output path buffer is too small.\n");
        }
    } else {
        fprintf(stderr, "Error: Invalid PPM file path format.\n");
    }
}

void SplatProcessor::SiteReport(const char *ppmFilePath, struct site xmtr) {
    char outputPath[256];     // Buffer to hold the directory path
    char reportName[256];     // Buffer for the full report file path
    char sanitizedName[256];  // Buffer for sanitized transmitter name
    double terrain;
    int x, azi;
    FILE *fd;

    // Extract the path up to the directory of the PPM file
    extractSplatPath(ppmFilePath, outputPath, sizeof(outputPath));

    // Sanitize the transmitter name to remove invalid characters
    strncpy(sanitizedName, xmtr.name, sizeof(sanitizedName));
    sanitizedName[sizeof(sanitizedName) - 1] = '\0';  // Ensure null-terminated
    for (x = 0; sanitizedName[x] != '\0'; x++) {
        if (sanitizedName[x] == 32 || sanitizedName[x] == 17 || sanitizedName[x] == 92 ||
            sanitizedName[x] == 42 || sanitizedName[x] == 47) {
            sanitizedName[x] = '_';
        }
    }
    // Construct the full report path
    sprintf(reportName, "%s%s-site_report.txt", outputPath, sanitizedName);
    /* sprintf(report_name,"%s-site_report.txt",xmtr.name);*/

    // Debug log
    printf("Extracted output path: %s\n", outputPath);
    printf("Full report path: %s\n", reportName);

    fd = fopen(reportName, "w");

    fprintf(fd, "\n\t--==[ %s v%s Site Analysis Report For: %s ]==--\n\n", splat_name_,
            splat_version_, xmtr.name);

    fprintf(fd, "%s\n\n", dashes_);

    if (xmtr.lat >= 0.0) {
        fprintf(fd, "Site location: %.4f North / %.4f West", xmtr.lat, xmtr.lon);
        fprintf(fd, " (%s N / ", dec2dms(xmtr.lat));
    }

    else {
        fprintf(fd, "Site location: %.4f South / %.4f West", -xmtr.lat, xmtr.lon);
        fprintf(fd, " (%s S / ", dec2dms(xmtr.lat));
    }

    fprintf(fd, "%s W)\n", dec2dms(xmtr.lon));

    if (metric_) {
        fprintf(fd, "Ground elevation: %.2f meters AMSL\n", METERS_PER_FOOT * GetElevation(xmtr));
        fprintf(fd, "Antenna height: %.2f meters AGL / %.2f meters AMSL\n",
                METERS_PER_FOOT * xmtr.alt, METERS_PER_FOOT * (xmtr.alt + GetElevation(xmtr)));
    }

    else {
        fprintf(fd, "Ground elevation: %.2f feet AMSL\n", GetElevation(xmtr));
        fprintf(fd, "Antenna height: %.2f feet AGL / %.2f feet AMSL\n", xmtr.alt,
                xmtr.alt + GetElevation(xmtr));
    }

    terrain = haat(xmtr);

    if (terrain > -4999.0) {
        if (metric_)
            fprintf(fd, "Antenna height above average terrain: %.2f meters\n\n",
                    METERS_PER_FOOT * terrain);
        else
            fprintf(fd, "Antenna height above average terrain: %.2f feet\n\n", terrain);

        /* Display the average terrain between 2 and 10 miles
           from the transmitter site at azimuths of 0, 45, 90,
           135, 180, 225, 270, and 315 degrees. */

        for (azi = 0; azi <= 315; azi += 45) {
            fprintf(fd, "Average terrain at %3d degrees azimuth: ", azi);
            terrain = AverageTerrain(xmtr, (double)azi, 2.0, 10.0);

            if (terrain > -4999.0) {
                if (metric_)
                    fprintf(fd, "%.2f meters AMSL\n", METERS_PER_FOOT * terrain);
                else
                    fprintf(fd, "%.2f feet AMSL\n", terrain);
            }

            else
                fprintf(fd, "No terrain\n");
        }
    }

    fprintf(fd, "\n%s\n\n", dashes_);
    fclose(fd);
    fprintf(stdout, "\nSite analysis report written to: \"%s\"\n", reportName);
}

void SplatProcessor::LoadTopoData(int max_lon, int min_lon, int max_lat, int min_lat) {
    /* This function loads the SDF files required
       to cover the limits of the region specified. */
    int x, y, width, ymin, ymax;

    width = ReduceAngle(max_lon - min_lon);

    if ((max_lon - min_lon) <= 180.0) {
        for (y = 0; y <= width; y++)
            for (x = min_lat; x <= max_lat; x++) {
                ymin = (int)(min_lon + (double)y);

                while (ymin < 0) ymin += 360;

                while (ymin >= 360) ymin -= 360;

                ymax = ymin + 1;

                while (ymax < 0) ymax += 360;

                while (ymax >= 360) ymax -= 360;

                if (ippd_ == 3600)
                    snprintf(string_, 19, "%d_%d_%d_%d-hd", x, x + 1, ymin, ymax);
                else
                    snprintf(string_, 16, "%d_%d_%d_%d", x, x + 1, ymin, ymax);
                LoadSDF(string_);
            }
    }

    else {
        for (y = 0; y <= width; y++)
            for (x = min_lat; x <= max_lat; x++) {
                ymin = max_lon + y;

                while (ymin < 0) ymin += 360;

                while (ymin >= 360) ymin -= 360;

                ymax = ymin + 1;

                while (ymax < 0) ymax += 360;

                while (ymax >= 360) ymax -= 360;

                if (ippd_ == 3600)
                    snprintf(string_, 19, "%d_%d_%d_%d-hd", x, x + 1, ymin, ymax);
                else
                    snprintf(string_, 16, "%d_%d_%d_%d", x, x + 1, ymin, ymax);
                LoadSDF(string_);
            }
    }
}

int SplatProcessor::LoadANO(char *filename) {
    /* This function reads a SPLAT! alphanumeric output
       file (-ani option) for analysis and/or map generation. */

    int error = 0, max_west, min_west, max_north, min_north;
    char string[80], *pointer = NULL;
    double latitude = 0.0, longitude = 0.0, azimuth = 0.0, elevation = 0.0, ano = 0.0;
    FILE *fd;

    fd = fopen(filename, "r");

    if (fd != NULL) {
        fgets(string, 78, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%d, %d", &max_west, &min_west);

        fgets(string, 78, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        sscanf(string, "%d, %d", &max_north, &min_north);

        fgets(string, 78, fd);
        pointer = strchr(string, ';');

        if (pointer != NULL) *pointer = 0;

        LoadTopoData(max_west - 1, min_west, max_north - 1, min_north);

        fprintf(stdout, "\nReading \"%s\"... ", filename);
        fflush(stdout);

        fgets(string, 78, fd);
        sscanf(string, "%lf, %lf, %lf, %lf, %lf", &latitude, &longitude, &azimuth, &elevation,
               &ano);

        while (feof(fd) == 0) {
            if (LR_.erp == 0.0) {
                /* Path loss */

                if (contour_threshold_ == 0 || (fabs(ano) <= (double)contour_threshold_)) {
                    ano = fabs(ano);

                    if (ano > 255.0) ano = 255.0;

                    PutSignal(latitude, longitude, ((unsigned char)round(ano)));
                }
            }

            if (LR_.erp != 0.0 && dbm_ != 0) {
                /* signal power level in dBm */

                if (contour_threshold_ == 0 || (ano >= (double)contour_threshold_)) {
                    ano = 200.0 + rint(ano);

                    if (ano < 0.0) ano = 0.0;

                    if (ano > 255.0) ano = 255.0;

                    PutSignal(latitude, longitude, ((unsigned char)round(ano)));
                }
            }

            if (LR_.erp != 0.0 && dbm_ == 0) {
                /* field strength dBuV/m */

                if (contour_threshold_ == 0 || (ano >= (double)contour_threshold_)) {
                    ano = 100.0 + rint(ano);

                    if (ano < 0.0) ano = 0.0;

                    if (ano > 255.0) ano = 255.0;

                    PutSignal(latitude, longitude, ((unsigned char)round(ano)));
                }
            }

            fgets(string, 78, fd);
            sscanf(string, "%lf, %lf, %lf, %lf, %lf", &latitude, &longitude, &azimuth, &elevation,
                   &ano);
        }

        fclose(fd);

        fprintf(stdout, " Done!\n");
        fflush(stdout);
    }

    else
        error = 1;

    return error;
}

void SplatProcessor::WriteKML(struct site source, struct site destination) {
    int x, y;
    char block, report_name[80];
    double distance, rx_alt, tx_alt, cos_xmtr_angle, azimuth, cos_test_angle, test_alt;
    FILE *fd = NULL;

    ReadPath(source, destination);

    sprintf(report_name, "%s-to-%s.kml", source.name, destination.name);

    for (x = 0; report_name[x] != 0; x++)
        if (report_name[x] == 32 || report_name[x] == 17 || report_name[x] == 92 ||
            report_name[x] == 42 || report_name[x] == 47)
            report_name[x] = '_';

    fd = fopen(report_name, "w");

    fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fd, "<kml xmlns=\"http://earth.google.com/kml/2.0\">\n");
    fprintf(fd, "<!-- Generated by %s Version %s -->\n", splat_name_, splat_version_);
    fprintf(fd, "<Folder>\n");
    fprintf(fd, "<name>SPLAT! Path</name>\n");
    fprintf(fd, "<open>1</open>\n");
    fprintf(fd, "<description>Path Between %s and %s</description>\n", source.name,
            destination.name);

    fprintf(fd, "<Placemark>\n");
    fprintf(fd, "    <name>%s</name>\n", source.name);
    fprintf(fd, "    <description>\n");
    fprintf(fd, "       Transmit Site\n");

    if (source.lat >= 0.0)
        fprintf(fd, "       <BR>%s North</BR>\n", dec2dms(source.lat));
    else
        fprintf(fd, "       <BR>%s South</BR>\n", dec2dms(source.lat));

    fprintf(fd, "       <BR>%s West</BR>\n", dec2dms(source.lon));

    azimuth = Azimuth(source, destination);
    distance = Distance(source, destination);

    if (metric_)
        fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
    else
        fprintf(fd, "       <BR>%.2f miles", distance);

    fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", destination.name,
            azimuth, 176);

    fprintf(fd, "    </description>\n");
    fprintf(fd, "    <visibility>1</visibility>\n");
    fprintf(fd, "    <Style>\n");
    fprintf(fd, "      <IconStyle>\n");
    fprintf(fd, "        <Icon>\n");
    fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
    fprintf(fd, "          <x>224</x>\n");
    fprintf(fd, "          <y>224</y>\n");
    fprintf(fd, "          <w>32</w>\n");
    fprintf(fd, "          <h>32</h>\n");
    fprintf(fd, "        </Icon>\n");
    fprintf(fd, "      </IconStyle>\n");
    fprintf(fd, "    </Style>\n");
    fprintf(fd, "    <Point>\n");
    fprintf(fd, "      <extrude>1</extrude>\n");
    fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
    fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n",
            (source.lon < 180.0 ? -source.lon : 360.0 - source.lon), source.lat);
    fprintf(fd, "    </Point>\n");
    fprintf(fd, "</Placemark>\n");

    fprintf(fd, "<Placemark>\n");
    fprintf(fd, "    <name>%s</name>\n", destination.name);
    fprintf(fd, "    <description>\n");
    fprintf(fd, "       Receive Site\n");

    if (destination.lat >= 0.0)
        fprintf(fd, "       <BR>%s North</BR>\n", dec2dms(destination.lat));
    else
        fprintf(fd, "       <BR>%s South</BR>\n", dec2dms(destination.lat));

    fprintf(fd, "       <BR>%s West</BR>\n", dec2dms(destination.lon));

    if (metric_)
        fprintf(fd, "       <BR>%.2f km", distance * KM_PER_MILE);
    else
        fprintf(fd, "       <BR>%.2f miles", distance);

    fprintf(fd, " to %s</BR>\n       <BR>toward an azimuth of %.2f%c</BR>\n", source.name,
            Azimuth(destination, source), 176);

    fprintf(fd, "    </description>\n");
    fprintf(fd, "    <visibility>1</visibility>\n");
    fprintf(fd, "    <Style>\n");
    fprintf(fd, "      <IconStyle>\n");
    fprintf(fd, "        <Icon>\n");
    fprintf(fd, "          <href>root://icons/palette-5.png</href>\n");
    fprintf(fd, "          <x>224</x>\n");
    fprintf(fd, "          <y>224</y>\n");
    fprintf(fd, "          <w>32</w>\n");
    fprintf(fd, "          <h>32</h>\n");
    fprintf(fd, "        </Icon>\n");
    fprintf(fd, "      </IconStyle>\n");
    fprintf(fd, "    </Style>\n");
    fprintf(fd, "    <Point>\n");
    fprintf(fd, "      <extrude>1</extrude>\n");
    fprintf(fd, "      <altitudeMode>relativeToGround</altitudeMode>\n");
    fprintf(fd, "      <coordinates>%f,%f,30</coordinates>\n",
            (destination.lon < 180.0 ? -destination.lon : 360.0 - destination.lon),
            destination.lat);
    fprintf(fd, "    </Point>\n");
    fprintf(fd, "</Placemark>\n");

    fprintf(fd, "<Placemark>\n");
    fprintf(fd, "<name>Point-to-Point Path</name>\n");
    fprintf(fd, "  <visibility>1</visibility>\n");
    fprintf(fd, "  <open>0</open>\n");
    fprintf(fd, "  <Style>\n");
    fprintf(fd, "    <LineStyle>\n");
    fprintf(fd, "      <color>7fffffff</color>\n");
    fprintf(fd, "    </LineStyle>\n");
    fprintf(fd, "    <PolyStyle>\n");
    fprintf(fd, "       <color>7fffffff</color>\n");
    fprintf(fd, "    </PolyStyle>\n");
    fprintf(fd, "  </Style>\n");
    fprintf(fd, "  <LineString>\n");
    fprintf(fd, "    <extrude>1</extrude>\n");
    fprintf(fd, "    <tessellate>1</tessellate>\n");
    fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
    fprintf(fd, "    <coordinates>\n");

    for (x = 0; x < path_.length; x++)
        fprintf(fd, "      %f,%f,5\n", (path_.lon[x] < 180.0 ? -path_.lon[x] : 360.0 - path_.lon[x]),
                path_.lat[x]);

    fprintf(fd, "    </coordinates>\n");
    fprintf(fd, "   </LineString>\n");
    fprintf(fd, "</Placemark>\n");

    fprintf(fd, "<Placemark>\n");
    fprintf(fd, "<name>Line-of-Sight Path</name>\n");
    fprintf(fd, "  <visibility>1</visibility>\n");
    fprintf(fd, "  <open>0</open>\n");
    fprintf(fd, "  <Style>\n");
    fprintf(fd, "    <LineStyle>\n");
    fprintf(fd, "      <color>ff00ff00</color>\n");
    fprintf(fd, "    </LineStyle>\n");
    fprintf(fd, "    <PolyStyle>\n");
    fprintf(fd, "       <color>7f00ff00</color>\n");
    fprintf(fd, "    </PolyStyle>\n");
    fprintf(fd, "  </Style>\n");
    fprintf(fd, "  <LineString>\n");
    fprintf(fd, "    <extrude>1</extrude>\n");
    fprintf(fd, "    <tessellate>1</tessellate>\n");
    fprintf(fd, "    <altitudeMode>relativeToGround</altitudeMode>\n");
    fprintf(fd, "    <coordinates>\n");

    /* Walk across the "path", indentifying obstructions along the way */

    for (y = 0; y < path_.length; y++) {
        distance = 5280.0 * path_.distance[y];
        tx_alt = earthradius_ + source.alt + path_.elevation[0];
        rx_alt = earthradius_ + destination.alt + path_.elevation[y];

        /* Calculate the cosine of the elevation of the
           transmitter as seen at the temp rx point. */

        cos_xmtr_angle = ((rx_alt * rx_alt) + (distance * distance) - (tx_alt * tx_alt)) /
                         (2.0 * rx_alt * distance);

        for (x = y, block = 0; x >= 0 && block == 0; x--) {
            distance = 5280.0 * (path_.distance[y] - path_.distance[x]);
            test_alt = earthradius_ + path_.elevation[x];

            cos_test_angle = ((rx_alt * rx_alt) + (distance * distance) - (test_alt * test_alt)) /
                             (2.0 * rx_alt * distance);

            /* Compare these two angles to determine if
               an obstruction exists.  Since we're comparing
               the cosines of these angles rather than
               the angles themselves, the following "if"
               statement is reversed from what it would
               be if the actual angles were compared. */

            if (cos_xmtr_angle >= cos_test_angle) block = 1;
        }

        if (block)
            fprintf(fd, "      %f,%f,-30\n",
                    (path_.lon[y] < 180.0 ? -path_.lon[y] : 360.0 - path_.lon[y]), path_.lat[y]);
        else
            fprintf(fd, "      %f,%f,5\n",
                    (path_.lon[y] < 180.0 ? -path_.lon[y] : 360.0 - path_.lon[y]), path_.lat[y]);
    }

    fprintf(fd, "    </coordinates>\n");
    fprintf(fd, "  </LineString>\n");
    fprintf(fd, "</Placemark>\n");

    fprintf(fd, "    <LookAt>\n");
    fprintf(fd, "      <longitude>%f</longitude>\n",
            (source.lon < 180.0 ? -source.lon : 360.0 - source.lon));
    fprintf(fd, "      <latitude>%f</latitude>\n", source.lat);
    fprintf(fd, "      <range>300.0</range>\n");
    fprintf(fd, "      <tilt>45.0</tilt>\n");
    fprintf(fd, "      <heading>%f</heading>\n", azimuth);
    fprintf(fd, "    </LookAt>\n");

    fprintf(fd, "</Folder>\n");
    fprintf(fd, "</kml>\n");

    fclose(fd);

    fprintf(stdout, "\nKML file written to: \"%s\"", report_name);

    fflush(stdout);
}

// START OF INCAPSULATION
void SplatProcessor::printHelp(const char *splat_name, const char *splat_version, int &y) {
    fprintf(stdout, "\n\t\t --==[ %s v%s Available Options... ]==--\n\n", splat_name,
            splat_version);

    fprintf(stdout, "       -t txsite(s).qth (max of 4 with -c, max of 30 with -L)\n");
    fprintf(stdout, "       -r rxsite.qth\n");
    fprintf(stdout,
            "       -c plot LOS coverage of TX(s) with an RX antenna at X feet/meters AGL\n");
    fprintf(stdout, "       -L plot path loss map of TX based on an RX at X feet/meters AGL\n");
    fprintf(stdout,
            "       -LA plot path loss map of TX based on an RX at X feet/meters AGL with "
            "specified coverage angles\n");
    fprintf(stdout, "       -s filename(s) of city/site file(s) to import (5 max)\n");
    fprintf(stdout, "       -b filename(s) of cartographic boundary file(s) to import (5 max)\n");
    fprintf(stdout, "       -p filename of terrain profile graph to plot\n");
    fprintf(stdout, "       -e filename of terrain elevation graph to plot\n");
    fprintf(stdout, "       -h filename of terrain height graph to plot\n");
    fprintf(stdout, "       -H filename of normalized terrain height graph to plot\n");
    fprintf(stdout, "       -l filename of path loss graph to plot\n");
    fprintf(stdout, "       -o filename of topographic map to generate (.ppm)\n");
    fprintf(stdout, "       -u filename of user-defined terrain file to import\n");
    fprintf(stdout, "       -d sdf file directory path (overrides path in ~/.splat_path file)\n");
    fprintf(stdout, "       -m earth radius multiplier\n");
    fprintf(stdout, "       -n do not plot LOS paths in .ppm maps\n");
    fprintf(stdout, "       -N do not produce unnecessary site or obstruction reports\n");
    fprintf(stdout, "       -f frequency for Fresnel zone calculation (MHz)\n");
    fprintf(stdout, "       -R modify default range for -c or -L (miles/kilometers)\n");
    fprintf(stdout, "      -sc display smooth rather than quantized contour levels\n");
    fprintf(stdout, "      -db threshold beyond which contours will not be displayed\n");
    fprintf(stdout, "      -nf do not plot Fresnel zones in height plots\n");
    fprintf(stdout, "      -fz Fresnel zone clearance percentage (default = 60)\n");
    fprintf(stdout, "      -gc ground clutter height (feet/meters)\n");
    fprintf(stdout, "     -ngs display greyscale topography as white in .ppm files\n");
    fprintf(stdout, "     -trans display transparent topography in .ppm files\n");
    fprintf(stdout, "     -erp override ERP in .lrp file (Watts)\n");
    fprintf(stdout, "     -ano name of alphanumeric output file\n");
    fprintf(stdout, "     -ani name of alphanumeric input file\n");
    fprintf(stdout, "     -udt name of user defined terrain input file\n");
    fprintf(stdout, "     -kml generate Google Earth (.kml) compatible output\n");
    fprintf(stdout, "     -geo generate an Xastir .geo georeference file (with .ppm output)\n");
    fprintf(stdout, "     -dbm plot signal power level contours rather than field strength\n");
    fprintf(stdout, "     -log copy command line string to this output file\n");
    fprintf(stdout, "   -gpsav preserve gnuplot temporary working files after SPLAT! execution\n");
    fprintf(stdout, "  -metric employ metric rather than imperial units for all user I/O\n");
    fprintf(stdout, "  -olditm invoke Longley-Rice rather than the default ITWOM model\n\n");
    fprintf(stdout, "If that flew by too fast, consider piping the output through 'less':\n");

    if (HD_MODE == 0)
        fprintf(stdout, "\n\tsplat | less\n\n");
    else
        fprintf(stdout, "\n\tsplat-hd | less\n\n");

    fprintf(stdout, "Type 'man splat', or see the documentation for more details.\n\n");

    y = (int)sqrt((int)MAXPAGES);

    fprintf(stdout, "This compilation of %s supports analysis over a region of %d square\n",
            splat_name_, y);

    if (y == 1)

        fprintf(stdout, "degree");
    else
        fprintf(stdout, "degrees");

    fprintf(stdout, " of terrain, and computes signal levels using ITWOM Version %.1f.\n\n",
            ITWOMVersion());
    fflush(stdout);
}

void SplatProcessor::prepareHeader(const char *splat_name, const char *splat_version,
                                   char *header) {
    sprintf(header, "\n\t\t--==[ Welcome To %s v%s ]==--\n\n", splat_name, splat_version);
}

void SplatProcessor::parseArguments(int argc, char *argv[], char *header, int &y) {
    auto start = std::chrono::high_resolution_clock::now();
    int x, z = 0, min_lat, min_lon, max_lat, max_lon, rxlat, rxlon, txlat, txlon, west_min,
           west_max, north_min, north_max;

    unsigned char coverage = 0, LRmap = 0, terrain_plot = 0, elevation_plot = 0, height_plot = 0,
                  map = 0, longley_plot = 0, cities = 0, bfs = 0, txsites = 0, norm = 0,
                  topomap = 0, geo = 0, kml = 0, pt2pt_mode = 0, area_mode = 0, max_txsites,
                  ngs = 0, nolospath = 0, nositereports = 0, fresnel_plot = 1, command_line_log = 0;

    char mapfile[255], city_file[5][255], elevation_file[255], height_file[255], longley_file[255],
        terrain_file[255], string[255], rxfile[255],
        *env = NULL, txfile[255], boundary_file[5][255], udt_file[255], rxsite = 0,
        ani_filename[255], ano_filename[255], ext[20], logfile[255];

    double altitude = 0.0, altitudeLR = 0.0, tx_range = 0.0, rx_range = 0.0, deg_range = 0.0,
           deg_limit = 0.0, deg_range_lon, er_mult;

    struct site tx_site[32], rx_site;

    FILE *fd;

    olditm_ = 0;
    kml = 0;
    geo = 0;
    dbm_ = 0;
    gpsav_ = 0;
    metric_ = 0;  // CHECK
    rxfile[0] = 0;
    txfile[0] = 0;
    string[0] = 0;
    mapfile[0] = 0;
    clutter_ = 0.0;
    forced_erp_ = -1.0;
    forced_freq_ = 0.0;
    elevation_file[0] = 0;
    terrain_file[0] = 0;
    sdf_path_[0] = 0;
    udt_file[0] = 0;
    path_.length = 0;
    max_txsites = 30;
    fzone_clearance_ = 0.6;  // CHECK
    contour_threshold_ = 0;
    rx_site.lat = 91.0;
    rx_site.lon = 361.0;
    longley_file[0] = 0;
    ano_filename[0] = 0;
    ani_filename[0] = 0;
    smooth_contours_ = 0;
    earthradius_ = EARTHRADIUS;

    ippd_ = IPPD;        /* pixels per degree (integer) */
    ppd_ = (double)ippd_; /* pixels per degree (double)  */
    dpp_ = 1.0 / ppd_;    /* degrees per pixel */
    mpi_ = ippd_ - 1;     /* maximum pixel index per degree */

    for (x = 0; x < 4; x++) {
        tx_site[x].lat = 91.0;
        tx_site[x].lon = 361.0;
    }

    for (x = 0; x < MAXPAGES; x++) {
        dem_[x].min_el = 32768;
        dem_[x].max_el = -32768;
        dem_[x].min_north = 90;
        dem_[x].max_north = -90;
        dem_[x].min_west = 360;
        dem_[x].max_west = -1;
    }


    /* Scan for command line arguments */
        for (x = 1; x <= y; x++)
        {
            if (strcmp(argv[x], "-R") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &max_range_);

                    if (max_range_ < 0.0)
                        max_range_ = 0.0;

                    if (max_range_ > 1000.0)
                        max_range_ = 1000.0;
                }
            }

            if (strcmp(argv[x], "-m") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &er_mult);

                    if (er_mult < 0.1)
                        er_mult = 1.0;

                    if (er_mult > 1.0e6)
                        er_mult = 1.0e6;

                    earthradius_ *= er_mult;
                }
            }

            if (strcmp(argv[x], "-gc") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &clutter_);

                    if (clutter_ < 0.0)
                        clutter_ = 0.0;
                }
            }

            if (strcmp(argv[x], "-fz") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &fzone_clearance_);

                    if (fzone_clearance_ < 0.0 || fzone_clearance_ > 100.0)
                        fzone_clearance_ = 60.0;

                    fzone_clearance_ /= 100.0;
                }
            }

            strncpy(mapfile, mapFilePath_.c_str(), 253);
            map = 1;

            if (strcmp(argv[x], "-log") == 0)
            {
                z = x + 1;

                logfile[0] = 0;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                    strncpy(logfile, argv[z], 253);

                command_line_log = 1;
            }

            if (strcmp(argv[x], "-udt") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                    strncpy(udt_file, argv[z], 253);
            }

            if (strcmp(argv[x], "-c") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &altitude);
                    map = 1;
                    coverage = 1;
                    area_mode = 1;
                    max_txsites = 4;
                }
            }

            if (strcmp(argv[x], "-db") == 0 || strcmp(argv[x], "-dB") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0]) /* A minus argument is legal here */
                    sscanf(argv[z], "%d", &contour_threshold_);
            }

            if (strcmp(argv[x], "-p") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(terrain_file, argv[z], 253);
                    terrain_plot = 1;
                    pt2pt_mode = 1;
                }
            }

            if (strcmp(argv[x], "-e") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(elevation_file, argv[z], 253);
                    elevation_plot = 1;
                    pt2pt_mode = 1;
                }
            }

            if (strcmp(argv[x], "-h") == 0 || strcmp(argv[x], "-H") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(height_file, argv[z], 253);
                    height_plot = 1;
                    pt2pt_mode = 1;
                }

                if (strcmp(argv[x], "-H") == 0)
                    norm = 1;
                else
                    norm = 0;
            }

            if (strcmp(argv[x], "-metric") == 0)
                metric_ = 1;

            if (strcmp(argv[x], "-gpsav") == 0)
                gpsav_ = 1;

            if (strcmp(argv[x], "-geo") == 0)
                geo = 1;

            if (strcmp(argv[x], "-kml") == 0)
                kml = 1;

            if (strcmp(argv[x], "-nf") == 0)
                fresnel_plot = 0;

            if (strcmp(argv[x], "-ngs") == 0)
                ngs = 1;

            if (strcmp(argv[x], "-n") == 0)
                nolospath = 1;

            if (strcmp(argv[x], "-dbm") == 0)
                dbm_ = 1;

            if (strcmp(argv[x], "-sc") == 0)
                smooth_contours_ = 1;

            if (strcmp(argv[x], "-olditm") == 0)
                olditm_ = 1;

            if (strcmp(argv[x], "-N") == 0)
            {
                nolospath = 1;
                nositereports = 1;
            }

            if (strcmp(argv[x], "-d") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                    strncpy(sdf_path_, argv[z], 253);
            }
            if (strcmp(argv[x], "-t") == 0)
            {
                int z = x + 1;

                // Check if transmitter name is provided
                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(tx_site[0].name, argv[z], 49);

                    // Check for latitude
                    z++;
                    if (z <= y && argv[z][0] && argv[z][0] != '-')
                    {
                        sscanf(argv[z], "%lf", &tx_site[0].lat);

                        // Check for longitude
                        z++;
                        if (z <= y && argv[z][0] && argv[z][0] != '-')
                        {
                            sscanf(argv[z], "%lf", &tx_site[0].lon);

                            // First negate the longitude value if needed
                            tx_site[0].lon = -tx_site[0].lon;

                            // Then apply the same normalization as in LoadQTH
                            if (tx_site[0].lon < 0.0)
                                tx_site[0].lon += 360.0;

                            // Check for altitude
                            z++;
                            if (z <= y && argv[z][0] && argv[z][0] != '-')
                            {
                                // Add debug output before setting
                                printf("DEBUG: Altitude string from argv: %s\n", argv[z]);

                                // Parse altitude - knowing it's always in meters
                                double altitude_meters = 0.0;
                                int result = sscanf(argv[z], "%lf", &altitude_meters);

                                // Check if parsing was successful
                                if (result == 1)
                                {
                                    // Always convert from meters to feet
                                    tx_site[0].alt = altitude_meters * 3.28084;
                                    printf("DEBUG: Parsed altitude: %f meters = %f feet\n",
                                           altitude_meters, tx_site[0].alt);
                                }
                                else
                                {
                                    fprintf(stderr, "Error: Failed to parse altitude value: %s\n", argv[z]);
                                    fflush(stderr);
                                    exit(1);
                                }

                                // Initialize filename based on transmitter name
                                snprintf(tx_site[0].filename, 254, "%s.qth", tx_site[0].name);

                                // Validate parameters
                                if (tx_site[0].lat < -90.0 || tx_site[0].lat > 90.0)
                                {
                                    fprintf(stderr, "Error: Transmitter latitude must be between -90 and 90 degrees\n");
                                    fflush(stderr);
                                    exit(1);
                                }

                                // After normalization, longitude should be 0-360
                                if (tx_site[0].lon < 0.0 || tx_site[0].lon > 360.0)
                                {
                                    fprintf(stderr, "Error: Normalized transmitter longitude must be between 0 and 360 degrees\n");
                                    fflush(stderr);
                                    exit(1);
                                }

                                if (tx_site[0].alt < 0)
                                {
                                    fprintf(stderr, "Error: Transmitter altitude must be non-negative\n");
                                    fflush(stderr);
                                    exit(1);
                                }

                                // Increment txsites counter since we've successfully added a transmitter
                                txsites = 1;
                                printf("DEBUG: Final values - lat: %f, lon: %f, alt: %f\n",
                                       tx_site[0].lat, tx_site[0].lon, tx_site[0].alt);

                                x = z; // Update x to skip the processed arguments
                            }
                            else
                            {
                                fprintf(stderr, "Error: Altitude not specified after longitude\n");
                                fflush(stderr);
                                exit(1);
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: Longitude not specified after latitude\n");
                            fflush(stderr);
                            exit(1);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error: Latitude not specified after transmitter name\n");
                        fflush(stderr);
                        exit(1);
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Transmitter name not specified after -t\n");
                    fflush(stderr);
                    exit(1);
                }
            }


            if (strcmp(argv[x], "-L") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &altitudeLR);
                    map = 1;
                    LRmap = 1;
                    area_mode = 1;

                    if (coverage)
                        fprintf(stdout, "c and L are exclusive options, ignoring L.\n");
                }
            }
            if (strcmp(argv[x], "-trans") == 0)
            {
                fprintf(stdout, "trasnparent mode activated");
                fflush(stdout);
                transparent_mode_ = 1; // Enable transparency mode
            }
            if (strcmp(argv[x], "-LA") == 0)
            {
                int z = x + 1;

                // Check if altitude is provided
                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &altitudeLR);

                    // Check for start angle
                    z++;
                    if (z <= y && argv[z][0] && argv[z][0] != '-')
                    {
                        sscanf(argv[z], "%lf", &start_angle_);

                        // Check for end angle
                        z++;
                        if (z <= y && argv[z][0] && argv[z][0] != '-')
                        {
                            sscanf(argv[z], "%lf", &end_angle_);

                            // Enable specified angle mode
                            map = 1;
                            LRmap = 1;
                            area_mode = 1;
                            specified_angle_mode_ = 1; // New flag for -LA option

                            if (coverage)
                                fprintf(stdout,
                                        "specified_angle_mode is set in -L coverage settings.\n");
                        }
                        else
                        {
                            fprintf(stderr, "Error: End angle not specified after start angle.\n");
                            fflush(stderr);
                            exit(1);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error: Start angle not specified after altitude.\n");
                        fflush(stderr);
                        exit(1);
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Altitude not specified after -LA.\n");
                    fflush(stderr);
                    exit(1);
                }
            }

            if (strcmp(argv[x], "-l") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(longley_file, argv[z], 253);
                    longley_plot = 1;
                    pt2pt_mode = 1;
                }
            }

            if (strcmp(argv[x], "-r") == 0)
            {
                /* Read Receiver Location */

                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    strncpy(rxfile, argv[z], 253);
                    rx_site = LoadQTH(rxfile);
                    rxsite = 1;
                    pt2pt_mode = 1;
                }
            }

            if (strcmp(argv[x], "-s") == 0)
            {
                /* Read city file(s) */

                z = x + 1;

                while (z <= y && argv[z][0] && argv[z][0] != '-' && cities < 5)
                {
                    strncpy(city_file[cities], argv[z], 253);
                    cities++;
                    z++;
                }

                z--;
            }

            if (strcmp(argv[x], "-b") == 0)
            {
                /* Read Boundary File(s) */

                z = x + 1;

                while (z <= y && argv[z][0] && argv[z][0] != '-' && bfs < 5)
                {
                    strncpy(boundary_file[bfs], argv[z], 253);
                    bfs++;
                    z++;
                }

                z--;
            }

            if (strcmp(argv[x], "-f") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &forced_freq_);

                    if (forced_freq_ < 20.0)
                        forced_freq_ = 0.0;

                    if (forced_freq_ > 20.0e3)
                        forced_freq_ = 20.0e3;
                }
            }

            if (strcmp(argv[x], "-erp") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                {
                    sscanf(argv[z], "%lf", &forced_erp_);

                    if (forced_erp_ < 0.0)
                        forced_erp_ = -1.0;
                }
            }

            if (strcmp(argv[x], "-ano") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                    strncpy(ano_filename, argv[z], 253);
            }

            if (strcmp(argv[x], "-ani") == 0)
            {
                z = x + 1;

                if (z <= y && argv[z][0] && argv[z][0] != '-')
                    strncpy(ani_filename, argv[z], 253);
            }
        }

        /* Perform some error checking on the arguments
           and switches parsed from the command-line.
           If an error is encountered, print a message
           and exit gracefully. */
        if (tx_site[0].name == nullptr || tx_site[0].lat == 91.0 || tx_site[0].lon == 361.0 || tx_site[0].alt == 0.0)
        {
            fprintf(stderr, "\n%c*** ERROR: No transmitter site(s) specified!\n\n", 7);
            exit(-1);
        }

        for (x = 0, y = 0; x < txsites; x++)
        {
            if (tx_site[x].lat == 91.0 && tx_site[x].lon == 361.0)
            {
                fprintf(stderr, "\n*** ERROR: Transmitter site #%d not found!", x + 1);
                y++;
            }
        }

        if (y)
        {
            fprintf(stderr, "%c\n\n", 7);
            exit(-1);
        }

        if ((coverage + LRmap + ani_filename[0]) == 0 && rx_site.lat == 91.0 && rx_site.lon == 361.0)
        {
            if (max_range_ != 0.0 && txsites != 0)
            {
                /* Plot topographic map of radius "max_range_" */

                map = 0;
                topomap = 1;
            }

            else
            {
                fprintf(stderr, "\n%c*** ERROR: No receiver site found or specified!\n\n", 7);
                exit(-1);
            }
        }

        /* No major errors were detected.  Whew!  :-) */

        /* Adjust input parameters if -metric option is used */

        if (metric_)
        {
            altitudeLR /= METERS_PER_FOOT; /* meters --> feet */
            max_range_ /= KM_PER_MILE;      /* kilometers --> miles */
            altitude /= METERS_PER_FOOT;   /* meters --> feet */
            clutter_ /= METERS_PER_FOOT;    /* meters --> feet */
        }

        /* If no SDF path was specified on the command line (-d), check
           for a path specified in the $HOME/.splat_path file.  If the
           file is not found, then sdf_path[] remains NULL, and the
           current working directory is assumed to contain the SDF
           files. */

        if (sdf_path_[0] == 0)
        {
            env = getenv("HOME");
            snprintf(string_, 253, "%s/.splat_path", env);
            fd = fopen(string_, "r");

            if (fd != NULL)
            {
                fgets(string, 253, fd);

                /* Remove <CR> and/or <LF> from string */

                for (x = 0; string[x] != 13 && string[x] != 10 && string[x] != 0 && x < 253; x++)
                    ;
                string[x] = 0;

                strncpy(sdf_path_, string_, 253);

                fclose(fd);
            }
        }

        /* Ensure a trailing '/' is present in sdf_path */

        if (sdf_path_[0])
        {
            x = strlen(sdf_path_);

            if (sdf_path_[x - 1] != '/' && x != 0)
            {
                sdf_path_[x] = '/';
                sdf_path_[x + 1] = 0;
            }
        }

        fprintf(stdout, "%s", header);
        fflush(stdout);

        if (ani_filename[0])
        {
            ReadLRParm(tx_site[0], 0); /* Get ERP status */
            y = LoadANO(ani_filename);

            for (x = 0; x < txsites && x < max_txsites; x++)
                PlaceMarker(tx_site[x]);

            if (rxsite)
                PlaceMarker(rx_site);

            if (bfs)
            {
                for (x = 0; x < bfs; x++)
                    LoadBoundaries(boundary_file[x]);

                fprintf(stdout, "\n");
                fflush(stdout);
            }

            if (cities)
            {
                for (x = 0; x < cities; x++)
                    LoadCities(city_file[x]);

                fprintf(stdout, "\n");
                fflush(stdout);
            }
            if (LR_.erp == 0.0)
            {
                WritePPMDBM(mapfile, ngs, tx_site, txsites);
                //WritePPMLR(mapfile, geo, kml, ngs, tx_site, txsites);
            }
            else
            {
                if (dbm_)
                {
                    WritePPMDBM(mapfile, ngs, tx_site, txsites);
                }
                else
                {
                    WritePPMSS(mapfile, geo, kml, ngs, tx_site, txsites);
                }
            }

            exit(0);
        }

        x = 0;
        y = 0;

        min_lat = 90;
        max_lat = -90;

        min_lon = (int)floor(tx_site[0].lon);
        max_lon = (int)floor(tx_site[0].lon);

        for (y = 0, z = 0; z < txsites && z < max_txsites; z++)
        {
            txlat = (int)floor(tx_site[z].lat);
            txlon = (int)floor(tx_site[z].lon);

            if (txlat < min_lat)
                min_lat = txlat;

            if (txlat > max_lat)
                max_lat = txlat;

            if (LonDiff(txlon, min_lon) < 0.0)
                min_lon = txlon;

            if (LonDiff(txlon, max_lon) >= 0.0)
                max_lon = txlon;
        }

        if (rxsite)
        {
            rxlat = (int)floor(rx_site.lat);
            rxlon = (int)floor(rx_site.lon);

            if (rxlat < min_lat)
                min_lat = rxlat;

            if (rxlat > max_lat)
                max_lat = rxlat;

            if (LonDiff(rxlon, min_lon) < 0.0)
                min_lon = rxlon;

            if (LonDiff(rxlon, max_lon) >= 0.0)
                max_lon = rxlon;
        }

        /* Load the required SDF files */

        LoadTopoData(max_lon, min_lon, max_lat, min_lat);

        if (area_mode || topomap)
        {
            for (z = 0; z < txsites && z < max_txsites; z++)
            {
                /* "Ball park" estimates used to load any additional
                   SDF files required to conduct this analysis. */

                tx_range = sqrt(1.5 * (tx_site[z].alt + GetElevation(tx_site[z])));

                if (LRmap)
                    rx_range = sqrt(1.5 * altitudeLR);
                else
                    rx_range = sqrt(1.5 * altitude);

                /* deg_range determines the maximum
                   amount of topo data we read */

                deg_range = (tx_range + rx_range) / 57.0;

                /* max_range_ regulates the size of the
                   analysis.  A small, non-zero amount can
                   be used to shrink the size of the analysis
                   and limit the amount of topo data read by
                   SPLAT!  A large number will increase the
                   width of the analysis and the size of
                   the map. */

                if (max_range_ == 0.0)
                    max_range_ = tx_range + rx_range;

                deg_range = max_range_ / 57.0;

                /* Prevent the demand for a really wide coverage
                   from allocating more "pages" than are available
                   in memory. */

                switch (MAXPAGES)
                {
                case 1:
                    deg_limit = 0.125;
                    break;

                case 2:
                    deg_limit = 0.25;
                    break;

                case 4:
                    deg_limit = 0.5;
                    break;

                case 9:
                    deg_limit = 1.0;
                    break;

                case 16:
                    deg_limit = 1.5; /* WAS 2.0 */
                    break;

                case 25:
                    deg_limit = 2.0; /* WAS 3.0 */
                    break;

                case 36:
                    deg_limit = 2.5; /* New! */
                    break;

                case 49:
                    deg_limit = 3.0; /* New! */
                    break;

                case 64:
                    deg_limit = 3.5; /* New! */
                    break;
                }

                if (fabs(tx_site[z].lat) < 70.0)
                    deg_range_lon = deg_range / cos(DEG2RAD * tx_site[z].lat);
                else
                    deg_range_lon = deg_range / cos(DEG2RAD * 70.0);

                /* Correct for squares in degrees not being square in miles */

                if (deg_range > deg_limit)
                    deg_range = deg_limit;

                if (deg_range_lon > deg_limit)
                    deg_range_lon = deg_limit;

                north_min = (int)floor(tx_site[z].lat - deg_range);
                north_max = (int)floor(tx_site[z].lat + deg_range);

                west_min = (int)floor(tx_site[z].lon - deg_range_lon);

                while (west_min < 0)
                    west_min += 360;

                while (west_min >= 360)
                    west_min -= 360;

                west_max = (int)floor(tx_site[z].lon + deg_range_lon);

                while (west_max < 0)
                    west_max += 360;

                while (west_max >= 360)
                    west_max -= 360;

                if (north_min < min_lat)
                    min_lat = north_min;

                if (north_max > max_lat)
                    max_lat = north_max;

                if (LonDiff(west_min, min_lon) < 0.0)
                    min_lon = west_min;

                if (LonDiff(west_max, max_lon) >= 0.0)
                    max_lon = west_max;
            }

            /* Load any additional SDF files, if required */

            LoadTopoData(max_lon, min_lon, max_lat, min_lat);
        }

        if (udt_file[0])
            LoadUDT(udt_file);

        /***** Let the SPLATting begin! *****/

        if (pt2pt_mode)
        {
            PlaceMarker(rx_site);

            if (terrain_plot)
            {
                /* Extract extension (if present)
                   from "terrain_file" */

                y = strlen(terrain_file);

                for (x = y - 1; x > 0 && terrain_file[x] != '.'; x--)
                    ;

                if (x > 0) /* Extension found */
                {
                    for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++)
                        ext[z - (x + 1)] = tolower(terrain_file[z]);

                    ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
                    terrain_file[x] = 0;  /* Chop off extension */
                }

                else
                    strncpy(ext, "png\0", 4);
            }

            if (elevation_plot)
            {
                /* Extract extension (if present)
                   from "elevation_file" */

                y = strlen(elevation_file);

                for (x = y - 1; x > 0 && elevation_file[x] != '.'; x--)
                    ;

                if (x > 0) /* Extension found */
                {
                    for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++)
                        ext[z - (x + 1)] = tolower(elevation_file[z]);

                    ext[z - (x + 1)] = 0;  /* Ensure an ending 0 */
                    elevation_file[x] = 0; /* Chop off extension */
                }

                else
                    strncpy(ext, "png\0", 4);
            }

            if (height_plot)
            {
                /* Extract extension (if present)
                   from "height_file" */

                y = strlen(height_file);

                for (x = y - 1; x > 0 && height_file[x] != '.'; x--)
                    ;

                if (x > 0) /* Extension found */
                {
                    for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++)
                        ext[z - (x + 1)] = tolower(height_file[z]);

                    ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
                    height_file[x] = 0;   /* Chop off extension */
                }

                else
                    strncpy(ext, "png\0", 4);
            }

            if (longley_plot)
            {
                /* Extract extension (if present)
                   from "longley_file" */

                y = strlen(longley_file);

                for (x = y - 1; x > 0 && longley_file[x] != '.'; x--)
                    ;

                if (x > 0) /* Extension found */
                {
                    for (z = x + 1; z <= y && (z - (x + 1)) < 10; z++)
                        ext[z - (x + 1)] = tolower(longley_file[z]);

                    ext[z - (x + 1)] = 0; /* Ensure an ending 0 */
                    longley_file[x] = 0;  /* Chop off extension */
                }

                else
                    strncpy(ext, "png\0", 4);
            }

            for (x = 0; x < txsites && x < 4; x++)
            {
                PlaceMarker(tx_site[x]);

                if (nolospath == 0)
                {
                    switch (x)
                    {
                    case 0:
                        PlotPath(tx_site[x], rx_site, 1);
                        break;

                    case 1:
                        PlotPath(tx_site[x], rx_site, 8);
                        break;

                    case 2:
                        PlotPath(tx_site[x], rx_site, 16);
                        break;

                    case 3:
                        PlotPath(tx_site[x], rx_site, 32);
                    }
                }

                if (nositereports == 0)
                    SiteReport(mapfile, tx_site[x]);

                if (kml)
                    WriteKML(tx_site[x], rx_site);

                if (txsites > 1)
                    snprintf(string, 250, "%s-%c.%s%c", longley_file, '1' + x, ext, 0);
                else
                    snprintf(string, 250, "%s.%s%c", longley_file, ext, 0);

                if (nositereports == 0)
                {
                    if (longley_file[0] == 0)
                    {
                        ReadLRParm(tx_site[x], 0);
                        PathReport(tx_site[x], rx_site, string, 0);
                    }

                    else
                    {
                        ReadLRParm(tx_site[x], 1);
                        PathReport(tx_site[x], rx_site, string, longley_file[0]);
                    }
                }

                if (terrain_plot)
                {
                    if (txsites > 1)
                        snprintf(string, 250, "%s-%c.%s%c", terrain_file, '1' + x, ext, 0);
                    else
                        snprintf(string, 250, "%s.%s%c", terrain_file, ext, 0);
                    GraphTerrain(tx_site[x], rx_site, string);
                }

                if (elevation_plot)
                {
                    if (txsites > 1)
                        snprintf(string, 250, "%s-%c.%s%c", elevation_file, '1' + x, ext, 0);
                    else
                        snprintf(string, 250, "%s.%s%c", elevation_file, ext, 0);
                    GraphElevation(tx_site[x], rx_site, string);
                }

                if (height_plot)
                {
                    if (txsites > 1)
                        snprintf(string, 250, "%s-%c.%s%c", height_file, '1' + x, ext, 0);
                    else
                        snprintf(string, 250, "%s.%s%c", height_file, ext, 0);

                    GraphHeight(tx_site[x], rx_site, string, fresnel_plot, norm);
                }
            }
        }

        // THE HEAVY PART OF THE CODE
        if (area_mode && topomap == 0)
        {
            for (x = 0; x < txsites && x < max_txsites; x++)
            {
                if (coverage)
                {
                    PlotLOSMap(tx_site[x], altitude);
                }
                else if (!ReadCustomLRParm(tx_site[0], true))
                {
                    fprintf(stdout, "Using default LRP parameters since no valid LRP file found\n");
                }
                //else if (ReadLRParm(tx_site[x], 1))
                {
                    if (specified_angle_mode_)
                    {
                        // Call the new function for specified angles
                        PlotLRMapSpecifiedAngles(tx_site[x], altitudeLR, ano_filename, start_angle_,
                                                 end_angle_);
                    }
                    else
                    {
                        // Call the original function for full coverage
                        PlotLRMap(tx_site[x], altitudeLR, ano_filename);
                    }
                    // don't need to generate site report for each transmitter site
                    // SiteReport(mapfile, tx_site[x]);
                }
            }
        }

        if (map || topomap)
        {
            /* Label the map */

            if (cities)
            {
                for (y = 0; y < cities; y++)
                    LoadCities(city_file[y]);

                fprintf(stdout, "\n");
                fflush(stdout);
            }

            /* Load city and county boundary data files */

            if (bfs)
            {
                for (y = 0; y < bfs; y++)
                    LoadBoundaries(boundary_file[y]);

                fprintf(stdout, "\n");
                fflush(stdout);
            }

            /* Plot the map */

            if (coverage || pt2pt_mode || topomap)
            {

                WritePPM(mapfile, geo, kml, ngs, tx_site, txsites);
            }

            else
            {

                if (LR_.erp == 0.0)
                {
                    WritePPMLR(mapfile, geo, kml, ngs, tx_site, txsites);
                }
                else if (dbm_)
                {

                    WritePPMDBM(mapfile, ngs, tx_site, txsites);
                }
                else
                {

                    WritePPMSS(mapfile, geo, kml, ngs, tx_site, txsites);
                }
            }
        }

        if (command_line_log && strlen(logfile) > 0)
        {
            fd = fopen(logfile, "w");

            if (fd != NULL)
            {
                for (x = 0; x < argc; x++)
                    fprintf(fd, "%s ", argv[x]);

                fprintf(fd, "\n");

                fclose(fd);

                fprintf(stdout, "\nCommand-line parameter log written to: \"%s\"\n", logfile);
            }
        }

        printf("\n");
    }

    /* That's all, folks! */
//}

void SplatProcessor::process() {
    int y;
    char header[80];
    strncpy(splat_version_, "2.0.0\0", 6);

    if (HD_MODE == 1)
        strncpy(splat_name_, "SPLAT! HD\0", 10);
    else
        strncpy(splat_name_, "SPLAT!\0", 7);

    strncpy(dashes_, "---------------------------------------------------------------------------\0",
            76);

    if (argv_.size() == 1) {
        printHelp(splat_name_, splat_version_, y);
    }

    y = argv_.size() - 1;

    // Prepare header for welcome message
    prepareHeader(splat_name_, splat_version_, header);
    parseArguments(argv_.size(), argv_.data(), header, y);
    argv_.clear();
}

void SplatProcessor::resetSplat() {
    opened_ = 0,
    gpsav_ = 0,
    max_range_ = 0.0,
    forced_erp_ = -1.0,
    fzone_clearance_ = 0.6,
    min_north_ = 90,
    max_north_ = -90,
    min_west_ = 360,
    max_west_ = -1,
    max_elevation_ = -32768,
    min_elevation_ = 32768,
    metric_ = 0,
    dbm_ = 0,
    smooth_contours_ = 0,
    start_angle_ = 0.0,
    end_angle_ = 360.0,
    specified_angle_mode_ = 0,
    transparent_mode_ = 0,
    generatedImageInfo_ = SMSplatGenInfo();
    }

void SplatProcessor::setParameters(const SMSplatInputInfo &params) {
    // Stringify parameters
    argv_.push_back("splat");

    //  ====== Validate parameters ======
    // Transmitter site Check
    argv_.push_back("-t");
    if (params.transmitter_name != nullptr && strlen(params.transmitter_name) > 0) {
        argv_.push_back((char *)params.transmitter_name);
    } else {
        throw std::invalid_argument("Transmitter name must not be empty");
    }
    if (params.transmitter_lat < -90.0 || params.transmitter_lat > 90.0) {
        throw std::invalid_argument("Transmitter latitude must be between -90° (South Pole) and +90° (North Pole)");
    }
    argv_.push_back(strdup(std::to_string(params.transmitter_lat).c_str()));
    if (params.transmitter_lon < -180.0 || params.transmitter_lon > 180.0) {
        throw std::invalid_argument("Transmitter longitude must be between -180° (West) and +180° (East)");
    }
    argv_.push_back(strdup(std::to_string(params.transmitter_lon).c_str())); 
    if (params.transmitter_alt < 0) {
        throw std::invalid_argument("Transmitter altitude must be non-negative");
    }
    argv_.push_back(strdup(std::to_string(params.transmitter_alt).c_str()));


    // Angle range Check
    if (params.start_angle < 0 || params.start_angle > 360 || params.end_angle < 0 ||
        params.end_angle > 360 || params.start_angle > params.end_angle) {
        throw std::invalid_argument("Invalid angle range");
    }

    // Frequency Check
    if (params.frequency >= 20.0 && params.frequency <= 20000.0) {
        argv_.push_back("-f");
        std::string frequencyStr = std::to_string(params.frequency);
        argv_.push_back(strdup(frequencyStr.c_str()));
    } else {
        throw std::invalid_argument("Frequency must be between 20 MHz and 20 GHz");
    }

    // Receiver height Check

    if (params.receiver_height < 0) {
        throw std::invalid_argument("Receiver height must be non-negative");
    }


    if (params.start_angle == 0.0 && params.end_angle == 360.0) {
        argv_.push_back("-L");
        std::string receiverHeightStr = std::to_string(params.receiver_height);
        argv_.push_back(strdup(receiverHeightStr.c_str()));
    } else if (params.start_angle > 0.0 || params.end_angle < 360.0) {
        argv_.push_back("-LA");
        std::string receiverHeightStr = std::to_string(params.receiver_height);
        argv_.push_back(strdup(receiverHeightStr.c_str()));

        std::string startAngleStr = std::to_string(params.start_angle);
        argv_.push_back(strdup(startAngleStr.c_str()));

        std::string endAngleStr = std::to_string(params.end_angle);
        argv_.push_back(strdup(endAngleStr.c_str()));
    }

    // Add other flags based on booleans (dbm, olditm, sc, ngs)
    argv_.push_back("-dbm");
    argv_.push_back("-olditm");
    argv_.push_back("-sc");
    argv_.push_back("-ngs");
    argv_.push_back("-metric");

    // Add radius, frequency, and fresnel_zone if provided
    if (params.radius >= 0.0 && params.radius <= 1000.0) {
        argv_.push_back("-R");
        std::string radiusStr = std::to_string(params.radius);
        argv_.push_back(strdup(radiusStr.c_str()));
    } else {
        throw std::invalid_argument("Radius must be between 0.0 and 1000.0");
    }


    if (params.fresnel_zone >= 0.0 && params.fresnel_zone <= 100.0) {
        argv_.push_back("-fz");
        std::string fresnelZoneStr = std::to_string(params.fresnel_zone);
        argv_.push_back(strdup(fresnelZoneStr.c_str()));
    } else {
        throw std::invalid_argument("Fresnel zone must be between 0.0 and 100.0");
    }

    // Elevation path Check
    if (!std::filesystem::exists(elevFilePath_)) {
        throw std::invalid_argument("Elevation path does not exist");
    } else {
        argv_.push_back("-d");
        argv_.push_back(strdup(elevFilePath_.c_str()));
    }

    // Log the final argv for debugging
    for (int i = 0; i < argv_.size(); ++i) {
        std::cout << "argv[" << i << "] = " << argv_[i] << std::endl;
    }
    updateCoverageInfo(params.transmitter_name, params.transmitter_lat, params.transmitter_lon, params.radius);
}
