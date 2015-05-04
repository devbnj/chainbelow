/*
 * Panchanga.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: DevbNJ
 */

#include "arduino.h";
#include <math.h>
#include "Panchanga.h"
#include "Corr.h"

// ----- Reduce to 360 degrees
double Panchanga::fix360(double v) {
  while (v < 0.0)
    v += 360.0;
  while (v > 360.0)
    v -= 360.0;
  return v;
}

//-----------------------------------------------------
// Nutation
//-----------------------------------------------------
double Panchanga::nutation(double jd) {
  double t = (jd - 2415020) / 36525;
  double t2 = t * t;
  double ls = 279.6967 + 36000.7689 * t + 0.000303 * t2;
  double l = 270.4341639 + 481267.8831417 * t - 0.0011333333 * t2;
  double ms = 358.4758333333334 + 35999.04974999958 * t
              - t2 * 1.500000059604645e-4;
  double ml = 296.1046083333757 + 477198.8491083336 * t
              + 0.0091916667090522 * t2;
  double d = 350.7374861110581 + 445267.1142166667 * t
             - t2 * 1.436111132303874e-3;
  double om = 259.1832750002543 - 1934.142008333206 * t + .0020777778 * t2;
  ls *= d2r;
  l *= d2r;
  ms *= d2r;
  ml *= d2r;
  d *= d2r;
  om *= d2r;
  double d2 = d * d;
  double l2 = l * l;
  double ls2 = ls * ls;
  double nut = (-17.2327 - 0.01737 * t) * sin(om);
  nut += 0.2088 * sin(2.0 * om);
  nut += 0.0675 * sin(ml);
  nut -= 0.0149 * sin(ml - d2);
  nut -= 0.0342 * sin(l2 - om);
  nut += 0.0114 * sin(l2 - ml);
  nut -= 0.2037 * sin(l2);
  nut -= 0.0261 * sin(l2 + ml);
  nut += 0.0124 * sin(ls2 - om);
  nut += 0.0214 * sin(ls2 - ms);
  nut -= 1.2729 * sin(ls2);
  nut -= 0.0497 * sin(ls2 + ms);
  nut += 0.1261 * sin(ms);
  nut = nut / 3600.0;
  return nut;
}
//-----------------------------------------------------
//Ayanamsa (deg.)
//-----------------------------------------------------
double Panchanga::computeAyanamsa(double jd) {
  double t = (jd - 2415020) / 36525;
  double om = 259.183275 - 1934.142008333206 * t + 0.0020777778 * t * t
              + 0.0000022222222 * t * t * t;
  double ls = 279.696678 + 36000.76892 * t + 0.0003025 * t * t;
  double aya = 17.23 * sin(d2r * om) + 1.27 * sin(d2r * ls * 2)
               - (5025.64 + 1.11 * t) * t;
  aya = (aya - 80861.27) / 3600.0; // 84038.27 = Fagan-Bradley, 80861.27 = Lahiri
  return aya;
}

