      DOUBLE PRECISION FUNCTION DPMPAR(I)
      INTEGER I
C     **********
C
C     FUNCTION DPMPAR
C
C     THIS FUNCTION PROVIDES DOUBLE PRECISION MACHINE PARAMETERS
C     WHEN THE APPROPRIATE SET OF DATA STATEMENTS IS ACTIVATED (BY
C     REMOVING THE C FROM COLUMN 1) AND ALL OTHER DATA STATEMENTS ARE
C     RENDERED INACTIVE. MOST OF THE PARAMETER VALUES WERE OBTAINED
C     FROM THE CORRESPONDING BELL LABORATORIES PORT LIBRARY FUNCTION.
C
C     THE FUNCTION STATEMENT IS
C
C       DOUBLE PRECISION FUNCTION DPMPAR(I)
C
C     WHERE
C
C       I IS AN INTEGER INPUT VARIABLE SET TO 1, 2, OR 3 WHICH
C         SELECTS THE DESIRED MACHINE PARAMETER. IF THE MACHINE HAS
C         T BASE B DIGITS AND ITS SMALLEST AND LARGEST EXPONENTS ARE
C         EMIN AND EMAX, RESPECTIVELY, THEN THESE PARAMETERS ARE
C
C         DPMPAR(1) = B**(1 - T), THE MACHINE PRECISION,
C
C         DPMPAR(2) = B**(EMIN - 1), THE SMALLEST MAGNITUDE,
C
C         DPMPAR(3) = B**EMAX*(1 - B**(-T)), THE LARGEST MAGNITUDE.
C
C     ARGONNE NATIONAL LABORATORY. MINPACK PROJECT. JUNE 1983.
C     BURTON S. GARBOW, KENNETH E. HILLSTROM, JORGE J. MORE
C
C     Modified Mon Aug 28 14:46:17 CDT 1989 by John W. Eaton
C     (chpf127@emx.utexas.edu) to use D1MACH
C
C     **********
C
      DOUBLE PRECISION  D1MACH
C
      IF ( I .EQ. 1 ) THEN
        DPMPAR = D1MACH(4)
      ELSEIF ( I . EQ. 2 ) THEN
        DPMPAR = D1MACH(1)
      ELSEIF ( I .EQ. 3 ) THEN
        DPMPAR = D1MACH(2)
      ENDIF
C
      RETURN
C
C     LAST CARD OF FUNCTION DPMPAR.
C
      END
