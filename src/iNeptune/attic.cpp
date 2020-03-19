
// This function doesn't actually have any use in iNeptune regular code,
// But is useful for users when a XY list needs to be converted to LatLon
// and for debug.
string Neptune::PointsStrToLatLon(string pointsStr) {
  string newPoints = "{";
  string curPointString = biteStringX(pointsStr, ':');
  while (!curPointString.empty()) {
    double lat, lon, x, y;
    try {
      x = stod(biteStringX(curPointString, ','));
      y = stod(curPointString);
    } catch (invalid_argument &e) {
      reportRunWarning("Unable to parse latitude / longitude!");
      return "pts={}";
    }
    m_geo.UTM2LatLong(x, y, lat, lon);
    newPoints += doubleToStringX(lat) + "," + doubleToStringX(lon);
    curPointString = biteStringX(pointsStr, ':');
    if (!curPointString.empty()) newPoints += ":";
  }
  return newPoints + "}";
}