//-----------------------------------------------------------------------------------
//Calculation Geocentric ecliptic longitude of Moon and angular speed.
//(accuracy 2 sec. of long.)
//-----------------------------------------------------------------------------------
double Panchanga::computeMoon(double jd) {
  Corr corrMoon[93] = { { 0, 0, 0, 4, 13.902 }, { 0, 0, 0, 2, 2369.912 }, {
      1,
      0, 0, 4, 1.979
    }, { 1, 0, 0, 2, 191.953 },
    { 1, 0, 0, 0, 22639.500 }, { 1, 0, 0, -2, -4586.465 }, { 1, 0, 0,
      -4, -38.428
    }, { 1, 0, 0, -6, -0.393 },
    { 0, 1, 0, 4, -0.289 }, { 0, 1, 0, 2, -24.420 }, { 0, 1, 0, 0,
      -668.146
    }, { 0, 1, 0, -2, -165.145 },
    { 0, 1, 0, -4, -1.877 }, { 0, 0, 0, 3, 0.403 }, { 0, 0, 0, 1,
      -125.154
    }, { 2, 0, 0, 4, 0.213 }, { 2, 0, 0, 2, 14.387 }, {
      2, 0, 0, 0, 769.016
    }, { 2, 0, 0, -2, -211.656 }, {
      2, 0, 0,
      -4, -30.773
    }, { 2, 0, 0, -6, -0.570 },
    { 1, 1, 0, 2, -2.921 }, { 1, 1, 0, 0, -109.673 }, { 1, 1, 0, -2,
      -205.962
    }, { 1, 1, 0, -4, -4.391 }, { 1, -1, 0, 4, 0.283 },
    { 1, -1, 0, 2, 14.577 }, { 1, -1, 0, 0, 147.687 }, { 1, -1, 0, -2,
      28.475
    }, { 1, -1, 0, -4, 0.636 }, { 0, 2, 0, 2, -0.189 }, {
      0, 2, 0, 0, -7.486
    }, { 0, 2, 0, -2, -8.096 }, {
      0, 0, 2, 2,
      -5.741
    }, { 0, 0, 2, 0, -411.608 },
    { 0, 0, 2, -2, -55.173 }, { 0, 0, 2, -4, 0.025 }, { 1, 0, 0, 1,
      -8.466
    }, { 1, 0, 0, -1, 18.609 }, { 1, 0, 0, -3, 3.215 }, {
      0, 1, 0, 1, 18.023
    }, { 0, 1, 0, -1, 0.560 }, {
      3, 0, 0, 2,
      1.060
    }, { 3, 0, 0, 0, 36.124 }, { 3, 0, 0, -2, -13.193 }, {
      3, 0, 0, -4, -1.187
    }, { 3, 0, 0, -6, -0.293 }, {
      2, 1, 0,
      2, -0.290
    }, { 2, 1, 0, 0, -7.649 },
    { 2, 1, 0, -2, -8.627 }, { 2, 1, 0, -4, -2.740 }, { 2, -1, 0, 2,
      1.181
    }, { 2, -1, 0, 0, 9.703 }, { 2, -1, 0, -2, -2.494 }, {
      2, -1, 0, -4, 0.360
    }, { 1, 2, 0, 0, -1.167 }, {
      1, 2, 0,
      -2, -7.412
    }, { 1, 2, 0, -4, -0.311 },
    { 1, -2, 0, 2, 0.757 }, { 1, -2, 0, 0, 2.580 }, { 1, -2, 0, -2,
      2.533
    }, { 0, 3, 0, -2, -0.344 }, { 1, 0, 2, 2, -0.992 }, {
      1, 0, 2, 0, -45.099
    }, { 1, 0, 2, -2, -0.179 }, {
      1, 0, -2,
      2, -6.382
    }, { 1, 0, -2, 0, 39.528 },
    { 1, 0, -2, -2, 9.366 }, { 0, 1, 2, 0, 0.415 }, { 0, 1, 2, -2,
      -2.152
    }, { 0, 1, -2, 2, -1.440 }, { 0, 1, -2, -2, 0.384 },
    { 2, 0, 0, 1, -0.586 }, { 2, 0, 0, -1, 1.750 },
    { 2, 0, 0, -3, 1.225 }, { 1, 1, 0, 1, 1.267 }, { 1, -1, 0, -1,
      -1.089
    }, { 0, 0, 2, -1, 0.584 }, { 4, 0, 0, 0, 1.938 }, {
      4, 0, 0, -2, -0.952
    }, { 3, 1, 0, 0, -0.551 }, {
      3, 1, 0,
      -2, -0.482
    }, { 3, -1, 0, 0, 0.681 },
    { 2, 0, 2, 0, -3.996 }, { 2, 0, 2, -2, 0.557 }, { 2, 0, -2, 2,
      -0.459
    }, { 2, 0, -2, 0, -1.298 }, { 2, 0, -2, -2, 0.538 },
    { 1, 1, -2, -2, 0.426 }, { 1, -1, 2, 0, -0.304 }, { 1, -1, -2, 2,
      -0.372
    }, { 0, 0, 4, 0, 0.418 }, { 2, -1, 0, -1, -0.352 }
  };
  Corr2 corrMoon2[27] = { { 0.127, 0, 0, 0, 6 }, { -0.151, 0, 2, 0, -4 }, {
      -0.085, 0, 0, 2, 4
    }, { 0.150, 0, 1, 0, 3 },
    { -0.091, 2, 1, 0, -6 }, { -0.103, 0, 3, 0, 0 }, { -0.301, 1, 0, 2,
      -4
    }, { 0.202, 1, 0, -2, -4 }, { 0.137, 1, 1, 0, -1 }, {
      0.233, 1, 1, 0, -3
    }, { -0.122, 1, -1, 0, 1 }, {
      -0.276, 1,
      -1, 0, -3
    }, { 0.255, 0, 0, 2, 1 }, { 0.254, 0, 0, 2, -3 },
    { -0.100, 3, 1, 0, -4 }, { -0.183, 3, -1, 0, -2 }, { -0.297, 2, 2,
      0, -2
    }, { -0.161, 2, 2, 0, -4 }, { 0.197, 2, -2, 0, 0 }, {
      0.254, 2, -2, 0, -2
    }, { -0.250, 1, 3, 0, -2 }, {
      -0.123, 2,
      0, 2, 2
    }, { 0.173, 2, 0, -2, -4 }, { 0.263, 1, 1, 2, 0 }, {
      0.130, 3, 0, 0, -1
    }, { 0.113, 5, 0, 0, 0 }, {
      0.092, 3, 0,
      2, -2
    }
  };

  // Days from epoch 1900:
  double tdays = jd - 2415020.0;
  // Time in Julian centuries from epoch 1900:
  double t = tdays / 36525.0;
  double t2 = t * t;
  double t3 = t * t * t;
  // inclination of an ecliptic to equator:
  double ob = 23.452294 - 0.0130125 * t - 0.00000164 * t2 + 0.000000503 * t3;
  // moon's mean longitude:
  double l = 270.4337361 + 13.176396544528099 * tdays - 5.86 * t2 / 3600
             + 0.0068 * t3 / 3600;
  // elongation of moon from sun:
  double d = 350.7374861110581 + 445267.1142166667 * t
             - t2 * 1.436111132303874e-3 + 0.0000018888889 * t3;
  //	moon's perihel:
  double pe = 334.329556 + 14648522.52 * t / 3600 - 37.17 * t2 / 3600
              - 0.045 * t3 / 3600;
  //	sun's mean anomaly:
  double ms = 358.4758333333334 + 35999.04974999958 * t
              - t2 * 1.500000059604645e-4 - t3 * 3.3333333623078e-6;
  //	moon's mean anomaly:
  double ml = fix360(l - pe);
  //	moon's mean node:
  double om = 259.183275 - 6962911.23 * t / 3600 + 7.48 * t2 / 3600
              + 0.008 * t3 / 3600;
  //	moon's mean longitude, counted from node:
  double f = fix360(l - om);
  //		The periodic corrections:
  double r2rad = 360.0 * d2r;
  double tb = tdays * 1e-12;
  double t2c = tdays * tdays * 1e-16; // *10^16
  double a1 = sin(r2rad * (0.53733431 - 10104982 * tb + 191 * t2c));
  double a2 = sin(r2rad * (0.71995354 - 147094228 * tb + 43 * t2c));
  double c2 = cos(r2rad * (0.71995354 - 147094228 * tb + 43 * t2c));
  double a3 = sin(r2rad * (0.14222222 + 1536238 * tb));
  double a4 = sin(r2rad * (0.48398132 - 147269147 * tb + 43 * t2c));
  double c4 = cos(r2rad * (0.48398132 - 147269147 * tb + 43 * t2c));
  double a5 = sin(r2rad * (0.52453688 - 147162675 * tb + 43 * t2c));
  double a6 = sin(r2rad * (0.84536324 - 11459387 * tb));
  double a7 = sin(r2rad * (0.23363774 + 1232723 * tb + 191 * t2c));
  double a8 = sin(r2rad * (0.58750000 + 9050118 * tb));
  double a9 = sin(r2rad * (0.61043085 - 67718733 * tb));
  double dlm = 0.84 * a3 + 0.31 * a7 + 14.27 * a1 + 7.261 * a2 + 0.282 * a4
               + 0.237 * a6;
  double dpm = -2.1 * a3 - 2.076 * a2 - 0.840 * a4 - 0.593 * a6;
  double dkm = 0.63 * a3 + 95.96 * a2 + 15.58 * a4 + 1.86 * a5;
  double dls = -6.4 * a3 - 0.27 * a8 - 1.89 * a6 + 0.20 * a9;
  double dgc = (-4.318 * c2 - 0.698 * c4) / 3600.0 / 360.0;

  dgc = (1.000002708 + 139.978 * dgc);
  ml = d2r * (ml + (dlm - dpm) / 3600.0); // moon's mean anomaly
  ms = d2r * (ms + dls / 3600.0); // sun's mean anomaly
  f = d2r * (f + (dlm - dkm) / 3600.0); // moon's mean longitude, counted from
  // node
  d = d2r * (d + (dlm - dls) / 3600.0); // elongation of moon from sun

  double lk = 0;
  double lk1 = 0;
  double sk = 0;
  double sinp = 0;
  double nib = 0;
  double g1c = 0;

  double i1corr = 1.0 - 6.8320e-8 * tdays;
  double i2corr = dgc * dgc;
  uint8_t i;
  for (i = 0; i < 93; i++) { // indignation in a longitude
    double arg = corrMoon[i].mlcor * ml + corrMoon[i].mscor * ms
                 + corrMoon[i].fcor * f + corrMoon[i].dcor * d;
    double sinarg = sin(arg);
    if (corrMoon[i].mscor != 0) {
      sinarg *= i1corr;
      if (corrMoon[i].mscor == 2 || corrMoon[i].mscor == -2)
        sinarg *= i1corr;
    }
    if (corrMoon[i].fcor != 0)
      sinarg *= i2corr;
    lk += corrMoon[i].lcor * sinarg;
  }

  for (i = 0; i < 27; i++) { // indignation in a longitude additional
    double arg = corrMoon2[i].ml * ml + corrMoon2[i].ms * ms
                 + corrMoon2[i].f * f + corrMoon2[i].d * d;
    double sinarg = sin(arg);
    lk1 += corrMoon2[i].l * sinarg;
  }
  //		Indignation from planets:
  double dlid = 0.822 * sin(r2rad * (0.32480 - 0.0017125594 * tdays));
  dlid += 0.307 * sin(r2rad * (0.14905 - 0.0034251187 * tdays));
  dlid += 0.348 * sin(r2rad * (0.68266 - 0.0006873156 * tdays));
  dlid += 0.662 * sin(r2rad * (0.65162 + 0.0365724168 * tdays));
  dlid += 0.643 * sin(r2rad * (0.88098 - 0.0025069941 * tdays));
  dlid += 1.137 * sin(r2rad * (0.85823 + 0.0364487270 * tdays));
  dlid += 0.436 * sin(r2rad * (0.71892 + 0.0362179180 * tdays));
  dlid += 0.327 * sin(r2rad * (0.97639 + 0.0001734910 * tdays));
  l = l + nutation(jd) + (dlm + lk + lk1 + dlid) / 3600.0;
  lMoonYoga = l;

  l = fix360(l);
  //		Moon's angular speed (deg/day):
  double vl = 13.176397;
  vl = vl + 1.434006 * cos(ml);
  vl = vl + .280135 * cos(2 * d);
  vl = vl + .251632 * cos(2 * d - ml);
  vl = vl + .09742 * cos(2 * ml);
  vl = vl - .052799 * cos(2 * f);
  vl = vl + .034848 * cos(2 * d + ml);
  vl = vl + .018732 * cos(2 * d - ms);
  vl = vl + .010316 * cos(2 * d - ms - ml);
  vl = vl + .008649 * cos(ms - ml);
  vl = vl - .008642 * cos(2 * f + ml);
  vl = vl - .007471 * cos(ms + ml);
  vl = vl - .007387 * cos(d);
  vl = vl + .006864 * cos(3 * ml);
  vl = vl + .00665 * cos(4 * d - ml);
  vl = vl + .003523 * cos(2 * d + 2 * ml);
  vl = vl + .003377 * cos(4 * d - 2 * ml);
  vl = vl + .003287 * cos(4 * d);
  vl = vl - .003193 * cos(ms);
  vl = vl - .003003 * cos(2 * d + ms);
  vl = vl + .002577 * cos(ml - ms + 2 * d);
  vl = vl - .002567 * cos(2 * f - ml);
  vl = vl - .001794 * cos(2 * d - 2 * ml);
  vl = vl - .001716 * cos(ml - 2 * f - 2 * d);
  vl = vl - .001698 * cos(2 * d + ms - ml);
  vl = vl - .001415 * cos(2 * d + 2 * f);
  vl = vl + .001183 * cos(2 * ml - ms);
  vl = vl + .00115 * cos(d + ms);
  vl = vl - .001035 * cos(d + ml);
  vl = vl - .001019 * cos(2 * f + 2 * ml);
  vl = vl - .001006 * cos(ms + 2 * ml);

  skar = vl;
  return l;
}

