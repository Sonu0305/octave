      DOUBLE PRECISION FUNCTION QUOTNT( U, V )
C
      DOUBLE PRECISION   U, V
C
      DOUBLE PRECISION   WMACH
      COMMON    /SOLMCH/ WMACH(15)
C
C  QUOTNT  RETURNS THE QUOTIENT  U / V,  GUARDING AGAINST OVERFLOW.
C  VERSION OF FEBRUARY 1983.
C
      DOUBLE PRECISION   ABSV, FLMAX, ZERO, ONE
      DOUBLE PRECISION   DABS
      DATA               ZERO, ONE / 0.0D+0, 1.0D+0 /
C
      FLMAX  = WMACH(7)
C
      ABSV   = DABS( V )
      IF (ABSV .GE. ONE) GO TO 100
      QUOTNT = FLMAX
      IF (V .EQ. ZERO  .AND.  U .LT. ZERO) QUOTNT = - FLMAX
      IF (DABS( U ) .GE. ABSV*FLMAX) GO TO 110
  100 QUOTNT = U / V
C
  110 RETURN
C
C  END OF QUOTNT
      END
