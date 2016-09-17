/**
 * From http://forum.arduino.cc/index.php?topic=368720.0
 */
char *dtostrf(double value, int width, unsigned int precision, char *result) {
	int decpt, sign, reqd, pad;
	const char *s, *e;
	char *p;
	s = fcvt(value, precision, &decpt, &sign);
	if (precision == 0 && decpt == 0) {
		s = (*s < '5') ? "0" : "1";
		reqd = 1;
	} else {
		reqd = strlen(s);
		if (reqd > decpt)
			reqd++;
		if (decpt == 0)
			reqd++;
	}
	if (sign)
		reqd++;
	p = result;
	e = p + reqd;
	pad = width - reqd;
	if (pad > 0) {
		e += pad;
		while (pad-- > 0)
			*p++ = ' ';
	}
	if (sign)
		*p++ = '-';
	if (decpt <= 0 && precision > 0) {
		*p++ = '0';
		*p++ = '.';
		e++;
		while (decpt < 0) {
			decpt++;
			*p++ = '0';
		}
	}
	while (p < e) {
		*p++ = *s++;
		if (p == e)
			break;
		if (--decpt == 0)
			*p++ = '.';
	}
	if (width < 0) {
		pad = (reqd + width) * -1;
		while (pad-- > 0)
			*p++ = ' ';
	}
	*p = 0;
	return result;
}