//-----------------------------------------------------
//Solution of the Kepler equation (in radians)
//-----------------------------------------------------
double Panchanga::kepler(double m, double xe, double re) {
  m *= d2r;
  double u0 = m;
  re *= d2r;
  double delta = 1;
  while (abs(delta) >= re) {
    delta = (m + xe * sin(u0) - u0) / (1 - xe * cos(u0));
    u0 += delta;
  }
  return u0;
}

//----------------------------------------------------------------------
//Calculation Geocentric ecliptic longitude of Sun
//(accuracy 1 sec. of long.)
//----------------------------------------------------------------------
double Panchanga::computeSun(double jd) {
  //	Days from epoch 1900:
  double tdays = jd - 2415020;
  //	Time in Julian centuries from epoch 1900:
  double t = tdays / 36525;
  double t2 = t * t;
  double t3 = t * t * t;
  //	sun's mean longitude:
  double ls = 279.696678 + 0.9856473354 * tdays + 1.089 * t2 / 3600;
  //	sun's perihel:
  double pes = 101.220833 + 6189.03 * t / 3600 + 1.63 * t2 / 3600
               + 0.012 * t3 / 3600;
  //	sun's mean anomaly:
  double ms = fix360(ls - pes + 180);
  //	longperiodic terms:
  double g = ms
             + (0.266 * sin((31.8 + 119.0 * t) * d2r)
                + 6.40 * sin((231.19 + 20.2 * t) * d2r)
                + (1.882 - 0.016 * t) * sin((57.24 + 150.27 * t) * d2r))
             / 3600.0;
  //	sun's mean longitude:
  double oms = 259.18 - 1934.142 * t;
  //	excentricity of earth orbit:
  double ex = 0.01675104 - 0.0000418 * t - 0.000000126 * t2;
  //	moon's mean longitude:
  double l = 270.4337361 + 13.176396544528099 * tdays - 5.86 * t2 / 3600
             + 0.0068 * t3 / 3600;
  //	moon's mean anomaly:
  double ml = 296.1046083333757 + 477198.8491083336 * t
              + 0.0091916667090522 * t2 + 0.0000143888893 * t3;
  //	mean longitude of earth:
  double le = 99.696678 + 0.9856473354 * tdays + 1.089 * t2 / 3600;
  //	moon's mean node longitude:
  double om = 259.183275 - 6962911.23 * t / 3600 + 7.48 * t2 / 3600
              + 0.008 * t3 / 3600;
  //	the Kepler equation:
  double u = kepler(g, ex, 0.0000003);

  //	sun's true anomaly:
  double b = sqrt((1 + ex) / (1 - ex));
  double truanom = 0;
  if (abs(pi - u) < 1.0e-10)
    truanom = u;
  else
    truanom = 2.0 * atan(b * tan(u / 2.0));
  truanom = fix360(truanom * r2d);
  double u1 = (153.23 + 22518.7541 * t) * d2r;
  double u2 = (216.57 + 45037.5082 * t) * d2r;
  double u3 = (312.69 + 32964.3577 * t) * d2r;
  double u4 = (350.74 + 445267.1142 * t - 0.00144 * t2) * d2r;
  double u6 = (353.4 + 65928.71550000001 * t) * d2r;
  double u5 = (315.6 + 893.3 * t) * d2r;
  double dl = 0.00134 * cos(u1);
  dl += 0.00154 * cos(u2);
  dl += 0.002 * cos(u3);
  dl += 0.00179 * sin(u4);
  dl += 0.202 * sin(u5) / 3600;
  double dr = 0.00000543 * sin(u1);
  dr += 0.00001575 * sin(u2);
  dr += 0.00001627 * sin(u3);
  dr += 0.00003076 * cos(u4);
  dr += 9.26999999e-06 * sin(u6);
  //		sun's true longitude (deg.):
  double il = ls + dl + truanom - ms;
  //		aberracion (deg):
  double r1 = 1.0000002 * (1 - ex * ex) / (1 + ex * cos(truanom * d2r));
  double rs = r1 + dr; // radius-vector
  double ab = (20.496 * (1 - ex * ex) / rs) / 3600.0;
  ls = il + nutation(jd) - ab; // visible longitude of sun
  lSunYoga = ls;
  ls = fix360(ls);
  return ls;
}

