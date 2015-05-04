/*
 * Panchanga.h
 *
 *  Created on: Apr 11, 2015
 *      Author: DevbNJ
 */

#ifndef JOTIZ_PANCHANGA_H_
#define JOTIZ_PANCHANGA_H_

class Panchanga {
  public:
    static const double pi = 3.14159265359;
    static const double d2r = 3.14159265359 / 180;
    static const double r2d = 180 / 3.14159265359;
    char wd[];
    double lMoon, lMoon1, lSun, skar, lMoonYoga, lSunYoga;
    double ayanamsa;
    int n_wday, n_tithi, n_naksh, n_karana, n_yoga, panch, transitDate;
    char s_wday, s_tithi, s_naksh, s_karana, s_yoga;
    unsigned int kyear, kmonth, kday, khour, kminute, ksecond;

    double computeMoon( double jd );
    double computeSun(double jd);
    void initialize_panchanga(uint8_t dd, uint8_t mm, int yy, uint8_t hi, uint8_t mi, int tz);

  private:
    double fix360(double v);
    double nutation( double jd );
    double computeAyanamsa( double jd );
    double kepler(double m, double xe, double re);
    double mdy2julian(long m, long d, long y);
    double dTime (double jd);
    void calData(double jd);
    unsigned int tithi (double jd, long n1, double tzone, int len);
    double novolun (double jd, long knv);
};

#endif /* JOTIZ_PANCHANGA_H_ */