//------------------------------------------------------------------------------------------
//Julian day from calendar day
//------------------------------------------------------------------------------------------
double Panchanga::mdy2julian(long m, long d, long y) {
  unsigned long im = (12 * y);
  im += 57600;
  im = im + m - 3;
  double j1 = (2 * (im - floor(im / 12) * 12) + 7 + 365 * im) / 12;
  double j = (floor(j1) + d + floor(im / 48.0)) - 32083;
  if (j > 2299171.0) {
    j += floor(im / 4800.0);
    j = j - floor(im / 1200.0) + 38.0;
  }
  j -= 0.5;
  return j;
}
//-----------------------------------------------------------------------------------------
//Returns delta t (in julian days) from universal time (h.)
//-----------------------------------------------------------------------------------------
double Panchanga::dTime(double jd) {
  //	delta t from 1620 to 2010 (sec.):
  double efdt[] = { 124.0, 85.0, 62.0, 48.0, 37.0, 26.0, 16.0, 10.0, 9.0,
                    10.0, 11.0, 11.0, 12.0, 13.0, 15.0, 16.0, 17.0, 17.0, 13.7, 12.5,
                    12.0, 7.5, 5.7, 7.1, 7.9, 1.6, -5.4, -5.9, -2.7, 10.5, 21.2, 24.0,
                    24.3, 29.2, 33.2, 40.2, 50.5, 56.9, 65.7, 75.5
                  };
  // devb hold onto this string
  calData(jd);
  double dgod = kyear + (kmonth - 1) / 12.0 + (kday - 1) / 365.25;
  double t = (jd - 2378497.0) / 36525.0;
  unsigned long i1 = 0;
  double di = 0;
  double dt = 0;
  if (dgod >= 1620 && dgod < 2010) {
    i1 = floor((dgod - 1620.0) / 10.0);
    di = dgod - (1620.0 + i1 * 10.0);
    dt = (efdt[i1] + ((efdt[i1 + 1] - efdt[i1]) * di) / 10.0);
  } else {
    if (dgod >= 2010)
      dt = 25.5 * t * t - 39.0;
    if (dgod >= 948 && dgod < 1620)
      dt = 25.5 * t * t;
    if (dgod < 948)
      dt = 1361.7 + 320.0 * t + 44.3 * t * t;
  }
  dt /= 3600.0;
  return dt;
}
//------------------------------------------------------------------------------------------
//Calendar day from Julian Day
//------------------------------------------------------------------------------------------
void Panchanga::calData(double jd) {
  char s[16] = "";
  double z1 = jd + 0.5;
  unsigned long z2 = floor(z1);
  double f = z1 - z2;
  double a = 0;
  double alf = 0;
  if (z2 < 2299161.0)
    a = z2;
  else {
    alf = floor((z2 - 1867216.25) / 36524.25);
    a = z2 + 1 + alf - floor(alf / 4);
  }
  double b = a + 1524;
  double c = floor((b - 122.1) / 365.25);
  double d = floor(365.25 * c);
  double e = floor((b - d) / 30.6001);
  double days = b - d - floor(30.6001 * e) + f;
  kday = floor(days);
  if (e < 13.5)
    kmonth = e - 1;
  else
    kmonth = e - 13;
  if (kmonth > 2.5)
    kyear = c - 4716;
  if (kmonth < 2.5)
    kyear = c - 4715;
  long hh1 = (days - kday) * 24;
  khour = floor(hh1);
  kminute = hh1 - khour;
  ksecond = kminute * 60;
  kminute = floor(ksecond);
  ksecond = floor((ksecond - kminute) * 60);
  /*
   strcat(s, kmonth);
   strcat(s, "/");
   strcat(s, kday);
   strcat(s, "/");
   strcat(s, kyear);
   strcat(s, " ");
   strcat(s, khour);
   strcat(s, ":");
   strcat(s, kminute);
   return s;
   */
}

unsigned int Panchanga::tithi(double jd, long n1, double tzone, int len) {
  unsigned int s_t = 0;
  int flag;
  double lSun0 = 0.0;
  double lMoon0 = 0.0;
  double jdt = jd;
  long knv = floor(((jd - 2415020) / 365.25) * 12.3685);
  unsigned int itit;
  for (itit = n1; itit < (n1 + 2); ++itit) {
    int aspect = len * itit;
    flag = 0;
    if (aspect == 0) {
      jdt = novolun(jd, knv);
      flag = 1;
    }
    if (aspect == 360) {
      jdt = novolun(jd, (knv + 1));
      flag = 1;
    }
    while (flag < 1) {
      lSun0 = computeSun(jdt);
      lMoon0 = computeMoon(jdt);
      double a = fix360(lSun0 + aspect);
      double asp1 = a - lMoon0;
      if (asp1 > 180)
        asp1 -= 360;
      if (asp1 < -180)
        asp1 += 360;
      flag = 1;
      if (abs(asp1) > 0.001) {
        jdt += (asp1 / (skar - 1));
        flag = 0;
      }
    }
    // if (itit == n1) s_t = calData(jdt + tzone / 24);
    // if (itit == (n1 + 1)) s_t += "&nbsp;" + calData(jdt + tzone / 24);
  }
  return s_t;
}

double Panchanga::novolun(double jd, long knv) {
  double t = (jd - 2415020) / 36525;
  double t2 = t * t;
  double t3 = t * t * t;
  double jdnv = 2415020.75933 + 29.53058868 * knv + 0.0001178 * t2
                - 0.000000155 * t3;
  jdnv += 0.00033 * sin((166.56 + 132.87 * t - 0.009173 * t2) * d2r);
  double m = 359.2242 + 29.10535608 * knv - 0.0000333 * t2 - 0.00000347 * t3;
  double ml = 306.0253 + 385.81691806 * knv + 0.0107306 * t2
              + 0.00001236 * t3;
  double f = 21.2964 + 390.67050646 * knv - 0.0016528 * t2 - 0.00000239 * t3;
  m *= d2r;
  ml *= d2r;
  f *= d2r;
  double djd = (0.1734 - 0.000393 * t) * sin(m);
  djd += 0.0021 * sin(2 * m);
  djd -= 0.4068 * sin(ml);
  djd += 0.0161 * sin(2 * ml);
  djd -= 0.0004 * sin(3 * ml);
  djd += 0.0104 * sin(2 * f);
  djd -= 0.0051 * sin(m + ml);
  djd -= 0.0074 * sin(m - ml);
  djd += 0.0004 * sin(2 * f + m);
  djd -= 0.0004 * sin(2 * f - m);
  djd -= 0.0006 * sin(2 * f + ml);
  djd += 0.001 * sin(2 * f - ml);
  djd += 0.0005 * sin(m + 2 * ml);
  jdnv += djd;
  return jdnv;
}

void Panchanga::initialize_panchanga(uint8_t dd, uint8_t mm, int yy, uint8_t hi, uint8_t mi, int tz) {
  n_tithi = 1;
  n_naksh = 1;
  ayanamsa = 0.0;
  unsigned int day = dd;
  unsigned int mon = mm;
  unsigned int year = yy;
  double hr = hi;
  unsigned int inpmin = mi;
  hr += inpmin / 60.0;
  double hh = hr;
  double tzone = tz;

  double dayhr = day + hr / 24;
  long n_wday = 0;

  double jd0 = mdy2julian(mon, day, year);
  double jdut = jd0 + ((hr - tzone) / 24.0);
  double dt = dTime(jdut);
  double jd = jdut + dt / 24.0;
  ayanamsa = computeAyanamsa(jd);
  Serial.print("PCH, Ayanamsa: ");
  Serial.println(ayanamsa);
  lMoon = computeMoon(jd);
  Serial.print("PCH, Moon: ");
  Serial.println(lMoon);
  lMoon1 = computeMoon(jd);
  lSun = computeSun(jd);
  Serial.print("PCH, Sun: ");
  Serial.println(lSun);
  double dmoonYoga = (lMoonYoga + ayanamsa - 491143.07698973856);
  double dsunYoga = (lSunYoga + ayanamsa - 36976.91240579201);
  double zyoga = dmoonYoga + dsunYoga;
  double n_yoga1 = zyoga * 6 / 80;
  while (n_yoga1 < 0)
    n_yoga1 += 27;
  while (n_yoga1 > 27)
    n_yoga1 -= 27;
  double n3 = n_yoga;
  n_yoga = floor(n_yoga1);
  Serial.print("PCH, Yoga: ");
  Serial.println(n_yoga);
  // double s_yoga = yoga(jd, zyoga, tzone);
  double lMoon0 = fix360(lMoon + ayanamsa);
  n_naksh = floor(lMoon0 * 6 / 80);
  Serial.print("PCH, Nakshatra:");
  Serial.println(n_naksh);
  // double s_naksh = nakshatra(jd, n_naksh, tzone);
  lMoon0 = lMoon;
  double lSun0 = lSun;
  if (lMoon0 < lSun0)
    lMoon0 += 360;
  n_tithi = floor((lMoon0 - lSun0) / 12);
  // s_tithi = tithi(jd, n_tithi, tzone, 12);
  lMoon0 = lMoon;
  lSun0 = lSun;
  if (lMoon0 < lSun0)
    lMoon0 += 360;
  long nk = floor((lMoon0 - lSun0) / 6);
  if (nk == 0)
    n_karana = 10;
  if (nk >= 57)
    n_karana = nk - 50;
  if (nk > 0 && nk < 57)
    n_karana = (nk - 1) - (floor((nk - 1) / 7)) * 7;
  Serial.print("PCH, Karana:");
  Serial.println(n_karana);
  // s_karana = tithi(jd, nk, tzone, 6);
  return;
}

